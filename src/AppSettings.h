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
// Created: 2010-04-27 13:06:43
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

  void setAlwaysCreateNewFile (bool bEnable);
  bool getAlwaysCreateNewFile (void);

  void setAskPointName (bool bEnable);
  bool getAskPointName (void);

  void setAskPauseName (bool bEnable);
  bool getAskPauseName (void);

  void setUnitSystem (uint uiUnitSystem);
  uint getUnitSystem (void);

  void setHorizSpeedUnit (uint uiHorizSpeedUnit);
  uint getHorizSpeedUnit (void);

  void setPreventBlankScreen (bool bEnable);
  bool getPreventBlankScreen (void);

  void   setLastMeansOfTransport (quint8 ucMeansOfTransport);
  quint8 getLastMeansOfTransport (void);

  void       setLastOtherMeansOfTransport (const QByteArray& strOtherMeansOfTransport);
  QByteArray getLastOtherMeansOfTransport (void);

  void setConvertTxt (bool bEnable);
  bool getConvertTxt (void);
  void setConvertCsv (bool bEnable);
  bool getConvertCsv (void);
  void setConvertGpx (bool bEnable);
  bool getConvertGpx (void);
  void setConvertKml (bool bEnable);
  bool getConvertKml (void);

  void setConvertExportPauses (bool bEnable);
  bool getConvertExportPauses (void);

  void setTxtIncludeLocationFix (bool bInclude);
  bool getTxtIncludeLocationFix (void);

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


  static uint             defaultLogStep             (void) { return 5; }
  static QPair<uint,uint> logStepBounds              (void) { return QPair<uint,uint>(1, 10800); }
  static bool             defaultGpsAssisted         (void) { return true; }
  static bool             defaultGpsAlwaysConnected  (void) { return true; }
  static bool             defaultAlwaysCreateNewFile (void) { return false; }
  static bool             defaultAskPointName        (void) { return true; }
  static bool             defaultAskPauseName        (void) { return true; }
  static uint             defaultUnitSystem          (void) { return UNITSYSTEM_METRIC; }
  static uint             defaultHorizSpeedUnit      (void) { return HORIZSPEEDUNIT_KMH; }
  static bool             defaultPreventBlankScreen  (void) { return false; }
  static quint8           defaultMeansOfTransport    (void) { return GPSRFile::MEANSTRANSPORT_UNKNOWN; }

  static bool defaultConvertTxt (void) { return false; }
  static bool defaultConvertCsv (void) { return true; }
  static bool defaultConvertGpx (void) { return true; }
  static bool defaultConvertKml (void) { return true; }

  static bool defaultConvertExportPauses (void) { return true; }

  static bool        horizSpeedUnitValidate (uint uiHorizSpeedUnit);
  static const char* horizSpeedUnitToName   (uint uiHorizSpeedUnit);
  static bool        unitSystemValidate     (uint uiUnitSystem);
  static const char* unitSystemToName       (uint uiUnitSystem);


signals :
  void sigSettingsWritten (void);


private :
  uint getLogStep_Impl (void);


private :
  QSettings m_Settings;

  uint m_uiLogStepClone; // an internal clone value to optimize calls to getLogStep()
};


#endif // #ifndef __APPSETTINGS_H__
