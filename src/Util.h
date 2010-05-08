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

  // maemo file system
  static QByteArray maemoFindHomeDir   (void);
  static QByteArray maemoFindMyDocsDir (void);

  // time
  static const char* timeString (bool bForFilename=false, time_t uiTime=0, bool bUTC=false);
  static int         timeDiff   (uint uiTimeOld, uint uiTimeNew, bool bAbsolute=false);
  static bool        timeSetup  (uint uiNewTime, int* pnTimeDiff=0);
};


#endif // #ifndef __UTIL_H__
