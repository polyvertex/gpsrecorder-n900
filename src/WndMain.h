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
class WndMain : public WndBase
{
  Q_OBJECT

public :
  WndMain (QMainWindow* pParent=0);
  virtual ~WndMain (void);

  void setMeansOfTransport (quint8 ucMeansOfTransport, const QString& strOtherMeansOfTransport);


protected :
  void closeEvent (QCloseEvent* pEvent);


private :
  void createWidgets (void);

  void showHome (void);
  void showFix  (void);

  void clearFixFields (void);


private slots :
  void onClickedStartStop        (void);
  void onClickedPauseResume      (void);
  void onClickedSnap             (void);
  void onClickedConfig           (void);
  void onClickedConvert          (void);
  void onClickedMeansOfTransport (void);
  void onClickedSat              (void);
  void onClickedSpeed            (void);
  void onClickedCell             (void);
  void onClickedAbout            (void);

  void onAppStatePixChanged (QPixmap* pNewStatePixmap);

  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  QAction* m_pMenuStartStop;
  QAction* m_pMenuConfig;
  QAction* m_pMenuConvert;
  QAction* m_pMenuAbout;

  QLabel* m_pLblStatus;
  QLabel* m_pLblStatusIcon;
  QLabel* m_pLblFixFields;
  QLabel* m_pLblFixMode;
  QLabel* m_pLblFixTime;
  QLabel* m_pLblFixSatUse;
  QLabel* m_pLblFixLat;
  QLabel* m_pLblFixLong;
  QLabel* m_pLblFixAlt;
  QLabel* m_pLblFixTrack;
  QLabel* m_pLblFixSpeed;

  QLabel* m_pLblTimeFixed;
  QLabel* m_pLblFixesWritten;
  QLabel* m_pLblLastWrittenFixTime;

  QLabel* m_pLblMeansOfTransport;

  QPushButton* m_pBtnPauseResume;
  QPushButton* m_pBtnSnap;
  QPushButton* m_pBtnMOT;
  QPushButton* m_pBtnCell;
};


#endif // #ifndef __WNDMAIN_H__
