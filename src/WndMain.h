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
// Created On : 2010-03-25 14:53:21
//
// $Id$
//
//***************************************************************************

#ifndef __WNDMAIN_H__
#define __WNDMAIN_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndMain
//---------------------------------------------------------------------------
class WndMain : public QMainWindow
{
  Q_OBJECT

public :
  WndMain (QMainWindow* pParent=0);
  virtual ~WndMain (void);


protected :
  void closeEvent (QCloseEvent* pEvent);


private :
  void createWidgets (void);

  void showHome (void);
  void showFix  (void);

  void clearFixFields (void);


private slots :
  void onPushedStartStop (void);
  void onPushedSnap      (void);
  void onPushedConfig    (void);
  void onPushedConvert   (void);
  void onPushedSat       (void);
  void onPushedAbout     (void);

  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  QAction* m_pMenuStartStop;
  QAction* m_pMenuSnap;
  QAction* m_pMenuConfig;
  QAction* m_pMenuConvert;
  QAction* m_pMenuSat;
  QAction* m_pMenuAbout;

  QLabel* m_pLblStatus;
  QLabel* m_pLblFixFields;
  QLabel* m_pLblFixMode;
  QLabel* m_pLblFixTime;
  QLabel* m_pLblFixSatUse;
  QLabel* m_pLblFixLat;
  QLabel* m_pLblFixLong;
  QLabel* m_pLblFixAlt;
  QLabel* m_pLblFixTrack;
  QLabel* m_pLblFixSpeed;
  QLabel* m_pLblFixGsm;
  QLabel* m_pLblFixWcdma;

  LocationFixCellInfoGsm   m_CellInfoGsm;
  time_t                   m_uiCellInfoGsmTime;
  LocationFixCellInfoWcdma m_CellInfoWcdma;
  time_t                   m_uiCellInfoWcdmaTime;
};


#endif // #ifndef __WNDMAIN_H__
