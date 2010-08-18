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

  time_t lastTimeSetup (void) const { return m_uiSystemTimeSetup; }

  WndMain*  wndMain  (void) { return m_pWndMain; }
  WndSat*   wndSat   (void) { return m_pWndSat; }
  WndSpeed* wndSpeed (void) { return m_pWndSpeed; }
  WndCell*  wndCell  (void) { return m_pWndCell; }
  WndAbout* wndAbout (void) { return m_pWndAbout; }

  QPixmap* pixCellModeNone (void) { return m_pPixCellModeNone; }
  QPixmap* pixCellMode2G   (void) { return m_pPixCellMode2G; }
  QPixmap* pixCellMode3G   (void) { return m_pPixCellMode3G; }
  QPixmap* pixStart        (void) { return m_pPixStart; }
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
  QString askTrackName  (void);
  void    closeGPSRFile (void);
  bool    applyGpsTime  (uint uiGpsTime);


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
  QPixmap* m_pPixPauseGrey;
  QPixmap* m_pPixPauseGreen;
  QPixmap* m_pPixPauseOrange;
  QPixmap* m_pPixPauseRed;
  QPixmap* m_pPixRecordGrey;
  QPixmap* m_pPixRecordGreen;
  QPixmap* m_pPixRecordOrange;
  QPixmap* m_pPixRecordRed;
  QPixmap* m_pPixStart;
  QPixmap* m_pPixStop;
  QPixmap* m_pPixSnap;

  WndMain*  m_pWndMain;
  WndSat*   m_pWndSat;
  WndSpeed* m_pWndSpeed;
  WndCell*  m_pWndCell;
  WndAbout* m_pWndAbout;

  State  m_eState;
  time_t m_uiSystemTimeSetup; // last time the system time has been setup

  GPSRFile m_GPSRFile;
  bool     m_bVirginOutput;
  time_t   m_uiLastFixWrite;
};


#endif // #ifndef __APP_H__
