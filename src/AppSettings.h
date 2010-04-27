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
class AppSettings
{
public :
  AppSettings (void);
  ~AppSettings (void);

  QSettings& qsettings (void) { return m_Settings; }

  QSettings::Status write (void);


  void setLogStep (uint uiLogStepSeconds);
  uint getLogStep (void);

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


  static uint             defaultLogStep (void) { return 2; }
  static QPair<uint,uint> logStepBounds  (void) { return QPair<uint,uint>(1, 60); }

  static bool defaultConvertCsv (void) { return true; }
  static bool defaultConvertGpx (void) { return true; }
  static bool defaultConvertKml (void) { return true; }


private :
  QSettings m_Settings;
};


#endif // #ifndef __APPSETTINGS_H__
