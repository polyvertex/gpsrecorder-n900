//***************************************************************************
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


  void setLogStep (uint uiLogStepSeconds);
  uint getLogStep (void) { return m_uiLogStepClone; }

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

  void   setKmlLineColor (const QColor& lineColor);
  QColor getKmlLineColor (void);

  void setKmlLineWidth (int nLineWidth);
  int  getKmlLineWidth (void);

  void setKmlAircraftMode (bool bEnable);
  bool getKmlAircraftMode (void);


  static uint             defaultLogStep            (void) { return 5; }
  static QPair<uint,uint> logStepBounds             (void) { return QPair<uint,uint>(1, 60); }
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
