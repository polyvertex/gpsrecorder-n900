//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:36
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// fileExists
//---------------------------------------------------------------------------
bool Util::fileExists (const char* pszFile)
{
  //struct stat sStats;
  //return stat(pszFile, &sStats) == 0;

  return QFile::exists(pszFile);
}

//---------------------------------------------------------------------------
// fileIsDir
//---------------------------------------------------------------------------
bool Util::fileIsDir (const char* pszFile)
{
  //struct stat sStats;
  //return (stat(pszFile, &sStats) == 0) && S_ISDIR(sStats.st_mode);

  QFileInfo fi(pszFile);
  return fi.isDir();
}

//---------------------------------------------------------------------------
// filePermissions
//---------------------------------------------------------------------------
QFile::Permissions Util::filePermissions (const char* pszFile)
{
  QFileInfo fi(pszFile);
  return fi.permissions();
}



//---------------------------------------------------------------------------
// timeString
//---------------------------------------------------------------------------
const char* Util::timeString (bool bForFilename/*=false*/, time_t uiTime/*=0*/, bool bUTC/*=false*/)
{
  static char szDateTime[32];

  QDateTime dt;
  QString strFormat;

  dt.setTimeSpec(bUTC ? Qt::UTC : Qt::LocalTime);
  dt.setTime_t((uiTime > 0) ? uiTime : time(0));
  strFormat = bForFilename ? "yyyyMMdd-hhmmss" : "yyyy-MM-dd hh:mm:ss";

  strcpy((char*)&szDateTime, qPrintable(dt.toString(strFormat)));

  return (char*)&szDateTime;
}
