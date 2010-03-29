//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-09 18:29:46
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// m a i n
//---------------------------------------------------------------------------
int main (int nArgc, char** ppszArgv)
{
  QCoreApplication::setOrganizationName("polyvertex");
  QCoreApplication::setOrganizationDomain("polyvertex.net");
  QCoreApplication::setApplicationName("gpsrecord");
  QCoreApplication::setApplicationVersion("0.0.1");

  App app(nArgc, ppszArgv);
  return app.exec();
}
