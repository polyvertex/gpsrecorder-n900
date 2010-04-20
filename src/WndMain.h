//***************************************************************************
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


private :
  void showHome (void);
  void showFix  (void);

  void clearFixFields (void);


private slots :
  void onPushedStartStop (void);
  void onPushedSnap      (void);
  void onPushedExport    (void);

  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  QAction* m_pMenuStartStop;
  QAction* m_pMenuSnap;
  QAction* m_pMenuExport;

  QLabel* m_pLblStatus;

  QLabel* m_pLblFixFields;
  QLabel* m_pLblFixMode;
  QLabel* m_pLblFixTime;
  QLabel* m_pLblFixLat;
  QLabel* m_pLblFixLong;
  QLabel* m_pLblFixAlt;
  QLabel* m_pLblFixTrack;
  QLabel* m_pLblFixSpeed;
  QLabel* m_pLblFixSatUse;
  QLabel* m_pLblFixGsm;
  QLabel* m_pLblFixWcdma;

  time_t m_uiStartTime;

  LocationFixCellInfoGsm   m_CellInfoGsm;
  time_t                   m_uiCellInfoGsmTime;
  LocationFixCellInfoWcdma m_CellInfoWcdma;
  time_t                   m_uiCellInfoWcdmaTime;
};


#endif // #ifndef __WNDMAIN_H__
