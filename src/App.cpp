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
// App
//---------------------------------------------------------------------------
App::App (int& nArgc, char** ppszArgv)
: QApplication(nArgc, ppszArgv)
{
  // ensure the QSettings default constructor will load *our* settings
  Q_ASSERT(QCoreApplication::organizationName().isEmpty() == false);
  Q_ASSERT(QCoreApplication::applicationName().isEmpty() == false);

  // show up main window
  m_WndMain.show();
}

//---------------------------------------------------------------------------
// ~App
//---------------------------------------------------------------------------
App::~App (void)
{
}
