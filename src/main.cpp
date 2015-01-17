//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex@gmail.com>
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
// Created: 2010-03-09 18:29:46
//
//***************************************************************************

#include "stable.h"
#include "version.h"


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
      "* %s v%s log started on %s\n"
      "%s\n"
      , qPrintable(str)
      , qPrintable(QCoreApplication::applicationName())
      , qPrintable(QCoreApplication::applicationVersion())
      , strLogStart.constData()
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
          Util::timeString().constData(),
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

  QCoreApplication::setOrganizationName("jcl");
  //QCoreApplication::setOrganizationDomain("example.com");
  QCoreApplication::setApplicationName("gpsrecorder");
  QCoreApplication::setApplicationVersion(APP_VERSION_STR);
  App::setApplicationLabel("GPS Recorder");
  App::setApplicationUrl("http://garage.maemo.org/projects/gpsrecorder/");
  App::setApplicationEMail("polyvertex+gpsrecorder" /*NO*/ "@" /*SPAM*/ "gmail.com");

  printf("%s version %s compiled on " __DATE__ " at " __TIME__ ".\n",
    qPrintable(QCoreApplication::applicationName()),
    qPrintable(QCoreApplication::applicationVersion()));

  (void)App::outputDir(); // force outputDir to be setup by App now !
  //printf("Output directory is %s\n", qPrintable(App::outputDir()));

  fflush(NULL);

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
