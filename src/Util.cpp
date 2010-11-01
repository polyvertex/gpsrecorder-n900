//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
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
// fileIsWritableDir
//---------------------------------------------------------------------------
bool Util::fileIsWritableDir (const char* pszFile)
{
  QFileInfo fi(pszFile);

  return
    fi.isDir() &&
    (fi.permissions() & QFile::WriteUser) != 0;
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
QByteArray Util::timeString (bool bUTC/*=false*/, time_t uiTime/*=0*/, int iSecondsOffset/*=0*/)
{
  QDateTime dt;
  int       iAppliedOffset = 0;

  uiTime = Util::timeApplyOffset(iSecondsOffset, (uiTime > 0) ? uiTime : time(0), &iAppliedOffset);
  if (iSecondsOffset != iAppliedOffset)
    iSecondsOffset = iAppliedOffset;

  dt.setTimeSpec((bUTC || iSecondsOffset) ? Qt::UTC : Qt::LocalTime);
  dt.setTime_t(uiTime);

  return QByteArray(dt.toString("yyyy-MM-dd hh:mm:ss").toLatin1());
}

//---------------------------------------------------------------------------
// timeStringIso8601
//---------------------------------------------------------------------------
QByteArray Util::timeStringIso8601 (bool bUTC/*=false*/, time_t uiTime/*=0*/, int iSecondsOffset/*=0*/)
{
  QDateTime  dt;
  QByteArray strDateTime;
  int        iAppliedOffset = 0;

  uiTime = Util::timeApplyOffset(iSecondsOffset, (uiTime > 0) ? uiTime : time(0), &iAppliedOffset);
  if (iSecondsOffset != iAppliedOffset)
    iSecondsOffset = iAppliedOffset;

  dt.setTimeSpec(bUTC || iSecondsOffset ? Qt::UTC : Qt::LocalTime);
  dt.setTime_t(uiTime);

  strDateTime = dt.toString(Qt::ISODate).toLatin1();
  if (bUTC && !iSecondsOffset && !strDateTime.endsWith("Z"))
    strDateTime += 'Z';

  return strDateTime;
}

//---------------------------------------------------------------------------
// timeStringForFileName
//---------------------------------------------------------------------------
QByteArray Util::timeStringForFileName (time_t uiTime/*=0*/)
{
  QDateTime dt;

  dt.setTime_t((uiTime > 0) ? uiTime : time(0));
  return QByteArray(dt.toString("yyyyMMdd-hhmmss").toLatin1());
}

//---------------------------------------------------------------------------
// timeDiff
//---------------------------------------------------------------------------
int Util::timeDiff (uint uiOldTime, uint uiNewTime, bool bAbsolute/*=false*/)
{
  uint uiDiff = qMax(uiOldTime, uiNewTime) - qMin(uiOldTime, uiNewTime);
  int  nSign  = !bAbsolute && (uiNewTime < uiOldTime) ? -1 : 1;

  return nSign * int(uiDiff);
}

//---------------------------------------------------------------------------
// timeDiffHuman
//---------------------------------------------------------------------------
QByteArray Util::timeDiffHuman (uint uiDiffSeconds)
{
  QByteArray strDiff;

  if (uiDiffSeconds > 86400)
  {
    strDiff += QString("%1d ").arg(uiDiffSeconds / 86400);
    uiDiffSeconds %= 86400;
  }

  if (uiDiffSeconds > 3600)
  {
    strDiff += QString("%1h ").arg(uiDiffSeconds / 3600);
    uiDiffSeconds %= 3600;
  }

  if (uiDiffSeconds > 60)
  {
    strDiff += QString("%1m ").arg(uiDiffSeconds / 60);
    uiDiffSeconds %= 60;
  }

  strDiff += QString("%1s").arg(uiDiffSeconds);

  return strDiff;
}

//---------------------------------------------------------------------------
// timeApplyOffset
//---------------------------------------------------------------------------
time_t Util::timeApplyOffset (int iSecondsOffset, time_t uiTime/*=0*/, int* piAppliedOffset/*=0*/)
{
  if (uiTime <= 0)
    uiTime = time(0);

  if (!iSecondsOffset)
  {
    // nothing to do
  }
  else if (iSecondsOffset > 0)
  {
    uiTime += (time_t)iSecondsOffset;
  }
  else if (iSecondsOffset < 0)
  {
    if (time_t(-iSecondsOffset) > uiTime)
    {
      Q_ASSERT(0);
      iSecondsOffset = 0;
    }
    else
    {
      uiTime -= time_t(-iSecondsOffset);
    }
  }

  if (piAppliedOffset)
    *piAppliedOffset = iSecondsOffset;

  return uiTime;
}

//---------------------------------------------------------------------------
// timeSetup
//---------------------------------------------------------------------------
bool Util::timeSetup (uint uiNewTime, int* pnTimeDiff)
{
  uint uiOldTime = time(0);

  // use nokia's libtime to setup time since settimeofday() requires to be suid
  if (time_set_time(uiNewTime) != 0)
  {
    if (pnTimeDiff)
      *pnTimeDiff = 0;
    return false;
  }

  if (pnTimeDiff)
    *pnTimeDiff = Util::timeDiff(uiOldTime, uiNewTime);

  return true;
}

//---------------------------------------------------------------------------
// timeZoneOffset
//---------------------------------------------------------------------------
int Util::timeZoneOffset (void)
{
  return -time_get_utc_offset(0); // nokia's libtime
}

//---------------------------------------------------------------------------
// timeZoneOffsetHuman
//---------------------------------------------------------------------------
QByteArray Util::timeZoneOffsetHuman (int iTimeZoneOffset)
{
  int iAbsOffset = (iTimeZoneOffset < 0) ? -iTimeZoneOffset : iTimeZoneOffset;

  return QString("%1%2:%3")
    .arg((iTimeZoneOffset < 0) ? '-' : '+')
    .arg(uint(iAbsOffset / 3600), 2, 10, QLatin1Char('0'))
    .arg(uint(iAbsOffset % 3600 / 60), 2, 10, QLatin1Char('0'))
    .toLocal8Bit();
}



//---------------------------------------------------------------------------
// maemoFindHomeDir
//---------------------------------------------------------------------------
QByteArray Util::maemoFindHomeDir (void)
{
  // according to the Qt documentation, the QDir::homePath() utility does not
  // seem to use the getpwuid() function...

  QByteArray strHomeDir;
  char* psz;

  psz = getenv("HOME");
  if (psz && Util::fileIsWritableDir(psz))
    return psz;

  struct passwd* ps_passwd = getpwuid(getuid());
  if (ps_passwd && ps_passwd->pw_dir && ps_passwd->pw_dir[0] &&
      Util::fileIsWritableDir(ps_passwd->pw_dir))
  {
    return ps_passwd->pw_dir;
  }

  strHomeDir = "~/";
  if (Util::fileIsWritableDir(strHomeDir.constData()))
    return strHomeDir;

  strHomeDir = "/";
  if (Util::fileIsWritableDir(strHomeDir.constData()))
    return strHomeDir;

  return QByteArray();
}

//---------------------------------------------------------------------------
// maemoFindMyDocsDir
//---------------------------------------------------------------------------
QByteArray Util::maemoFindMyDocsDir (void)
{
  QByteArray strMyDocsDir;
  char* psz;

  psz = getenv("MYDOCSDIR");
  if (psz)
    strMyDocsDir = psz;
  else // sudo gainroot does not give MYDOCSDIR env var
    strMyDocsDir = "/home/user/MyDocs";
  if (Util::fileIsWritableDir(strMyDocsDir.constData()))
    return strMyDocsDir;

  return QByteArray();
}
