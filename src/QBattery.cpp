//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex@gmail.com>
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
// Created: 2010-10-28 13:28:19
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

static const struct
{
  QBattery::PropId eId;
  bool             bStandard;
  const char*      pszName;
}
PROPSNAMES[] =
{
  // standard properties
  { QBattery::PROP_ISPRESENT,             true,  "battery.present" },
  { QBattery::PROP_ISRECHARGEABLE,        true,  "battery.is_rechargeable" },
  { QBattery::PROP_ISCHARGING,            true,  "battery.rechargeable.is_charging" },
  { QBattery::PROP_CHARGE_UNIT,           true,  "battery.reporting.unit" },
  { QBattery::PROP_CHARGE_CURRENT,        true,  "battery.reporting.current" },
  { QBattery::PROP_CHARGE_FULL_DESIGN,    true,  "battery.reporting.design" },
  { QBattery::PROP_CHARGE_FULL_LASTKNOWN, true,  "battery.reporting.last_full" },

  // non-standard properties
  { QBattery::PROP_CHARGE_FULL,           false, 0 },
  { QBattery::PROP_CHARGE_PERCENT,        false, 0 },
  { QBattery::PROP_CHARGE_CHARGESTR,      false, 0 },
};

static const int PROPSNAMES_COUNT = sizeof(PROPSNAMES) / sizeof(PROPSNAMES[0]);



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
QBatteryDBusListener::QBatteryDBusListener (QBattery* pParent, const QString& strDevPath)
: QObject(pParent)
, m_strDevPath(strDevPath)
{
}

void QBatteryDBusListener::listen (int iChangesCount, QList<QBatteryDBusProperty> vecModProps)
{
  QBattery* pParent = static_cast<QBattery*>(this->parent());
  pParent->onDBusPropertyModified(m_strDevPath, vecModProps);
}



//---------------------------------------------------------------------------
// QBattery::Property
//---------------------------------------------------------------------------
bool QBattery::Property::setValue (const QString& strNewValue)
{
  time_t now = time(0);
  bool bChanged = false;

  if (!this->bPresent || this->strValue != strNewValue)
  {
    this->strValue     = strNewValue;
    this->uiLastChange = now;
    bChanged = true;
  }

  this->bPresent     = true;
  this->uiLastUpdate = now;

  return bChanged;
}



//---------------------------------------------------------------------------
// QBattery
//---------------------------------------------------------------------------
QBattery::QBattery (QObject* pParent/*=0*/)
: QObject(pParent)
, m_Conn(QDBusConnection::systemBus())
, m_Hal(BATT_DBUS_SERVICE, BATT_DBUS_PATH_MGR, BATT_DBUS_IF_MGR, m_Conn)
, m_bSignalingChanges(false)
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
// devicesCount
//---------------------------------------------------------------------------
uint QBattery::devicesCount (void) const
{
  return m_Devs.size();
}

//---------------------------------------------------------------------------
// pollDevices
//---------------------------------------------------------------------------
void QBattery::pollDevices (void)
{
  if (m_bSignalingChanges)
    return;

  for (int i = 0; i < m_Devs.size(); ++i)
    this->pollDeviceProperties(m_Devs[i]);
}



//---------------------------------------------------------------------------
// propAsBool
//---------------------------------------------------------------------------
QPair<bool,bool> QBattery::propAsBool (uint uiDevIndex, QBattery::PropId ePropId)
{
  Property* pProp = (uiDevIndex < m_Devs.size()) ? m_Devs[uiDevIndex].propertyById(ePropId) : 0;
  if (pProp && pProp->isValid())
    return QPair<bool,bool>(true, pProp->toBool());

  return QPair<bool,bool>(false, false);
}

//---------------------------------------------------------------------------
// propAsInt
//---------------------------------------------------------------------------
QPair<bool,int> QBattery::propAsInt (uint uiDevIndex, QBattery::PropId ePropId)
{
  Property* pProp = (uiDevIndex < m_Devs.size()) ? m_Devs[uiDevIndex].propertyById(ePropId) : 0;
  if (pProp && pProp->isValid())
    return QPair<bool,int>(true, pProp->toInt());

  return QPair<bool,int>(false, 0);
}

//---------------------------------------------------------------------------
// propAsStr
//---------------------------------------------------------------------------
QPair<bool,QString> QBattery::propAsStr (uint uiDevIndex, QBattery::PropId ePropId)
{
  Property* pProp = (uiDevIndex < m_Devs.size()) ? m_Devs[uiDevIndex].propertyById(ePropId) : 0;
  if (pProp && pProp->isValid())
    return QPair<bool,QString>(true, pProp->strValue);

  return QPair<bool,QString>(false, QString());
}



//---------------------------------------------------------------------------
// discoverBatteries
//---------------------------------------------------------------------------
bool QBattery::discoverBatteries (void)
{
  if (!this->isInit())
  {
    qWarning("QBattery could not create DBus connection !");
    return false;
  }

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
    dev.pListener = new QBatteryDBusListener(this, strDev);

    //qDebug("New batt device %s (valid:%i).", qPrintable(strDev), dev.pIf->isValid()?1:0);
    if (!dev.pIf->isValid())
      goto __SkipDevice;

    // declare properties
    for (int i = 0; i < PROPSNAMES_COUNT; ++i)
    {
      Property prop;

      prop.eId       = PROPSNAMES[i].eId;
      prop.bStandard = PROPSNAMES[i].bStandard;
      if (PROPSNAMES[i].bStandard && PROPSNAMES[i].pszName)
        prop.strName = PROPSNAMES[i].pszName;

      dev.mapProps.insert(PROPSNAMES[i].eId, prop);
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
    dev.uiIndex = m_Devs.size();
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
// pollDeviceProperties
//---------------------------------------------------------------------------
void QBattery::pollDeviceProperties (QBattery::Device& dev)
{
  // poll every known properties
  QMutableMapIterator<PropId,Property> itProp(dev.mapProps);
  while (itProp.hasNext())
  {
    itProp.next();
    if (itProp.value().bStandard)
      this->pollProperty(dev, itProp.value());
  }

  // signal listeners if needed
  this->signalChanges();
}

//---------------------------------------------------------------------------
// pollProperty
//---------------------------------------------------------------------------
bool QBattery::pollProperty (QBattery::Device& dev, QBattery::Property& prop)
{
  QDBusMessage msg;
  QVariant var;

  msg = dev.pIf->call("GetProperty", prop.strName);
  var = msg.arguments()[0];

  if (msg.type() == QDBusMessage::ReplyMessage)
  {
    if (prop.setValue(var.toString()))
      this->onPropertyChanged(dev, prop);
    return true;
  }

  return false;
}



//---------------------------------------------------------------------------
// onDBusPropertyModified
//---------------------------------------------------------------------------
void QBattery::onDBusPropertyModified (const QString& strDevPath, QList<QBatteryDBusProperty>& vecModProps)
{
  Device*   pDevice = this->deviceByPath(strDevPath);
  PropId    ePropId;
  Property* pProp;

  if (!pDevice)
    return;

  // poll every modified properties
  QListIterator<QBatteryDBusProperty> itModProp(vecModProps);
  while (itModProp.hasNext())
  {
    const QBatteryDBusProperty& dbusProp = itModProp.next();

    // convert property's name to id
    ePropId = PROP_INVALID;
    for (int i = 0; i < PROPSNAMES_COUNT; ++i)
    {
      if (PROPSNAMES[i].bStandard && dbusProp.strName == PROPSNAMES[i].pszName)
      {
        ePropId = PROPSNAMES[i].eId;
        break;
      }
    }
    if (ePropId == PROP_INVALID)
      continue;

    // get pointer to our local property structure
    pProp = pDevice->propertyById(ePropId);
    if (!pProp)
      continue;

    if (dbusProp.bAdded)
      pProp->bPresent = true;
    else if (dbusProp.bRemoved)
      pProp->bPresent = false;
    else
      this->pollProperty(*pDevice, *pProp);
  }

  // now that all modified properties has been polled,
  // signal listeners about any change if needed
  this->signalChanges();
}

//---------------------------------------------------------------------------
// onPropertyChanged
//---------------------------------------------------------------------------
void QBattery::onPropertyChanged (Device& dev, QBattery::Property& prop)
{
  if (!m_mapChanges[dev.uiIndex].contains(prop.eId))
    m_mapChanges[dev.uiIndex].append(prop.eId);

  // trigger some specific changes to apply them on related properties
  switch (prop.eId)
  {
    // do not put non-standard properties in thoses cases or you will end-up
    // with an infinite loop
    case PROP_ISCHARGING :
    case PROP_CHARGE_CURRENT :
    case PROP_CHARGE_FULL_DESIGN :
    case PROP_CHARGE_FULL_LASTKNOWN :
      this->onPropertyChanged_Charge(dev);
      break;
  }
}

//---------------------------------------------------------------------------
// onPropertyChanged_Charge
//---------------------------------------------------------------------------
void QBattery::onPropertyChanged_Charge (QBattery::Device& dev)
{
  Property* pPropFullDesign = dev.propertyById(PROP_CHARGE_FULL_DESIGN);
  Property* pPropFullLast   = dev.propertyById(PROP_CHARGE_FULL_LASTKNOWN);
  Property* pPropFull       = dev.propertyById(PROP_CHARGE_FULL);
  Property* pPropCurr       = dev.propertyById(PROP_CHARGE_CURRENT);
  Property* pPropPercent    = dev.propertyById(PROP_CHARGE_PERCENT);
  Property* pPropIsCharging = dev.propertyById(PROP_ISCHARGING);
  Property* pPropChargeStr  = dev.propertyById(PROP_CHARGE_CHARGESTR);

  // PROP_CHARGE_FULL
  if (pPropFull)
  {
    bool bChanged = false;

    // actually, we never can use PROP_CHARGE_FULL_LASTKNOWN in place of
    // PROP_CHARGE_FULL_DESIGN because sometimes, PROP_CHARGE_FULL_LASTKNOWN
    // property is smaller than CURRENT. this is bad...

    //if (pPropFullLast && pPropFullLast->isValid())
    //  bChanged = pPropFull->setValue(pPropFullLast->strValue);
    //else
    if (pPropFullDesign && pPropFullDesign->isValid())
      bChanged = pPropFull->setValue(pPropFullDesign->strValue);

    if (bChanged)
      this->onPropertyChanged(dev, *pPropFull);
  }

  // PROP_CHARGE_PERCENT
  if (pPropPercent &&
      pPropCurr && pPropCurr->isValid() &&
      pPropFull && pPropFull->isValid())
  {
    int iPercent = int( double(pPropCurr->toInt()) * 100.0 / double(pPropFull->toInt()) );

    //qDebug("curr:%i full:%i design:%i last:%i percent:%i", pPropCurr->toInt(), pPropFull->toInt(), pPropFullDesign->toInt(), pPropFullLast->toInt(), iPercent);
    if (pPropPercent->setValue(QString::number(iPercent)))
      this->onPropertyChanged(dev, *pPropPercent);
  }

  // PROP_CHARGE_CHARGESTR
  if (pPropChargeStr)
  {
    QString strCharge;

    if (pPropPercent && pPropPercent->isValid())
    {
      strCharge = QString::number(pPropPercent->toInt()) + "%";
      if (pPropIsCharging && pPropIsCharging->isValid() && pPropIsCharging->toBool())
        strCharge += tr(" (charging)");
    }
    else if (pPropIsCharging && pPropIsCharging->isValid() && pPropIsCharging->toBool())
    {
      strCharge = tr("Charging");
    }
    else
    {
      strCharge = tr("N/A");
    }

    if (pPropChargeStr->setValue(strCharge))
      this->onPropertyChanged(dev, *pPropChargeStr);
  }
}

//---------------------------------------------------------------------------
// signalChanges
//---------------------------------------------------------------------------
void QBattery::signalChanges (void)
{
  m_bSignalingChanges = true;

  QMapIterator<uint,QVector<PropId> > itChange(m_mapChanges);
  while (itChange.hasNext())
  {
    itChange.next();
    this->sigPropertiesChanged(itChange.key(), itChange.value());
  }

  m_mapChanges.clear();
  m_bSignalingChanges = false;
}



//---------------------------------------------------------------------------
// deviceByPath
//---------------------------------------------------------------------------
QBattery::Device* QBattery::deviceByPath (const QString& strDevPath)
{
  for (int i = 0; i < m_Devs.size(); ++i)
  {
    if (m_Devs[i].pIf->path() == strDevPath)
      return &m_Devs[i];
  }

  return 0;
}
