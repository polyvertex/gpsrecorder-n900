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
const char* Util::timeString (bool bForFilename/*=false*/, time_t uiTime/*=0*/)
{
  /*
  static char szTime[24]; // YYYY-MM-DD hh:mm:ss

  time_t t = (uiTime > 0) ? uiTime : time(0);
  struct tm* ptm = localtime(&t);

  if (bForFilename)
    sprintf((char*)&szTime, "%04d%02d%02d-%02d%02d%02d", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  else
    sprintf((char*)&szTime, "%04d-%02d-%02d %02d:%02d:%02d", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

  return (char*)&szTime;
  */

  static char szDateTime[32];

  QDateTime dt;
  QString strFormat;

  dt.setTime_t((uiTime > 0) ? uiTime : time(0));
  strFormat = bForFilename ? "yyyyMMdd-hhmmss" : "yyyy-MM-dd hh:mm:ss";

  strcpy((char*)&szDateTime, qPrintable(dt.toString(strFormat)));

  return (char*)&szDateTime;
}
