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
QString App::ms_strApplicationLabel;
QString App::ms_strOutputDir;


//---------------------------------------------------------------------------
// App
//---------------------------------------------------------------------------
App::App (int& nArgc, char** ppszArgv)
: QApplication(nArgc, ppszArgv)
{
  // ensure the QSettings default constructor will load *our* settings
  Q_ASSERT(QCoreApplication::organizationName().isEmpty() == false);
  Q_ASSERT(QCoreApplication::applicationName().isEmpty() == false);

  // init properties
  m_eState = STATE_STOPPED;

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
  if (m_pWndMain)
    delete m_pWndMain;

  if (m_pLocation)
    delete m_pLocation;
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
// getStateStr
//---------------------------------------------------------------------------
void App::setState (App::State eNewState)
{
  if (m_eState == eNewState)
    return;

  if (m_eState == STATE_STOPPED && eNewState == STATE_STARTED)
  {
    QByteArray strPath;

    strPath  = App::outputDir().toAscii();
    strPath += "/";
    strPath += App::applicationName().toAscii();
    strPath += "-";
    strPath += Util::timeString(true);
    strPath += ".gpsr";
    if (m_GPSRFile.openWrite(strPath.constData(), true))
      qDebug("Opened GPSR file %s", strPath.constData());
  }
  else if (m_eState == STATE_STARTED && eNewState == STATE_STOPPED)
  {
    m_GPSRFile.close();
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
      return "stopped";
    case STATE_STARTED :
      return "started";

    default :
      Q_ASSERT(0);
      return "?";
  }
}



//---------------------------------------------------------------------------
// onLocationFixLost
//---------------------------------------------------------------------------
void App::onLocationFixLost (Location* pLocation, const LocationFixContainer* pLastFixCont)
{
  Q_UNUSED(pLocation);
  Q_UNUSED(pLastFixCont);

  if (m_GPSRFile.isOpen() && !m_GPSRFile.isReading())
    m_GPSRFile.writeLocationFixLost(time(0));
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void App::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  Q_UNUSED(pLocation);

  if (bAccurate && m_GPSRFile.isOpen() && !m_GPSRFile.isReading())
    m_GPSRFile.writeLocationFix(time(0), *pFixCont);
}
