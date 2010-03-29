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

  static void           setOutputDir (const QString& strOutputDir);
  static const QString& outputDir    (void);

  static App* instance (void) { return static_cast<App*>(QCoreApplication::instance()); }

  QSettings* settings (void) { return &m_Settings; }
  Location*  location (void) { return m_pLocation; }
  WndMain*   wndMain  (void) { return m_pWndMain; }


private :
  static QString ms_strOutputDir;


private :
  QSettings m_Settings;
  Location* m_pLocation;
  WndMain*  m_pWndMain;
};


#endif // #ifndef __GPSRECORD_APP_H__
