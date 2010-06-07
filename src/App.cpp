//***************************************************************************
//
// GPS Recorder
// Show, log and convert location data from Maemo's GPS subsystem.
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
  // init members
  m_eState         = STATE_STOPPED;
  m_bVirginOutput  = true;
  m_uiLastFixWrite = 0;

  // load pixmaps
  m_pPixCellModeNone = new QPixmap(":/cellmode-none-170.png");
  m_pPixCellMode2G   = new QPixmap(":/cellmode-2g-170.png");
  m_pPixCellMode3G   = new QPixmap(":/cellmode-3g-170.png");
  m_pPixPauseGrey    = new QPixmap(":/pause-grey-48-nomargin.png");
  m_pPixPauseGreen   = new QPixmap(":/pause-green-48-nomargin.png");
  m_pPixPauseOrange  = new QPixmap(":/pause-orange-48-nomargin.png");
  m_pPixPauseRed     = new QPixmap(":/pause-red-48-nomargin.png");
  m_pPixRecordGrey   = new QPixmap(":/record-grey-48-nomargin.png");
  m_pPixRecordGreen  = new QPixmap(":/record-green-48-nomargin.png");
  m_pPixRecordOrange = new QPixmap(":/record-orange-48-nomargin.png");
  m_pPixRecordRed    = new QPixmap(":/record-red-48-nomargin.png");
  m_pPixState        = m_pPixPauseGrey;

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
  delete m_pPixPauseGrey;
  delete m_pPixPauseGreen;
  delete m_pPixPauseOrange;
  delete m_pPixPauseRed;
  delete m_pPixRecordGrey;
  delete m_pPixRecordGreen;
  delete m_pPixRecordOrange;
  delete m_pPixRecordRed;
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

  if (m_eState == STATE_STOPPED && eNewState == STATE_STARTED)
  {
    QByteArray strPath;
    QString    strTrackName;

    if (m_Settings.getAskTrackName())
    {
      strTrackName = this->askTrackName();
      if (strTrackName == ".")
      {
        // here, user cancelled
        // TODO : information popup
        return false;
      }
    }

    m_bVirginOutput  = true;
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

    m_pPixState = m_pPixRecordGrey;

    m_pLocation->resetLastFix();
    m_pLocation->start();
  }
  else if (m_eState == STATE_STARTED && eNewState == STATE_STOPPED)
  {
    if (!m_Settings.getGpsAlwaysConnected())
      m_pLocation->stop();

    this->closeGPSRFile();

    m_pPixState = m_pPixPauseGrey;
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
    tr("Please enter desired track name or leave blank :"),
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

      if (m_bVirginOutput)
      {
        // TODO : information popup
        qDebug("Deleting %s because nothing was written in it.", strPath.constData());
        QFile::remove(strPath);
      }
    }
  }

  m_bVirginOutput = true;
}

//---------------------------------------------------------------------------
// applyGpsTime
//---------------------------------------------------------------------------
bool App::applyGpsTime (uint uiGpsTime)
{
  int nTimeDiff = 0;

  if (Util::timeSetup(uiGpsTime, &nTimeDiff))
  {
    QString strInfo;

    strInfo.sprintf(
      "System time synchronized with GPS and is now %s (offset was %+d seconds).",
      Util::timeString().constData(),
      nTimeDiff);

    // TODO : information popup
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
  if (!m_pLocation->isStarted() && (m_Settings.getGpsAlwaysConnected() || m_eState == STATE_STARTED))
    m_pLocation->start();

  // update state pixmap
  {
    QPixmap* pPrevStatePix = m_pPixState;

    m_pPixState = (m_eState == STATE_STOPPED) ? m_pPixPauseGrey : m_pPixRecordGrey;
    if (m_pPixState != pPrevStatePix)
      emit this->sigAppStatePixChanged(m_pPixState);
  }
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
      if (bAccurate)
        m_pPixState = m_pPixRecordGreen;
      else if (pLocation->isAcquiring())
        m_pPixState = m_pPixRecordOrange;
      else
        m_pPixState = m_pPixRecordRed;
    }
    else
    {
      if (bAccurate)
        m_pPixState = m_pPixPauseGreen;
      else if (pLocation->isAcquiring())
        m_pPixState = m_pPixPauseOrange;
      else
        m_pPixState = m_pPixPauseRed;
    }

    if (m_pPixState != pPrevStatePix)
      emit this->sigAppStatePixChanged(m_pPixState);
  }

  // setup system time if needed
  if (!m_GPSRFile.isOpen() &&
      pFixCont->getFix()->hasFields(FIXFIELD_TIME) &&
      pFixCont->getFix()->uiTimeEP == 0 && // <= m_Settings.getLogStep() &&
      Util::timeDiff(time(0), pFixCont->getFix()->uiTime, true) >= m_Settings.getLogStep())
  {
    this->applyGpsTime(pFixCont->getFix()->uiTime);
  }

  // write location fix
  if (bAccurate && m_GPSRFile.isOpen() && m_GPSRFile.isWriting())
  {
    bool bCanLog =
      (m_Settings.getLogStep() == m_pLocation->getFixStep()) ||
      ((pFixCont->getFix()->uiTime - m_uiLastFixWrite) >= m_Settings.getLogStep());

    if (bCanLog && m_bVirginOutput)
    {
      if (pFixCont->getFix()->hasFields(FIXFIELD_TIME) &&
          pFixCont->getFix()->uiTimeEP <= m_Settings.getLogStep())
      {
        this->applyGpsTime(pFixCont->getFix()->uiTime);
      }
      else
      {
        // do not log until we actually get time from GPS device
        bCanLog = false;
      }
    }

    if (bCanLog)
    {
      m_bVirginOutput  = false;
      m_uiLastFixWrite = pFixCont->getFix()->uiTime;
      m_GPSRFile.writeLocationFix(pFixCont->getFix()->uiTime, *pFixCont);
    }
  }
}
