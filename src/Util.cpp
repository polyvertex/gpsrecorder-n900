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

  dt.setTimeSpec(bUTC || iSecondsOffset ? Qt::UTC : Qt::LocalTime);
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
