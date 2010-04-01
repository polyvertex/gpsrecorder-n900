//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-09 18:29:46
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Local Variables
//---------------------------------------------------------------------------
static FILE* hLogFile = 0;



//---------------------------------------------------------------------------
// _isWritableDir
//---------------------------------------------------------------------------
static bool _isWritableDir (const char* pszPath)
{
  return
    Util::fileIsDir(pszPath) &&
    (Util::filePermissions(pszPath) & QFile::WriteUser) != 0;
}

//---------------------------------------------------------------------------
// _findDefaultUserDir
//---------------------------------------------------------------------------
static const char* _findDefaultUserDir (bool bOutputDir)
{
  // according to the Qt documentation, the QDir::homePath() utility does not
  // seem to use the getpwuid() function...

  char* psz;

  if (bOutputDir)
  {
    psz = getenv("MYDOCSDIR");
    if (psz && _isWritableDir(psz))
      return psz;

    if (_isWritableDir("/home/user/MyDocs")) // sudo gainroot does not give MYDOCSDIR env var
      return "/home/user/MyDocs";
  }

  psz = getenv("HOME");
  if (psz && _isWritableDir(psz))
    return psz;

  struct passwd* ps_passwd = getpwuid(getuid());
  if (ps_passwd && ps_passwd->pw_dir && ps_passwd->pw_dir[0] && _isWritableDir(ps_passwd->pw_dir))
    return ps_passwd->pw_dir;

  if (_isWritableDir("~/"))
    return "~/";

  return "/";
}

//---------------------------------------------------------------------------
// _logHandler
//---------------------------------------------------------------------------
static void _logHandler (QtMsgType eMsgType, const char* pszMessage)
{
  if (!hLogFile)
  {
    QString str;

    str  = _findDefaultUserDir(false);
    str += "/.";
    str += QCoreApplication::applicationName();
    str += ".log";

    hLogFile = fopen(qPrintable(str), "a");
    if (!hLogFile)
    {
      qInstallMsgHandler(0);
      qWarning("Could not open/create log file \"%s\" ! Error %d : %s", qPrintable(str), errno, strerror(errno));
      return;
    }

    str.fill('*', 78);

    fprintf(hLogFile,
      "\n"
      "%s\n"
      "* %s log started on %s\n"
      "%s\n"
      , qPrintable(str)
      , qPrintable(QCoreApplication::applicationName()), Util::timeString()
      , qPrintable(str) );
  }

  if (pszMessage && pszMessage[0])
  {
    size_t uiLen = strlen(pszMessage);
    bool   bHasNewLine = (pszMessage[uiLen-1] == '\n') ? 1 : 0;

    if (bHasNewLine && uiLen == 1)
    {
      fputs("\n", hLogFile);
    }
    else
    {
      char cLevel;

      switch (eMsgType)
      {
        case QtDebugMsg :
          cLevel = 'D';
          break;
        case QtWarningMsg :
          cLevel = 'W';
          break;
        case QtCriticalMsg :
          cLevel = 'E';
          break;
        case QtFatalMsg :
          cLevel = 'X';
          break;
        default :
          cLevel = '?';
          break;
      }

      for (int i = 0; i < 2; ++i)
      {
        fprintf((i == 0) ? hLogFile : stderr,
          "%s [%c] %s%s",
          Util::timeString(),
          cLevel,
          (pszMessage ? pszMessage : ""),
          (bHasNewLine ? "" : "\n") );
      }
    }
  }

  fflush(hLogFile);
}



//---------------------------------------------------------------------------
// m a i n
//---------------------------------------------------------------------------
int main (int nArgc, char** ppszArgv)
{
  int nRes;

  QCoreApplication::setOrganizationName("polyvertex");
  QCoreApplication::setOrganizationDomain("polyvertex.net");
  QCoreApplication::setApplicationName("gpsrecorder");
  App::setApplicationLabel("GPS Recorder");
  QCoreApplication::setApplicationVersion("0.0.1");

  qInstallMsgHandler(_logHandler);
  qDebug("\n"); // open log file now so we can track start time

  App::setOutputDir(_findDefaultUserDir(true));
  qDebug("Default output directory is %s", qPrintable(App::outputDir()));

  // run
  {
    App app(nArgc, ppszArgv);
    nRes = app.exec();
  }

  if (hLogFile)
  {
    fputs("\n", hLogFile);
    fclose(hLogFile);
  }

  return nRes;
}
