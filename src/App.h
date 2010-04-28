//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-28 23:54:01
//
// $Id$
//
//***************************************************************************

#ifndef __APP_H__
#define __APP_H__

#include "stable.h"


//---------------------------------------------------------------------------
// App
//---------------------------------------------------------------------------
class App : public QApplication
{
  Q_OBJECT

public :
  enum State
  {
    STATE_STOPPED,
    STATE_STARTED,
  };


public :
  App (int& nArgc, char** ppszArgv);
  virtual ~App (void);

  static void           setApplicationLabel (const QString& strLabel);
  static const QString& applicationLabel    (void);
  static void           setApplicationUrl   (const QString& strUrl);
  static const QString& applicationUrl      (void);
  static void           setOutputDir        (const QString& strOutputDir);
  static const QString& outputDir           (void);

  static App* instance (void) { return static_cast<App*>(QCoreApplication::instance()); }

  AppSettings* settings      (void) { return &m_Settings; }
  bool         writeSettings (void);
  Location*    location      (void) { return m_pLocation; }
  WndMain*     wndMain       (void) { return m_pWndMain; }
  GPSRFile*    outFile       (void) { return &m_GPSRFile; }

  void        setState    (State eNewState);
  State       getState    (void) const { return m_eState; }
  const char* getStateStr (void) const;


public slots :
  void onSettingsWritten (void);
  void onLocationFixLost (Location* pLocation, const LocationFixContainer* pLastFixCont);
  void onLocationFix     (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  void closeGPSRFile (void);


private :
  static QString ms_strApplicationLabel;
  static QString ms_strApplicationUrl;
  static QString ms_strOutputDir;


private :
  AppSettings m_Settings;
  Location*   m_pLocation;
  WndMain*    m_pWndMain;

  State m_eState;

  GPSRFile m_GPSRFile;
  bool     m_bVirginOutput;
  time_t   m_uiLastFixWrite;
};


#endif // #ifndef __APP_H__
