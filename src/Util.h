//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:32
//
// $Id$
//
//***************************************************************************

#ifndef __UTIL_H__
#define __UTIL_H__


//---------------------------------------------------------------------------
// Util
//---------------------------------------------------------------------------
class Util
{
public :

  // file system
  static bool               fileExists        (const char* pszFile);
  static bool               fileIsDir         (const char* pszFile);
  static bool               fileIsWritableDir (const char* pszFile);
  static QFile::Permissions filePermissions   (const char* pszFile);

  // time
  static QByteArray timeString            (bool bUTC=false, time_t uiTime=0, int iSecondsOffset=0);
  static QByteArray timeStringIso8601     (bool bUTC=false, time_t uiTime=0, int iSecondsOffset=0);
  static QByteArray timeStringForFileName (time_t uiTime=0);
  static int        timeDiff              (uint uiTimeOld, uint uiTimeNew, bool bAbsolute=false);
  static time_t     timeApplyOffset       (int iSecondsOffset, time_t uiTime=0, int* piAppliedOffset=0);
  static bool       timeSetup             (uint uiNewTime, int* pnTimeDiff=0);
  static int        timeZoneOffset        (void); // current time zone

  // maemo specific
  static QByteArray maemoFindHomeDir   (void);
  static QByteArray maemoFindMyDocsDir (void);
};


#endif // #ifndef __UTIL_H__
