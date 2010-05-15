//***************************************************************************
//
// GPS Recorder
// Show, log and convert GPS data from Maemo's GPS subsystem.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-27 13:06:43
//
// $Id$
//
//***************************************************************************

#ifndef __APPSETTINGS_H__
#define __APPSETTINGS_H__

#include "stable.h"


//---------------------------------------------------------------------------
// AppSettings
//---------------------------------------------------------------------------
class AppSettings : public QObject
{
  Q_OBJECT

public :
  AppSettings (QObject* pParent=0);
  ~AppSettings (void);

  QSettings& qsettings (void) { return m_Settings; }

  QSettings::Status write (void);


  static QByteArray readOutputDirectory  (void);
  static void       writeOutputDirectory (const QByteArray& strOutputDir);


  void setLogStep (uint uiLogStepSeconds);
  uint getLogStep (void) { return m_uiLogStepClone; }

  void setGpsAssisted (bool bEnable);
  bool getGpsAssisted (void);

  void setGpsAlwaysConnected (bool bEnable);
  bool getGpsAlwaysConnected (void);

  void setConvertCsv (bool bEnable);
  bool getConvertCsv (void);
  void setConvertGpx (bool bEnable);
  bool getConvertGpx (void);
  void setConvertKml (bool bEnable);
  bool getConvertKml (void);

  void setCsvSeparator (char cSep);
  char getCsvSeparator (void);

  void setKmlZipped (bool bEnable);
  bool getKmlZipped (void);

  void   setKmlLineColor (const QColor& lineColor);
  QColor getKmlLineColor (void);

  void setKmlLineWidth (int nLineWidth);
  int  getKmlLineWidth (void);

  void setKmlAircraftMode (bool bEnable);
  bool getKmlAircraftMode (void);


  static uint             defaultLogStep            (void) { return 5; }
  static QPair<uint,uint> logStepBounds             (void) { return QPair<uint,uint>(1, 120); }
  static bool             defaultGpsAssisted        (void) { return false; }
  static bool             defaultGpsAlwaysConnected (void) { return false; }

  static bool defaultConvertCsv (void) { return true; }
  static bool defaultConvertGpx (void) { return true; }
  static bool defaultConvertKml (void) { return true; }


signals :
  void sigSettingsWritten (void);


private :
  uint getLogStep_Impl (void);


private :
  QSettings m_Settings;

  uint m_uiLogStepClone; // an internal clone value to optimize calls to getLogStep()
};


#endif // #ifndef __APPSETTINGS_H__
