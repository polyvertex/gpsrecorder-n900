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

  if (m_pWndAbout)
    delete m_pWndAbout;
  if (m_pWndSpeed)
    delete m_pWndSpeed;
  if (m_pWndSat)
    delete m_pWndSat;
  if (m_pWndMain)
    delete m_pWndMain;

  if (m_pLocation)
    delete m_pLocation;

  this->closeGPSRFile();
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
void App::setState (App::State eNewState)
{
  if (m_eState == eNewState)
    return;

  if (m_eState == STATE_STOPPED && eNewState == STATE_STARTED)
  {
    QByteArray strPath;
    QString    strTrackName;

    m_bVirginOutput  = true;
    m_uiLastFixWrite = 0;

    if (m_Settings.getAskTrackName())
      strTrackName = this->askTrackName();

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
      // TODO : warn user !!!
    }

    m_pLocation->resetLastFix();
    m_pLocation->start();
  }
  else if (m_eState == STATE_STARTED && eNewState == STATE_STOPPED)
  {
    if (!m_Settings.getGpsAlwaysConnected())
      m_pLocation->stop();

    this->closeGPSRFile();
  }

  m_eState = eNewState;
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

  strTrackName = QInputDialog::getText(
    m_pWndMain,
    tr("Track name ?"),
    tr("Please enter desired track name or leave blank :"),
    QLineEdit::Normal).trimmed();

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
  if (m_Settings.getGpsAlwaysConnected() && !m_pLocation->isStarted())
    m_pLocation->start();
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
