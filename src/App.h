//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
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
    STATE_PAUSED,
  };


public :
  App (int& nArgc, char** ppszArgv);
  virtual ~App (void);

  static void           setApplicationLabel (const QString& strLabel);
  static const QString& applicationLabel    (void);
  static void           setApplicationUrl   (const QString& strUrl);
  static const QString& applicationUrl      (void);
  static void           setApplicationEMail (const QString& strEMail);
  static const QString& applicationEMail    (void);
  static void           setOutputDir        (const QString& strOutputDir);
  static const QString& outputDir           (void);

  static App* instance (void) { return static_cast<App*>(QCoreApplication::instance()); }

  AppSettings* settings      (void) { return &m_Settings; }
  bool         writeSettings (void);
  Location*    location      (void) { return m_pLocation; }
  GPSRFile*    outFile       (void) { return &m_GPSRFile; }
  void         resetFixTime  (void) { m_uiLastFixWrite = 0; }

  bool        setState    (State eNewState);
  State       getState    (void) const { return m_eState; }
  const char* getStateStr (void) const;
  QPixmap*    getStatePix (void) { return m_pPixState; }

  time_t lastTimeSetup      (void) const { return m_uiSystemTimeSetup; }
  uint   fixesWritten       (void) const { return m_uiFixesWritten; }
  time_t lastWrittenFixTime (void) const { return m_uiLastFixWrite; }

  WndMain*  wndMain  (void) { return m_pWndMain; }
  WndSat*   wndSat   (void) { return m_pWndSat; }
  WndSpeed* wndSpeed (void) { return m_pWndSpeed; }
  WndCell*  wndCell  (void) { return m_pWndCell; }
  WndAbout* wndAbout (void) { return m_pWndAbout; }

  QPixmap* pixCellModeNone (void) { return m_pPixCellModeNone; }
  QPixmap* pixCellMode2G   (void) { return m_pPixCellMode2G; }
  QPixmap* pixCellMode3G   (void) { return m_pPixCellMode3G; }
  QPixmap* pixStart        (void) { return m_pPixStart; }
  QPixmap* pixPause        (void) { return m_pPixPause; }
  QPixmap* pixStop         (void) { return m_pPixStop; }
  QPixmap* pixSnap         (void) { return m_pPixSnap; }


signals :
  void sigAppStatePixChanged (QPixmap* pNewStatePixmap);


public slots :
  void onSettingsWritten    (void);
  void onPreventBlankScreen (void);
  void onLocationFixLost    (Location* pLocation, const LocationFixContainer* pLastFixCont);
  void onLocationFix        (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  void closeGPSRFile (void);
  bool applyGpsTime  (uint uiGpsTime);


private :
  static QString ms_strApplicationLabel;
  static QString ms_strApplicationUrl;
  static QString ms_strApplicationEMail;
  static QString ms_strOutputDir;


private :
  AppSettings m_Settings;
  Location*   m_pLocation;
  QPixmap*    m_pPixState;

  QPixmap* m_pPixCellModeNone;
  QPixmap* m_pPixCellMode2G;
  QPixmap* m_pPixCellMode3G;
  QPixmap* m_pPixStoppedGrey;
  QPixmap* m_pPixStoppedGreen;
  QPixmap* m_pPixStoppedOrange;
  QPixmap* m_pPixStoppedRed;
  QPixmap* m_pPixPausedGrey;
  QPixmap* m_pPixPausedGreen;
  QPixmap* m_pPixPausedOrange;
  QPixmap* m_pPixPausedRed;
  QPixmap* m_pPixRecordingGrey;
  QPixmap* m_pPixRecordingGreen;
  QPixmap* m_pPixRecordingOrange;
  QPixmap* m_pPixRecordingRed;
  QPixmap* m_pPixStart;
  QPixmap* m_pPixStop;
  QPixmap* m_pPixPause;
  QPixmap* m_pPixSnap;

  WndMain*  m_pWndMain;
  WndSat*   m_pWndSat;
  WndSpeed* m_pWndSpeed;
  WndCell*  m_pWndCell;
  WndAbout* m_pWndAbout;

  State  m_eState;
  time_t m_uiSystemTimeSetup; // last time the system time has been setup

  GPSRFile m_GPSRFile;
  uint     m_uiFixesWritten;
  time_t   m_uiLastFixWrite;
};


#endif // #ifndef __APP_H__
