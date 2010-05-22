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
  WndMain*     wndMain       (void) { return m_pWndMain; }
  WndSat*      wndSat        (void) { return m_pWndSat; }
  WndSpeed*    wndSpeed      (void) { return m_pWndSpeed; }
  WndAbout*    wndAbout      (void) { return m_pWndAbout; }
  GPSRFile*    outFile       (void) { return &m_GPSRFile; }

  void        setState    (State eNewState);
  State       getState    (void) const { return m_eState; }
  const char* getStateStr (void) const;


public slots :
  void onSettingsWritten (void);
  void onLocationFixLost (Location* pLocation, const LocationFixContainer* pLastFixCont);
  void onLocationFix     (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


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
  WndMain*    m_pWndMain;
  WndSat*     m_pWndSat;
  WndSpeed*   m_pWndSpeed;
  WndAbout*   m_pWndAbout;

  State m_eState;

  GPSRFile m_GPSRFile;
  bool     m_bVirginOutput;
  time_t   m_uiLastFixWrite;
};


#endif // #ifndef __APP_H__
