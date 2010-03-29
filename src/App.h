//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-28 23:54:01
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRECORD_APP_H__
#define __GPSRECORD_APP_H__

#include "stable.h"


//---------------------------------------------------------------------------
// App
//---------------------------------------------------------------------------
class App : public QApplication
{
  Q_OBJECT

public :
  App (int& nArgc, char** ppszArgv);
  virtual ~App (void);

  QSettings* getSettings (void) { return &m_Settings; }
  Location*  getLocation (void) { return &m_Location; }


private :
  QSettings m_Settings;
  Location  m_Location;
  WndMain   m_WndMain;
};


#endif // #ifndef __GPSRECORD_APP_H__
