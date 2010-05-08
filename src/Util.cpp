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
