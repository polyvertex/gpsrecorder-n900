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
static FILE*      hLogFile = 0;
static QByteArray strLogStart;



//---------------------------------------------------------------------------
// _logHandler
//---------------------------------------------------------------------------
static void _logHandler (QtMsgType eMsgType, const char* pszMessage)
{
  if (!hLogFile)
  {
    QByteArray strLogFile;
    QString str;

    strLogFile = qPrintable(App::outputDir());
    if (!strLogFile.endsWith('/'))
      strLogFile += '/';
    strLogFile += qPrintable(QCoreApplication::applicationName());
    strLogFile += ".log";

    // destroy previous log file if it is too big
    {
      QFileInfo fi(strLogFile);
      if (fi.exists() && fi.size() >= 512000) // 500KB
        QFile::remove(strLogFile);
    }

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
      , qPrintable(QCoreApplication::applicationName()), strLogStart.constData()
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

  if (eMsgType == QtFatalMsg)
  {
    // exit to imitate default behavior
    fclose(hLogFile);
    fflush(NULL);
    exit(1);
  }
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

  printf("%s version %s compiled on " __DATE__ " at " __TIME__ ".\n",
    qPrintable(QCoreApplication::applicationName()),
    qPrintable(QCoreApplication::applicationVersion()));

  (void)App::outputDir(); // force outputDir to be setup by App now !
  printf("Output directory is %s\n", qPrintable(App::outputDir()));

  strLogStart = Util::timeString();
  qInstallMsgHandler(_logHandler);

  // run
  {
    App app(nArgc, ppszArgv);
    nRes = app.exec();
  }

  // close log file
  // at this point, App instance must be destroyed !
  qInstallMsgHandler(0);
  if (hLogFile)
  {
    fputs("\n", hLogFile);
    fclose(hLogFile);
  }

  return nRes;
}
