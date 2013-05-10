//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex+gpsrecorder [AT] gmail [DOT] com>
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
// Created On : 2010-10-28 13:28:14
//
// $Id$
//
//***************************************************************************

#ifndef __QBATTERY_H__
#define __QBATTERY_H__

#include "stable.h"


//---------------------------------------------------------------------------
// QBatteryDBusProperty
//---------------------------------------------------------------------------
struct QBatteryDBusProperty
{
  QString strName;
  bool    bAdded;
  bool    bRemoved;
};

Q_DECLARE_METATYPE(QBatteryDBusProperty);
Q_DECLARE_METATYPE(QList<QBatteryDBusProperty>);


//---------------------------------------------------------------------------
// QBatteryDBusListener
//---------------------------------------------------------------------------
class QBattery;
class QBatteryDBusListener : public QObject
{
  Q_OBJECT

public :
  QBatteryDBusListener (QBattery* pParent, const QString& strDevPath);

public slots :
  void listen (int iChangesCount, QList<QBatteryDBusProperty> vecModProps);

private :
  int     m_iDevIndex;
  QString m_strDevPath;
};


//---------------------------------------------------------------------------
// QBattery
//
// A class to get battery level and status without depending on the Qt
// Mobility library. Note that this class is not Maemo specific since the
// involved DBus calls are FreeDesktop compliant.
//
// References :
// * Creating Power Aware Applications on Linux using Qt
//   http://software.intel.com/en-us/articles/creating-power-aware-applications-on-linux-using-qt4/
// * FreeDesktop's HAL specifications :
//   http://people.freedesktop.org/~dkukawka/hal-spec-git/0.5.13/hal-spec.html
// * Maemo Wiki :
//   http://wiki.maemo.org/Desktop_Command_Execution_Widget_scripts#Battery
// * Console tests on Nokia N900 :
//   # hal-device bme
//   udi = '/org/freedesktop/Hal/devices/bme'
//     maemo.charger.type = 'host 500 mA'  (string)
//     maemo.charger.connection_status = 'connected'  (string)
//     maemo.rechargeable.charging_status = 'full'  (string)
//     maemo.rechargeable.positive_rate = true  (bool)
//     info.addons = { 'hald-addon-bme' } (string list)
//     info.subsystem = 'unknown'  (string)
//     info.product = 'Battery (BME-HAL)'  (string)
//     info.udi = '/org/freedesktop/Hal/devices/bme'  (string)
//     info.category = 'battery'  (string)
//     info.parent = '/org/freedesktop/Hal/devices/computer'  (string)
//     info.capabilities = { 'battery' } (string list)
//     battery.present = true  (bool)
//     battery.type = 'pda'  (string)
//     battery.charge_level.capacity_state = 'full'  (string)
//     battery.charge_level.design = 8  (0x8)  (int)
//     battery.charge_level.current = 8  (0x8)  (int)
//     battery.charge_level.unit = 'bars'  (string)
//     battery.charge_level.last_full = 8  (0x8)  (int)
//     battery.charge_level.percentage = 86  (0x56)  (int)
//     battery.is_rechargeable = true  (bool)
//     battery.rechargeable.is_charging = true  (bool)
//     battery.rechargeable.is_discharging = true  (bool)
//     battery.reporting.unit = 'mAh'  (string)
//     battery.reporting.design = 1264  (0x4f0)  (int)
//     battery.reporting.last_full = 1097  (0x449)  (int)
//     battery.reporting.current = 1097  (0x449)  (int)
//     battery.voltage.unit = 'mV'  (string)
//     battery.voltage.design = 4200  (0x1068)  (int)
//     battery.voltage.current = 4129  (0x1021)  (int)
//     battery.remaining_time = 0  (0x0)  (int)
//     battery.remaining_time.calculate_per_time = false  (bool)
//   # dbus-send --system --print-reply --dest=org.freedesktop.Hal /org/freedesktop/Hal/Manager org.freedesktop.Hal.Manager.FindDeviceByCapability string:'battery'
//   On the N900, this second command returns only one device named
//   "/org/freedesktop/Hal/devices/bme".
//---------------------------------------------------------------------------
class QBattery : public QObject
{
  Q_OBJECT

public :
  enum PropId
  {
    // CAUTION : do not forget to modify the PROPSNAMES structure in source
    // file according to your changes here !

    PROP_INVALID = 0,

    // standard properties
    PROP_ISPRESENT = 1,          // bool "battery.present"
    PROP_ISRECHARGEABLE,         // bool "battery.is_rechargeable"
    PROP_ISCHARGING,             // bool "battery.rechargeable.is_charging"
    PROP_CHARGE_UNIT,            // string "battery.reporting.unit"
    PROP_CHARGE_CURRENT,         // int "battery.reporting.current"
    PROP_CHARGE_FULL_DESIGN,     // int "battery.reporting.design"
    PROP_CHARGE_FULL_LASTKNOWN,  // int "battery.reporting.last_full"

    // properties added for convenience
    PROP_CHARGE_FULL,            // int ; "battery.reporting.last_full" or by default "battery.reporting.design"
    PROP_CHARGE_PERCENT,         // int ; PROP_CHARGE_CURRENT * 100 / PROP_CHARGE_FULL
    PROP_CHARGE_CHARGESTR,       // string "%" or "% (charging)"
  };

  struct Property
  {
    PropId  eId;
    bool    bStandard;
    bool    bPresent;
    time_t  uiLastUpdate;
    time_t  uiLastChange;
    QString strName;
    QString strValue;

    Property (void) : bStandard(false), bPresent(false), uiLastUpdate(0), uiLastChange(0) { }

    bool setValue (const QString& strNewValue);

    bool isValid (void) { return bPresent && uiLastUpdate && !strValue.isEmpty(); }
    bool toBool  (void) { return QVariant(strValue).toBool(); }
    int  toInt   (void) { return QVariant(strValue).toInt(); }
  };

  struct Device
  {
    uint                  uiIndex;
    QDBusInterface*       pIf;
    QObject*              pListener;
    QMap<PropId,Property> mapProps;

    Property* propertyById (PropId ePropId)
    {
      return
        mapProps.contains(ePropId) ?
        &mapProps.find(ePropId).value() :
        0;
    }
  };

public :
  QBattery (QObject* pParent=0);
  ~QBattery (void);

  // if this one returns false, you cannot use this class...
  bool isInit (void) const;

  // number of discovered battery devices
  // discovery is done by the constructor
  uint devicesCount (void) const;

  // force poll all properties of all discovered batteries now instead of
  // waiting for next update. note that a poll is performed once from the
  // constructor.
  void pollDevices (void);

  // properties
  QPair<bool,bool>    propAsBool (uint uiDevIndex, PropId ePropId);
  QPair<bool,int>     propAsInt  (uint uiDevIndex, PropId ePropId);
  QPair<bool,QString> propAsStr  (uint uiDevIndex, PropId ePropId);


signals :
  void sigPropertiesChanged (uint uiDevIndex, const QVector<QBattery::PropId>& vecChangedProps);


private :
  bool discoverBatteries    (void);
  void pollDeviceProperties (Device& dev);
  bool pollProperty         (Device& dev, Property& prop);

  friend class QBatteryDBusListener;
  void onDBusPropertyModified (const QString& strDevPath, QList<QBatteryDBusProperty>& vecModProps);

  void onPropertyChanged        (Device& dev, Property& prop);
  void onPropertyChanged_Charge (Device& dev);

  void signalChanges (void);

  Device* deviceByPath (const QString& strDevPath);


private :
  QDBusConnection m_Conn;
  QDBusInterface  m_Hal;
  QVector<Device> m_Devs;

  QMap<uint,QVector<PropId> > m_mapChanges;
  bool m_bSignalingChanges;
};


Q_DECLARE_METATYPE(QBattery::PropId);
Q_DECLARE_METATYPE(QVector<QBattery::PropId>);


#endif // #ifndef __QBATTERY_H__
