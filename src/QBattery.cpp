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
// Created On : 2010-10-28 13:28:19
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
static const char* BATT_DBUS_SERVICE          = "org.freedesktop.Hal";
static const char* BATT_DBUS_PATH_MGR         = "/org/freedesktop/Hal/Manager";
static const char* BATT_DBUS_IF_MGR           = "org.freedesktop.Hal.Manager";
static const char* BATT_DBUS_IF_DEV           = "org.freedesktop.Hal.Device";
static const char* BATT_DBUS_CMD_FINDDEVBYCAP = "FindDeviceByCapability";



//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------
static bool g_bFirstInit = true;



//---------------------------------------------------------------------------
// QBatteryDBusProperty
//---------------------------------------------------------------------------
const QDBusArgument& operator<< (QDBusArgument& arg, const QBatteryDBusProperty& prop)
{
  arg.beginStructure();
  arg << prop.strName << prop.bAdded << prop.bRemoved;
  arg.endStructure();
  return arg;
}

const QDBusArgument& operator>> (const QDBusArgument& arg, QBatteryDBusProperty& prop)
{
  arg.beginStructure();
  arg >> prop.strName >> prop.bAdded >> prop.bRemoved;
  arg.endStructure();
  return arg;
}



//---------------------------------------------------------------------------
// QBatteryDBusListener
//---------------------------------------------------------------------------
QBatteryDBusListener::QBatteryDBusListener (QBattery* pParent, int iDevIndex, const QString& strDevPath)
: QObject(pParent)
, m_iDevIndex(iDevIndex)
, m_strDevPath(strDevPath)
{
}

void QBatteryDBusListener::listen (int iChangesCount, QList<QBatteryDBusProperty> prop)
{
  QBattery* pParent = static_cast<QBattery*>(this->parent());
  for (int i = 0; i < prop.size(); ++i)
    pParent->propertyChanged(m_strDevPath, prop[i].strName, prop[i].bAdded, prop[i].bRemoved);
}



//---------------------------------------------------------------------------
// QBattery
//---------------------------------------------------------------------------
QBattery::QBattery (QObject* pParent/*=0*/)
: QObject(pParent)
, m_Conn(QDBusConnection::systemBus())
, m_Hal(BATT_DBUS_SERVICE, BATT_DBUS_PATH_MGR, BATT_DBUS_IF_MGR, m_Conn)
{
  if (g_bFirstInit)
  {
    qDBusRegisterMetaType<QBatteryDBusProperty>();
    qDBusRegisterMetaType< QList<QBatteryDBusProperty> >();
    g_bFirstInit = false;
  }

  this->discoverBatteries();
}

//---------------------------------------------------------------------------
// ~QBattery
//---------------------------------------------------------------------------
QBattery::~QBattery (void)
{
  while (!m_Devs.isEmpty())
  {
    delete m_Devs.back().pIf;
    m_Devs.pop_back();
  }
}



//---------------------------------------------------------------------------
// isInit
//---------------------------------------------------------------------------
bool QBattery::isInit (void) const
{
  return m_Conn.isConnected() && m_Hal.isValid();
}

//---------------------------------------------------------------------------
// count
//---------------------------------------------------------------------------
uint QBattery::count (void) const
{
  return m_Devs.size();
}

//---------------------------------------------------------------------------
// pollDevices
//---------------------------------------------------------------------------
void QBattery::pollDevices (void)
{
  for (int i = 0; i < m_Devs.size(); ++i)
    this->pollDevice(m_Devs[i]);
}



//---------------------------------------------------------------------------
// propertyChanged
//---------------------------------------------------------------------------
void QBattery::propertyChanged (const QString& strDevPath, const QString& strPropName, bool bPropAdded, bool bPropRemoved)
{
  Device*   pDevice = 0;
  Property* pProp = this->propertyByName(strDevPath, strPropName, &pDevice);

  //qDebug("PropertyChanged [%s] [%s] added:%d removed:%d", qPrintable(strDevPath), qPrintable(strPropName), bPropAdded, bPropRemoved);
  if (!pProp)
    return;

  if (bPropAdded)
    pProp->bPresent = true;
  else if (bPropRemoved)
    pProp->bPresent = false;
  else
    this->pollProperty(*pDevice, strPropName, *pProp);
}



//---------------------------------------------------------------------------
// discoverBatteries
//---------------------------------------------------------------------------
bool QBattery::discoverBatteries (void)
{
  QStringList listPollProps;

  if (!this->isInit())
  {
    qWarning("QBattery could not create DBus connection !");
    return false;
  }

  // build list of properties to poll
  //
  // device example on nokia n900 :
  // # hal-device bme
  // udi = '/org/freedesktop/Hal/devices/bme'
  //   maemo.charger.type = 'host 500 mA'  (string)
  //   maemo.charger.connection_status = 'connected'  (string)
  //   maemo.rechargeable.charging_status = 'full'  (string)
  //   maemo.rechargeable.positive_rate = true  (bool)
  //   info.addons = { 'hald-addon-bme' } (string list)
  //   info.subsystem = 'unknown'  (string)
  //   info.product = 'Battery (BME-HAL)'  (string)
  //   info.udi = '/org/freedesktop/Hal/devices/bme'  (string)
  //   info.category = 'battery'  (string)
  //   info.parent = '/org/freedesktop/Hal/devices/computer'  (string)
  //   info.capabilities = { 'battery' } (string list)
  //   battery.present = true  (bool)
  //   battery.type = 'pda'  (string)
  //   battery.charge_level.capacity_state = 'full'  (string)
  //   battery.charge_level.design = 8  (0x8)  (int)
  //   battery.charge_level.current = 8  (0x8)  (int)
  //   battery.charge_level.unit = 'bars'  (string)
  //   battery.charge_level.last_full = 8  (0x8)  (int)
  //   battery.charge_level.percentage = 86  (0x56)  (int)
  //   battery.is_rechargeable = true  (bool)
  //   battery.rechargeable.is_charging = true  (bool)
  //   battery.rechargeable.is_discharging = true  (bool)
  //   battery.reporting.unit = 'mAh'  (string)
  //   battery.reporting.design = 1264  (0x4f0)  (int)
  //   battery.reporting.last_full = 1097  (0x449)  (int)
  //   battery.reporting.current = 1097  (0x449)  (int)
  //   battery.voltage.unit = 'mV'  (string)
  //   battery.voltage.design = 4200  (0x1068)  (int)
  //   battery.voltage.current = 4129  (0x1021)  (int)
  //   battery.remaining_time = 0  (0x0)  (int)
  //   battery.remaining_time.calculate_per_time = false  (bool)
  listPollProps <<
    "battery.present" <<
    //"battery.type" <<
    //"battery.charge_level.unit" <<
    //"battery.charge_level.design" <<
    //"battery.charge_level.last_full" <<
    //"battery.charge_level.current" <<
    //"battery.charge_level.rate" <<
    //"battery.charge_level.warning" <<
    //"battery.charge_level.low" <<
    //"battery.charge_level.granularity_1" <<
    //"battery.charge_level.granularity_2" <<
    "battery.reporting.unit" <<
    "battery.reporting.design" <<
    "battery.reporting.last_full" <<
    "battery.reporting.current" <<
    //"battery.reporting.rate" <<
    //"battery.reporting.warning" <<
    //"battery.reporting.low" <<
    //"battery.reporting.granularity_1" <<
    //"battery.reporting.granularity_2" <<
    "battery.charge_level.capacity_state" <<
    "battery.voltage.unit" <<
    "battery.voltage.design" <<
    "battery.voltage.current" <<
    //"battery.alarm.unit" <<
    //"battery.alarm.design" <<
    //"battery.remaining_time" <<
    //"battery.remaining_time.calculate_per_time" <<
    "battery.charge_level.percentage" <<
    "battery.is_rechargeable" <<
    "battery.rechargeable.is_charging" <<
    "battery.rechargeable.is_discharging" <<
    //"battery.command_interface" <<
    //"battery.vendor" <<
    //"battery.model" <<
    //"battery.reporting.technology" <<
    //"battery.technology" <<
    //"battery.serial" <<
    "";

  // search for battery devices
  QDBusMessage msg = m_Hal.call(BATT_DBUS_CMD_FINDDEVBYCAP, "battery");
  QList<QVariant> listDevs = msg.arguments();
  QListIterator<QVariant> itDev(listDevs);

  // for each found device
  while (itDev.hasNext())
  {
    QString strDev = itDev.next().toStringList()[0];
    Device  dev;
    bool    bRes;

    // init device structure
    dev.pIf = new QDBusInterface(
      BATT_DBUS_SERVICE,
      strDev,
      BATT_DBUS_IF_DEV,
      m_Conn);
    dev.pListener = new QBatteryDBusListener(this, m_Devs.size(), strDev);

    //qDebug("New batt device %s (valid:%i).", qPrintable(strDev), dev.pIf->isValid()?1:0);
    if (!dev.pIf->isValid())
      goto __SkipDevice;

    // fill-in desired properties
    for (int i = 0; i < listPollProps.size(); ++i)
    {
      if (!listPollProps[i].isEmpty())
        dev.mapProps.insert(listPollProps[i], Property());
    }

    // listen for any PropertyModified event from this device
    bRes = dev.pIf->connection().connect(
      BATT_DBUS_SERVICE,
      strDev,
      BATT_DBUS_IF_DEV,
      "PropertyModified",
      dev.pListener,
      SLOT(listen(int, QList<QBatteryDBusProperty>)));
    if (!bRes)
    {
      qWarning("Unable to listen PropertyModified signal from battery %s ! Error (%d) %s", qPrintable(strDev), dev.pIf->connection().lastError().type(), qPrintable(dev.pIf->connection().lastError().message()));
      goto __SkipDevice;
    }

    // append device structure to our list
    m_Devs.append(dev);

    continue;

    __SkipDevice :
    qWarning("Skipping battery %s !", qPrintable(strDev));
    delete dev.pListener;
    delete dev.pIf;
  }

  if (!m_Devs.isEmpty())
    this->pollDevices();

  return true;
}

//---------------------------------------------------------------------------
// pollDevice
//---------------------------------------------------------------------------
void QBattery::pollDevice (QBattery::Device& dev)
{
  QMutableMapIterator<QString,Property> itProp(dev.mapProps);
  while (itProp.hasNext())
  {
    itProp.next();
    this->pollProperty(dev, itProp.key(), itProp.value());
  }
}

//---------------------------------------------------------------------------
// pollProperty
//---------------------------------------------------------------------------
bool QBattery::pollProperty (QBattery::Device& dev, const QString& strPropName, QBattery::Property& prop)
{
  QDBusMessage msg;
  QVariant var;

  msg = dev.pIf->call("GetProperty", strPropName);
  var = msg.arguments()[0];

  /*qDebug() << "*** GetProperty " << strPropName;
  switch (msg.type())
  {
    case QDBusMessage::MethodCallMessage:
      qDebug() << "Method Call Return: " << msg;
      break;
    case QDBusMessage::ReplyMessage:
      qDebug() << "Reply: " << strPropName << ": " << var.toString();
      break;
    case QDBusMessage::ErrorMessage:
      qDebug() << "Error Message: " << msg.errorMessage();
      break;
    case QDBusMessage::InvalidMessage:
      qDebug() << "Invalid Message: " << msg;
      break;
    case QDBusMessage::SignalMessage:
      qDebug() << "Signal Message: " << msg;
      break;
  }*/

  if (msg.type() == QDBusMessage::ReplyMessage)
  {
    time_t now = time(0);
    bool bChanged = false;

    if (!prop.bPresent || prop.strValue != var.toString())
    {
      prop.uiLastChange = now;
      bChanged = true;
    }

    prop.bPresent     = true;
    prop.uiLastUpdate = now;
    prop.strValue     = var.toString();

    //if (bChanged)
    //  qDebug("Battery property has changed : %s %s [%s]", qPrintable(dev.pIf->path()), qPrintable(strPropName), qPrintable(prop.strValue));

    return true;
  }

  return false;
}

//---------------------------------------------------------------------------
// deviceByPath
//---------------------------------------------------------------------------
QBattery::Device* QBattery::deviceByPath (const QString& strPath)
{
  for (int i = 0; i < m_Devs.size(); ++i)
  {
    if (m_Devs[i].pIf->path() == strPath)
      return &m_Devs[i];
  }

  return 0;
}

//---------------------------------------------------------------------------
// propertyByName
//---------------------------------------------------------------------------
QBattery::Property* QBattery::propertyByName (const QString& strDevPath, const QString& strPropName, QBattery::Device** ppOutDevice/*=0*/)
{
  Device*   pDevice = this->deviceByPath(strDevPath);
  Property* pProp = 0;

  if (pDevice && pDevice->mapProps.contains(strPropName))
    pProp = &pDevice->mapProps.find(strPropName).value();

  if (ppOutDevice)
    *ppOutDevice = pDevice;

  return pProp;
}
