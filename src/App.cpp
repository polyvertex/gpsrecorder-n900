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
// Created On : 2010-03-28 23:54:12
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Static Members
//---------------------------------------------------------------------------
QString App::ms_strApplicationLabel;
QString App::ms_strApplicationUrl;
QString App::ms_strApplicationEMail;
QString App::ms_strOutputDir;



//---------------------------------------------------------------------------
// App
//---------------------------------------------------------------------------
App::App (int& nArgc, char** ppszArgv)
: QApplication(nArgc, ppszArgv)
{
  // force POSIX locale for *printf() calls, this is required by the
  // Exporter classes.  we do this here because the QApplication()
  // constructor seems to apply locale settings by itself.
  setlocale(LC_NUMERIC, "C");

  // init members
  m_eState            = STATE_STOPPED;
  m_uiSystemTimeSetup = 0;
  m_uiFixesWritten    = 0;
  m_uiLastFixWrite    = 0;

  // load pixmaps
  m_pPixCellModeNone    = new QPixmap(":/icons/cellmode-none-170.png");
  m_pPixCellMode2G      = new QPixmap(":/icons/cellmode-2g-170.png");
  m_pPixCellMode3G      = new QPixmap(":/icons/cellmode-3g-170.png");
  m_pPixStoppedGrey     = new QPixmap(":/icons/stopped-grey-48.png");
  m_pPixStoppedGreen    = new QPixmap(":/icons/stopped-green-48.png");
  m_pPixStoppedOrange   = new QPixmap(":/icons/stopped-orange-48.png");
  m_pPixStoppedRed      = new QPixmap(":/icons/stopped-red-48.png");
  m_pPixPausedGrey      = new QPixmap(":/icons/paused-grey-48-nomargin.png");
  m_pPixPausedGreen     = new QPixmap(":/icons/paused-green-48-nomargin.png");
  m_pPixPausedOrange    = new QPixmap(":/icons/paused-orange-48-nomargin.png");
  m_pPixPausedRed       = new QPixmap(":/icons/paused-red-48-nomargin.png");
  m_pPixRecordingGrey   = new QPixmap(":/icons/recording-grey-48-nomargin.png");
  m_pPixRecordingGreen  = new QPixmap(":/icons/recording-green-48-nomargin.png");
  m_pPixRecordingOrange = new QPixmap(":/icons/recording-orange-48-nomargin.png");
  m_pPixRecordingRed    = new QPixmap(":/icons/recording-red-48-nomargin.png");
  m_pPixStart           = new QPixmap(":/icons/start-48.png");
  m_pPixStop            = new QPixmap(":/icons/stop-48.png");
  m_pPixPause           = new QPixmap(":/icons/pause-48.png");
  m_pPixSnap            = new QPixmap(":/icons/snap-48.png");
  m_pPixState           = m_pPixStoppedGrey;

  // connect to AppSettings signals
  this->connect(
    &m_Settings,
    SIGNAL(sigSettingsWritten(void)),
    SLOT(onSettingsWritten(void)) );

  // create location driver
  m_pLocation = Location::createDevice();
  m_pLocation->resetLastFix();
  this->connect(
    m_pLocation,
    SIGNAL(sigLocationFix(Location*, const LocationFixContainer*, bool)),
    SLOT(onLocationFix(Location*, const LocationFixContainer*, bool)) );
  this->connect(
    m_pLocation,
    SIGNAL(sigLocationFixLost(Location*, const LocationFixContainer*)),
    SLOT(onLocationFixLost(Location*, const LocationFixContainer*)) );

  // apply stored settings
  this->onSettingsWritten();

  // create main window
  m_pWndMain = new WndMain();
  Q_CHECK_PTR(m_pWndMain);

  // create satellites window
  m_pWndSat = new WndSat(m_pWndMain);
  Q_CHECK_PTR(m_pWndSat);

  // create speed window
  m_pWndSpeed = new WndSpeed(m_pWndMain);
  Q_CHECK_PTR(m_pWndSpeed);

  // create cell window
  m_pWndCell = new WndCell(m_pWndMain);
  Q_CHECK_PTR(m_pWndCell);

  // create about window
  m_pWndAbout = new WndAbout(m_pWndMain);
  Q_CHECK_PTR(m_pWndAbout);

  // show up main window
  m_pWndMain->show();
}

//---------------------------------------------------------------------------
// ~App
//---------------------------------------------------------------------------
App::~App (void)
{
  m_Settings.disconnect();

  delete m_pWndAbout;
  delete m_pWndCell;
  delete m_pWndSpeed;
  delete m_pWndSat;
  delete m_pWndMain;

  delete m_pLocation;

  this->closeGPSRFile();

  delete m_pPixCellModeNone;
  delete m_pPixCellMode2G;
  delete m_pPixCellMode3G;
  delete m_pPixStoppedGrey;
  delete m_pPixStoppedGreen;
  delete m_pPixStoppedOrange;
  delete m_pPixStoppedRed;
  delete m_pPixPausedGrey;
  delete m_pPixPausedGreen;
  delete m_pPixPausedOrange;
  delete m_pPixPausedRed;
  delete m_pPixRecordingGrey;
  delete m_pPixRecordingGreen;
  delete m_pPixRecordingOrange;
  delete m_pPixRecordingRed;
  delete m_pPixStart;
  delete m_pPixStop;
  delete m_pPixPause;
  delete m_pPixSnap;
}



//---------------------------------------------------------------------------
// setApplicationLabel
//---------------------------------------------------------------------------
void App::setApplicationLabel (const QString& strLabel)
{
  ms_strApplicationLabel = strLabel;
}

//---------------------------------------------------------------------------
// applicationLabel
//---------------------------------------------------------------------------
const QString& App::applicationLabel (void)
{
  return ms_strApplicationLabel;
}

//---------------------------------------------------------------------------
// setApplicationUrl
//---------------------------------------------------------------------------
void App::setApplicationUrl (const QString& strUrl)
{
  ms_strApplicationUrl = strUrl;
}

//---------------------------------------------------------------------------
// applicationUrl
//---------------------------------------------------------------------------
const QString& App::applicationUrl (void)
{
  return ms_strApplicationUrl;
}

//---------------------------------------------------------------------------
// setApplicationEMail
//---------------------------------------------------------------------------
void App::setApplicationEMail (const QString& strEMail)
{
  ms_strApplicationEMail = strEMail;
}

//---------------------------------------------------------------------------
// applicationEMail
//---------------------------------------------------------------------------
const QString& App::applicationEMail (void)
{
  return ms_strApplicationEMail;
}

//---------------------------------------------------------------------------
// setOutputDir
//---------------------------------------------------------------------------
void App::setOutputDir (const QString& strOutputDir)
{
  AppSettings::writeOutputDirectory(qPrintable(strOutputDir));
  ms_strOutputDir = strOutputDir;
}

//---------------------------------------------------------------------------
// outputDir
//---------------------------------------------------------------------------
const QString& App::outputDir (void)
{
  if (ms_strOutputDir.isEmpty())
  {
    QByteArray strOutDir(AppSettings::readOutputDirectory());

    // if output directory is not stored yet in configuration hive,
    // define a default one
    if (strOutDir.isEmpty())
    {
      strOutDir = Util::maemoFindMyDocsDir();
      if (strOutDir.isEmpty())
        qFatal("Failed to find MyDocs directory (usually /home/user/MyDocs and defined by MYDOCSDIR env var on Maemo) !");

      if (!strOutDir.endsWith('/'))
        strOutDir += '/';
      strOutDir += qPrintable(QCoreApplication::applicationName());
    }

    if (Util::fileIsWritableDir(strOutDir.constData()))
      goto __Ok;

    if (Util::fileExists(strOutDir.constData()))
      goto __Failed; // maybe a file ?

    if (mkdir(strOutDir.constData(), 0777) == 0)
      goto __Ok;

    __Failed :
    qFatal("Unable to setup output directory !");
    exit(1);

    __Ok :
    App::setOutputDir(strOutDir);
  }

  return ms_strOutputDir;
}



//---------------------------------------------------------------------------
// setState
//---------------------------------------------------------------------------
bool App::setState (App::State eNewState)
{
  if (m_eState == eNewState)
    return true;

  if (m_eState == STATE_STOPPED)
  {
    if (eNewState == STATE_STARTED)
    {
      QByteArray strPath;
      QString    strTrackName;

      if (m_Settings.getAskTrackName())
      {
        strTrackName = this->askTrackName();
        if (strTrackName == ".")
        {
          QMaemo5InformationBox::information(
            m_pWndMain,
            tr("Action canceled !"));
          return false;
        }
      }

      m_uiFixesWritten = 0;
      m_uiLastFixWrite = 0;

      strPath  = App::outputDir().toAscii();
      strPath += "/gpstrack-";
      strPath += Util::timeStringForFileName();
      if (!strTrackName.isEmpty())
      {
        strPath += '-';
        strPath += strTrackName.toAscii();
      }
      strPath += ".gpsr";

      if (!m_GPSRFile.openWrite(strPath.constData(), true))
      {
        QMessageBox::critical(m_pWndMain, "", tr("Could not create output file at %1 !").arg(strPath.constData()));
        return false;
      }

      m_pPixState = m_pPixRecordingGrey;

      m_pLocation->resetLastFix();
      m_pLocation->start();
    }
    else
    {
      return false;
    }
  }
  else if (m_eState == STATE_STARTED)
  {
    if (eNewState == STATE_STOPPED)
    {
      __Stop :

      if (!m_Settings.getGpsAlwaysConnected())
        m_pLocation->stop();

      this->closeGPSRFile();

      m_pPixState = m_pPixPausedGrey;
    }
    else if (eNewState == STATE_PAUSED)
    {
      QString strName;
      bool    bOk;

      if (m_Settings.getAskPauseName())
      {
        bOk = false;
        strName = QInputDialog::getText(
          m_pWndMain,
          tr("Pause name ?"),
          tr("Please enter the name of your pause or leave blank :"),
          QLineEdit::Normal,
          "",
          &bOk).trimmed();
      }
      else
      {
        bOk = true;
      }

      if (!bOk)
      {
        QMaemo5InformationBox::information(
          m_pWndMain,
          tr("Action canceled !"));

        return false;
      }
      else
      {
        if (m_GPSRFile.isOpen() && m_GPSRFile.isWriting())
          m_GPSRFile.writePaused(time(0), qPrintable(strName));

        if (!strName.isEmpty())
        {
          strName.prepend(" (<i>");
          strName.append("</i>)");
        }
        QMaemo5InformationBox::information(
          m_pWndMain,
          QString(tr("Recording paused%1 !")).arg(strName),
          3000);
      }
    }
    else
    {
      return false;
    }
  }
  else if (m_eState == STATE_PAUSED)
  {
    if (eNewState == STATE_STOPPED)
    {
      goto __Stop;
    }
    else if (eNewState == STATE_STARTED)
    {
      if (m_GPSRFile.isOpen() && m_GPSRFile.isWriting())
      {
        m_GPSRFile.writeResumed(time(0));

        // reset last fix time to get new fix as soon as possible
        this->resetFixTime();
      }

      QMaemo5InformationBox::information(m_pWndMain, tr("Recording resumed !"), 3000);
    }
    else
    {
      return false;
    }
  }
  else
  {
    Q_ASSERT(0);
    return false;
  }

  m_eState = eNewState;

  emit this->sigAppStatePixChanged(m_pPixState);

  return true;
}

//---------------------------------------------------------------------------
// getStateStr
//---------------------------------------------------------------------------
const char* App::getStateStr (void) const
{
  switch (m_eState)
  {
    case STATE_STOPPED :
      return "Stopped";
    case STATE_STARTED :
      return "Started";
    case STATE_PAUSED :
      return "Paused";

    default :
      Q_ASSERT(0);
      return "?";
  }
}



//---------------------------------------------------------------------------
// askTrackName
//---------------------------------------------------------------------------
QString App::askTrackName (void)
{
  QString strTrackName;
  bool bOk = false;

  strTrackName = QInputDialog::getText(
    m_pWndMain,
    tr("Track name ?"),
    tr("Please enter track name or leave blank :"),
    QLineEdit::Normal,
    "",
    &bOk).trimmed();

  if (!bOk)
    return QString(".");

  strTrackName.replace('.', '_');

  return strTrackName;
}

//---------------------------------------------------------------------------
// closeGPSRFile
//---------------------------------------------------------------------------
void App::closeGPSRFile (void)
{
  if (m_GPSRFile.isOpen())
  {
    if (!m_GPSRFile.isWriting())
    {
      m_GPSRFile.close();
    }
    else
    {
      QByteArray strPath;

      strPath = m_GPSRFile.getPath();
      m_GPSRFile.close();

      if (!m_uiFixesWritten)
      {
        QMaemo5InformationBox::information(
          m_pWndMain,
          tr("Deleting track file because nothing was written in it !"),
          5000);
        qDebug("Deleting %s because nothing was written in it.", strPath.constData());
        QFile::remove(strPath);
      }
    }
  }

  m_uiFixesWritten = 0;
}

//---------------------------------------------------------------------------
// applyGpsTime
//---------------------------------------------------------------------------
bool App::applyGpsTime (uint uiGpsTime)
{
  int nTimeDiff = 0;

  if (Util::timeSetup(uiGpsTime, &nTimeDiff))
  {
    static uint uiLastTime = 0;

    QString strInfo;

    m_uiSystemTimeSetup = time(0);

    if (uiGpsTime < uiLastTime || (uiGpsTime - uiLastTime) >= 10) // do not bother user too much
    {
      strInfo = tr("System time synchronized with GPS and is now %1.<br>Offset was %2%3 seconds.");
      strInfo = strInfo
        .arg(Util::timeString().constData())
        .arg(nTimeDiff > 0 ? "+" : "")
        .arg(nTimeDiff);

      QMaemo5InformationBox::information(m_pWndMain, strInfo, 5000);
      uiLastTime = uiGpsTime;
    }

    strInfo.sprintf(
      "System time synchronized with GPS and is now %s (offset was %+d seconds).",
      Util::timeString().constData(),
      nTimeDiff);
    qDebug(qPrintable(strInfo));

    return true;
  }

  return false;
}



//---------------------------------------------------------------------------
// onSettingsWritten
//---------------------------------------------------------------------------
void App::onSettingsWritten (void)
{
  // adjust location settings
  // but first, force stop() to avoid multiple calls to stop() while
  // adjusting parameters.
  m_pLocation->stop();
  m_pLocation->setFixStep(Location::selectBestAllowedFixStep(m_Settings.getLogStep()));
  m_pLocation->setAssisted(m_Settings.getGpsAssisted());

  // start location service if needed
  if (!m_pLocation->isStarted() && (m_Settings.getGpsAlwaysConnected() || m_eState == STATE_STARTED || m_eState == STATE_PAUSED))
    m_pLocation->start();

  // update state pixmap
  {
    QPixmap* pPrevStatePix = m_pPixState;

    m_pPixState = (m_eState == STATE_STOPPED) ? m_pPixStoppedGrey : m_pPixRecordingGrey;
    if (m_pPixState != pPrevStatePix)
      emit this->sigAppStatePixChanged(m_pPixState);
  }

  // prevent blank screen if needed
  if (m_Settings.getPreventBlankScreen())
    QTimer::singleShot(1000, this, SLOT(onPreventBlankScreen()));
}

//---------------------------------------------------------------------------
// onPreventBlankScreen
//---------------------------------------------------------------------------
void App::onPreventBlankScreen (void)
{
  if (!m_Settings.getPreventBlankScreen())
    return;

  // send dbus command
  QDBusConnection::systemBus().call(
    QDBusMessage::createMethodCall(
      MCE_SERVICE,
      MCE_REQUEST_PATH,
      MCE_REQUEST_IF,
      MCE_PREVENT_BLANK_REQ));

  // the above dbus command has to be sent every 60 seconds at least
  QTimer::singleShot(50000, this, SLOT(onPreventBlankScreen()));
}



//---------------------------------------------------------------------------
// onLocationFixLost
//---------------------------------------------------------------------------
void App::onLocationFixLost (Location* pLocation, const LocationFixContainer* pLastFixCont)
{
  Q_UNUSED(pLocation);
  Q_UNUSED(pLastFixCont);

  if (m_GPSRFile.isOpen() && m_GPSRFile.isWriting())
    m_GPSRFile.writeLocationFixLost(time(0));
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void App::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  Q_UNUSED(pLocation);

  // update status icon
  {
    QPixmap* pPrevStatePix = m_pPixState;

    if (m_GPSRFile.isOpen() && m_GPSRFile.isWriting())
    {
      if (m_eState == STATE_PAUSED)
      {
        if (bAccurate)
          m_pPixState = m_pPixPausedGreen;
        else if (pLocation->isAcquiring())
          m_pPixState = m_pPixPausedOrange;
        else
          m_pPixState = m_pPixPausedRed;
      }
      else
      {
        if (bAccurate)
          m_pPixState = m_pPixRecordingGreen;
        else if (pLocation->isAcquiring())
          m_pPixState = m_pPixRecordingOrange;
        else
          m_pPixState = m_pPixRecordingRed;
      }
    }
    else
    {
      if (bAccurate)
        m_pPixState = m_pPixStoppedGreen;
      else if (pLocation->isAcquiring())
        m_pPixState = m_pPixStoppedOrange;
      else
        m_pPixState = m_pPixStoppedRed;
    }

    if (m_pPixState != pPrevStatePix)
      emit this->sigAppStatePixChanged(m_pPixState);
  }

  // setup system time if needed
  if (!m_uiSystemTimeSetup &&
      pFixCont->getFix()->hasFields(FIXFIELD_TIME) &&
      pFixCont->getFix()->uiTimeEP <= m_Settings.getLogStep() &&
      1) //Util::timeDiff(time(0), pFixCont->getFix()->uiTime, true) >= m_Settings.getLogStep())
  {
    this->applyGpsTime(pFixCont->getFix()->uiTime);
  }

  // write location fix
  if (m_eState == STATE_STARTED &&
      bAccurate &&
      m_GPSRFile.isOpen() &&
      m_GPSRFile.isWriting())
  {
    // log only if we have to
    bool bCanLog =
      (m_Settings.getLogStep() == m_pLocation->getFixStep()) ||
      ((pFixCont->getFix()->uiTime - m_uiLastFixWrite) >= m_Settings.getLogStep());

    // do not log anything until we've got accurate time from gps device
    if (bCanLog && !m_uiFixesWritten && !m_uiSystemTimeSetup)
      bCanLog = false;

    if (bCanLog)
    {
      ++m_uiFixesWritten;
      m_uiLastFixWrite = pFixCont->getFix()->uiTime;
      m_GPSRFile.writeLocationFix(pFixCont->getFix()->uiTime, *pFixCont);
    }
  }
}
