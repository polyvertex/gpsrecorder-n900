//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:36
//
// $Id$
//
//***************************************************************************

#include "main.h"


//---------------------------------------------------------------------------
// FileExists
//---------------------------------------------------------------------------
bool Util::FileExists (const char* pszFile)
{
  struct stat sStats;
  return stat(pszFile, &sStats) == 0;
}

//---------------------------------------------------------------------------
// FileIsDir
//---------------------------------------------------------------------------
bool Util::FileIsDir (const char* pszFile)
{
  struct stat sStats;

  if ((stat(pszFile, &sStats) == 0) && S_ISDIR(sStats.st_mode))
    return true;

  return false;
}



//---------------------------------------------------------------------------
// TimeString
//---------------------------------------------------------------------------
const char* Util::TimeString (bool bForFilename, time_t nTime/*=0*/)
{
  static char szTime[24]; // YYYY-MM-DD hh:mm:ss

  time_t t = (nTime > 0) ? nTime : time(NULL);
  struct tm* ptm = localtime(&t);

  if (bForFilename)
    sprintf((char*)&szTime, "%04d%02d%02d-%02d%02d%02d", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  else
    sprintf((char*)&szTime, "%04d-%02d-%02d %02d:%02d:%02d", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

  return (char*)&szTime;
}
