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
// Created: 2010-10-31 18:32:01
//
//***************************************************************************

#ifndef __QMAEMOBATTERY_H__
#define __QMAEMOBATTERY_H__

#include "stable.h"


//---------------------------------------------------------------------------
// QMaemoBatteryDBusProperty
//---------------------------------------------------------------------------
struct QMaemoBatteryDBusProperty
{
  QString strName;
  bool    bAdded;
  bool    bRemoved;
};

Q_DECLARE_METATYPE(QMaemoBatteryDBusProperty);
Q_DECLARE_METATYPE(QList<QMaemoBatteryDBusProperty>);


//---------------------------------------------------------------------------
// QMaemoBattery
//---------------------------------------------------------------------------
class QMaemoBattery : public QObject
{
  Q_OBJECT

public :
  QMaemoBattery (QObject* pParent=0);
  ~QMaemoBattery (void);

  bool isValid     (void) const;
  void pollBattery (void);


signals :
  void sigBatteryStatusChanged (int iCurrent, int iLastFull, int iDesign, bool bCharging);


private slots :
  void onDBusPropertyModified (int iCount, QList<QMaemoBatteryDBusProperty> vecModProps);


private :
  void discoverBattery (void);


private :
  // dbus connection
  QDBusConnection m_Conn;
  QDBusInterface  m_Hal;
  QDBusInterface* m_pDev;
  QString         m_strDevPath;

  // dbus properties
  bool m_bCharging;
  int  m_iChargeCurrent;
  int  m_iChargeLastFull;
  int  m_iChargeDesign;
};


#endif // #ifndef __QMAEMOBATTERY_H__
