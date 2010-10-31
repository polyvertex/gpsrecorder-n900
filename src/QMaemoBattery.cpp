//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-10-31 18:32:04
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
static const char* BATT_DBUS_SERVICE  = "org.freedesktop.Hal";
static const char* BATT_DBUS_PATH_MGR = "/org/freedesktop/Hal/Manager";
static const char* BATT_DBUS_IF_MGR   = "org.freedesktop.Hal.Manager";
static const char* BATT_DBUS_IF_DEV   = "org.freedesktop.Hal.Device";

static const char* BATT_DBUS_PROP_ISCHARGING     = "battery.rechargeable.is_charging";
static const char* BATT_DBUS_PROP_CHARGECURRENT  = "battery.reporting.current";
static const char* BATT_DBUS_PROP_CHARGELASTFULL = "battery.charge_level.last_full";
static const char* BATT_DBUS_PROP_CHARGEDESIGN   = "battery.reporting.design";



//---------------------------------------------------------------------------
// QMaemoBatteryDBusProperty
//---------------------------------------------------------------------------
const QDBusArgument& operator<< (QDBusArgument& arg, const QMaemoBatteryDBusProperty& prop)
{
  arg.beginStructure();
  arg << prop.strName << prop.bAdded << prop.bRemoved;
  arg.endStructure();
  return arg;
}

const QDBusArgument& operator>> (const QDBusArgument& arg, QMaemoBatteryDBusProperty& prop)
{
  arg.beginStructure();
  arg >> prop.strName >> prop.bAdded >> prop.bRemoved;
  arg.endStructure();
  return arg;
}



//---------------------------------------------------------------------------
// QMaemoBattery
//---------------------------------------------------------------------------
QMaemoBattery::QMaemoBattery (QObject* pParent/*=0*/)
: QObject(pParent)
, m_Conn(QDBusConnection::systemBus())
, m_Hal(BATT_DBUS_SERVICE, BATT_DBUS_PATH_MGR, BATT_DBUS_IF_MGR, m_Conn)
, m_pDev(0)
{
  static bool bFirstInit = true;

  if (bFirstInit)
  {
    qDBusRegisterMetaType<QMaemoBatteryDBusProperty>();
    qDBusRegisterMetaType< QList<QMaemoBatteryDBusProperty> >();
    bFirstInit = false;
  }

  if (this->isValid())
    this->discoverBattery();
  else
    qWarning("QMaemoBattery could not create DBus connection !");
}

//---------------------------------------------------------------------------
// ~QMaemoBattery
//---------------------------------------------------------------------------
QMaemoBattery::~QMaemoBattery (void)
{
  if (m_pDev)
    delete m_pDev;
}



//---------------------------------------------------------------------------
// isValid
//---------------------------------------------------------------------------
bool QMaemoBattery::isValid (void) const
{
  return m_Conn.isConnected() && m_Hal.isValid();
}

//---------------------------------------------------------------------------
// pollBattery
//---------------------------------------------------------------------------
void QMaemoBattery::pollBattery (void)
{
  QStringList   propNames;
  QSet<QString> propNamesPolled;
  QDBusMessage  msg;
  QVariant      var;

  if (!this->isValid() || !m_pDev)
    return;

  // properties we want to poll
  propNames <<
    BATT_DBUS_PROP_ISCHARGING <<
    BATT_DBUS_PROP_CHARGECURRENT <<
    BATT_DBUS_PROP_CHARGELASTFULL <<
    BATT_DBUS_PROP_CHARGEDESIGN;

  // poll properties one by one
  for (int i = 0; i < propNames.size(); ++i)
  {
    msg = m_pDev->call("GetProperty", propNames[i]);
    if (msg.type() != QDBusMessage::ReplyMessage)
      break; // failed

    var = msg.arguments()[0];

    if (propNames[i] == BATT_DBUS_PROP_ISCHARGING)
    {
      m_bCharging = var.toBool();
      propNamesPolled.insert(propNames[i]);
    }
    else if (propNames[i] == BATT_DBUS_PROP_CHARGECURRENT)
    {
      m_iChargeCurrent = var.toInt();
      propNamesPolled.insert(propNames[i]);
    }
    else if (propNames[i] == BATT_DBUS_PROP_CHARGELASTFULL)
    {
      m_iChargeLastFull = var.toInt();
      propNamesPolled.insert(propNames[i]);
    }
    else if (propNames[i] == BATT_DBUS_PROP_CHARGEDESIGN)
    {
      m_iChargeDesign = var.toInt();
      propNamesPolled.insert(propNames[i]);
    }
  }

  // if we've got all required value, signal clients
  if (propNamesPolled.size() == propNames.size())
    this->sigBatteryStatusChanged(m_iChargeCurrent, m_iChargeLastFull, m_iChargeDesign, m_bCharging);
}



//---------------------------------------------------------------------------
// onDBusPropertyModified
//---------------------------------------------------------------------------
void QMaemoBattery::onDBusPropertyModified (int iCount, QList<QMaemoBatteryDBusProperty> vecModProps)
{
  QStringList propNames;

  QListIterator<QMaemoBatteryDBusProperty> itModProp(vecModProps);
  while (itModProp.hasNext())
  {
    const QMaemoBatteryDBusProperty& dbusProp = itModProp.next();
    if (!dbusProp.bAdded && !dbusProp.bRemoved)
      propNames.append(dbusProp.strName);
  }

  if (!propNames.isEmpty())
    this->pollBattery();
}



//---------------------------------------------------------------------------
// discoverBattery
//---------------------------------------------------------------------------
void QMaemoBattery::discoverBattery (void)
{
  Q_ASSERT(!m_pDev);
  if (m_pDev)
    return;

  // find battery device
  // we care only about the first found battery device
  {
    QDBusMessage msg = m_Hal.call("FindDeviceByCapability", "battery");
    QList<QVariant> listDevs = msg.arguments();
    if (listDevs.isEmpty())
      return;

    m_strDevPath = listDevs[0].toString();
  }

  // connect to the battery
  m_pDev = new QDBusInterface(
    BATT_DBUS_SERVICE,
    m_strDevPath,
    BATT_DBUS_IF_DEV,
    m_Conn);

  // listen for any modification made on battery's properties
  m_pDev->connection().connect(
    BATT_DBUS_SERVICE,
    m_strDevPath,
    BATT_DBUS_IF_DEV,
    "PropertyModified",
    this,
    SLOT(onDBusPropertyModified(int, QList<QMaemoBatteryDBusProperty>)));

  // poll battery properties
  this->pollBattery();
}
