//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-09 18:29:46
//
// $Id$
//
//***************************************************************************

#include "stable.h"
#include "revision.h"


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
// _findHomeDir
//---------------------------------------------------------------------------
static void _findHomeDir (QByteArray& strOutPath)
{
  // according to the Qt documentation, the QDir::homePath() utility does not
  // seem to use the getpwuid() function...

  char* psz;

  psz = getenv("MYDOCSDIR");
  if (psz)
    strOutPath = psz;
  else // sudo gainroot does not give MYDOCSDIR env var
    strOutPath = "/home/user/MyDocs";
  if (_isWritableDir(strOutPath.constData()))
    return;

  psz = getenv("HOME");
  if (psz && _isWritableDir(psz))
  {
    strOutPath = psz;
    return;
  }

  struct passwd* ps_passwd = getpwuid(getuid());
  if (ps_passwd && ps_passwd->pw_dir && ps_passwd->pw_dir[0] && _isWritableDir(ps_passwd->pw_dir))
  {
    strOutPath = ps_passwd->pw_dir;
    return;
  }

  strOutPath = "~/";
  if (_isWritableDir(strOutPath.constData()))
    return;

  strOutPath = "/";
}

//---------------------------------------------------------------------------
// _findOutputDir
//---------------------------------------------------------------------------
static bool _findOutputDir (QByteArray& strOutPath)
{
  _findHomeDir(strOutPath);
  if (!strOutPath.endsWith('/'))
    strOutPath += '/';
  strOutPath += qPrintable(QCoreApplication::applicationName());

  if (_isWritableDir(strOutPath.constData()))
    return true;

  if (Util::fileExists(strOutPath.constData()))
    goto __Failed; // maybe a file ?

  if (mkdir(strOutPath.constData(), 0777) == 0)
    return true;

__Failed :
  strOutPath = qPrintable(QCoreApplication::applicationName());
  return false;
}

//---------------------------------------------------------------------------
// _logHandler
//---------------------------------------------------------------------------
static void _logHandler (QtMsgType eMsgType, const char* pszMessage)
{
  if (!hLogFile)
  {
    QByteArray strLogFile;
    QString str;

    _findOutputDir(strLogFile);
    strLogFile += "/";
    strLogFile += qPrintable(QCoreApplication::applicationName());
    strLogFile += ".log";

    hLogFile = fopen(strLogFile.constData(), "a");
    if (!hLogFile)
    {
      qInstallMsgHandler(0);
      qWarning("Could not open/create log file \"%s\" ! Error %d : %s", strLogFile.constData(), errno, strerror(errno));
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
  QCoreApplication::setApplicationVersion(QString("0.0.") + SVN_REVISION_STR);
  App::setApplicationLabel("GPS Recorder");
  App::setApplicationUrl("http://gpsrecorder.jcl.name/");

  printf("%s version %s compiled on " __DATE__ " at " __TIME__ "\n",
    qPrintable(QCoreApplication::applicationName()),
    qPrintable(QCoreApplication::applicationVersion()));

  {
    QByteArray strOutDir;

    if (!_findOutputDir(strOutDir))
      qFatal("Failed to choose output directory !");

    App::setOutputDir(strOutDir);
    qDebug("Output directory is %s", strOutDir.constData());
  }

  qInstallMsgHandler(_logHandler);
  qDebug("\n"); // open log file now so we can track start time

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
