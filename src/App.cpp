//***************************************************************************
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
const char* App::SETTINGNAME_LOGSTEP            = "LogStep";
const char* App::SETTINGNAME_GPSALWAYSCONNECTED = "GpsAlwaysConnected";
const char* App::SETTINGNAME_CONVERT_CSV        = "ConvertCsv";
const char* App::SETTINGNAME_CONVERT_GPX        = "ConvertGpx";
const char* App::SETTINGNAME_CONVERT_KML        = "ConvertKml";
const char* App::SETTINGNAME_KML_LINECOLOR      = "KmlLineColor";
const char* App::SETTINGNAME_KML_LINEWIDTH      = "KmlLineWidth";
const char* App::SETTINGNAME_KML_AIRCRAFTMODE   = "KmlAircraftMode";
const char* App::SETTINGNAME_KML_COLORBYSPEED   = "KmlColorBySpeed";

QString App::ms_strApplicationLabel;
QString App::ms_strApplicationUrl;
QString App::ms_strOutputDir;



//---------------------------------------------------------------------------
// App
//---------------------------------------------------------------------------
App::App (int& nArgc, char** ppszArgv)
: QApplication(nArgc, ppszArgv)
{
  // ensure the QSettings default constructor has loaded *our* settings
  Q_ASSERT(QCoreApplication::organizationName().isEmpty() == false);
  Q_ASSERT(QCoreApplication::applicationName().isEmpty() == false);

  // init members
  m_eState        = STATE_STOPPED;
  m_bVirginOutput = true;

  // create location driver
  m_pLocation = Location::createDevice();
  this->connect(
    m_pLocation,
    SIGNAL(sigLocationFix(Location*, const LocationFixContainer*, bool)),
    SLOT(onLocationFix(Location*, const LocationFixContainer*, bool)) );
  this->connect(
    m_pLocation,
    SIGNAL(sigLocationFixLost(Location*, const LocationFixContainer*)),
    SLOT(onLocationFixLost(Location*, const LocationFixContainer*)) );

  // show up main window
  // everything must be initialized before creating the main window !
  m_pWndMain = new WndMain();
  Q_CHECK_PTR(m_pWndMain);
  m_pWndMain->show();
}

//---------------------------------------------------------------------------
// ~App
//---------------------------------------------------------------------------
App::~App (void)
{
  this->writeSettings();

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
// setOutputDir
//---------------------------------------------------------------------------
void App::setOutputDir (const QString& strOutputDir)
{
  ms_strOutputDir = strOutputDir;
}

//---------------------------------------------------------------------------
// outputDir
//---------------------------------------------------------------------------
const QString& App::outputDir (void)
{
  return ms_strOutputDir;
}



//---------------------------------------------------------------------------
// writeSettings
//---------------------------------------------------------------------------
bool App::writeSettings (void)
{
  QSettings::Status eStatus;

  m_Settings.sync();

  eStatus = m_Settings.status();
  if (eStatus != QSettings::NoError)
  {
    qWarning("Failed to write settings ! Code %d.", eStatus);
    return false;
  }

  return true;
}



//---------------------------------------------------------------------------
// getStateStr
//---------------------------------------------------------------------------
void App::setState (App::State eNewState)
{
  if (m_eState == eNewState)
    return;

  if (m_eState == STATE_STOPPED && eNewState == STATE_STARTED)
  {
    QByteArray strPath;

    m_bVirginOutput = true;

    strPath  = App::outputDir().toAscii();
    strPath += "/gpstrack-";
    strPath += Util::timeString(true);
    strPath += ".gpsr";
    if (!m_GPSRFile.openWrite(strPath.constData(), true))
    {
      // TODO : warn user !!!
    }
  }
  else if (m_eState == STATE_STARTED && eNewState == STATE_STOPPED)
  {
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
        qDebug("Deleting %s because nothing was written in it.", strPath.constData());
        QFile::remove(strPath);
      }
    }
  }

  m_bVirginOutput = true;
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

  if (bAccurate && m_GPSRFile.isOpen() && m_GPSRFile.isWriting())
  {
    m_bVirginOutput = false;
    m_GPSRFile.writeLocationFix(pFixCont->getFix()->uiTime, *pFixCont);
  }
}
