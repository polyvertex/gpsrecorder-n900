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

  // create location driver
  m_pLocation = Location::createDevice();

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
