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
// Created On : 2010-10-28 13:28:14
//
// $Id$
//
//***************************************************************************

#ifndef __QBATTERY_H__
#define __QBATTERY_H__

#include "stable.h"


//---------------------------------------------------------------------------
// QBattery
//
// A simple class to get battery level and status without depending on the
// Qt Mobility library. Note that this class is not Maemo specific since the
// involved DBus calls are FreeDesktop compliant.
//
// Reference :
// * Creating Power Aware Applications on Linux using Qt
//   http://software.intel.com/en-us/articles/creating-power-aware-applications-on-linux-using-qt4/
// * FreeDesktop's HAL specifications :
//   http://people.freedesktop.org/~dkukawka/hal-spec-git/0.5.13/hal-spec.html
// * Maemo Wiki :
//   http://wiki.maemo.org/Desktop_Command_Execution_Widget_scripts#Battery
// * Console tests :
//   # hal-device bme
//   # dbus-send --system --print-reply --dest=org.freedesktop.Hal /org/freedesktop/Hal/Manager org.freedesktop.Hal.Manager.FindDeviceByCapability string:'battery'
//   On the N900, this second command returns only one device named
//   "/org/freedesktop/Hal/devices/bme".
//---------------------------------------------------------------------------
class QBattery : public QObject
{
  Q_OBJECT

public :
  struct Property
  {
    bool    bPresent;
    time_t  uiLastUpdate;
    time_t  uiLastChange;
    QString strValue;

    Property (void) : bPresent(false), uiLastUpdate(0), uiLastChange(0) { }
  };

  struct Device
  {
    QDBusInterface*        pIf;
    QObject*               pListener;
    QMap<QString,Property> mapProps;
  };

public :
  QBattery (QObject* pParent=0);
  ~QBattery (void);

  bool isInit      (void) const;
  uint count       (void) const;
  void pollDevices (void);


public :
  // for internal use only
  void propertyChanged (const QString& strDevPath, const QString& strPropName, bool bPropAdded, bool bPropRemoved);


private :
  bool discoverBatteries (void);
  void pollDevice        (Device& dev);
  bool pollProperty      (Device& dev, const QString& strPropName, Property& prop);

  Device*   deviceByPath   (const QString& strPath);
  Property* propertyByName (const QString& strDevPath, const QString& strPropName, QBattery::Device** ppOutDevice=0);


private :
  QDBusConnection m_Conn;
  QDBusInterface  m_Hal;
  QVector<Device> m_Devs;
};


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
class QBatteryDBusListener : public QObject
{
  Q_OBJECT

public :
  QBatteryDBusListener (QBattery* pParent, int iDevIndex, const QString& strDevPath);

public slots :
  void listen (int iChangesCount, QList<QBatteryDBusProperty> prop);

private :
  int     m_iDevIndex;
  QString m_strDevPath;
};


#endif // #ifndef __QBATTERY_H__
