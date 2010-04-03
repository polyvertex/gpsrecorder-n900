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

  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  QAction* m_pMenuStartStop;

  QLineEdit* m_pTxtStatus;

  QLineEdit* m_pTxtFixFields;
  QLineEdit* m_pTxtFixMode;
  QLineEdit* m_pTxtFixTime;
  QLineEdit* m_pTxtFixLat;
  QLineEdit* m_pTxtFixLong;
  QLineEdit* m_pTxtFixAlt;
  QLineEdit* m_pTxtFixTrack;
  QLineEdit* m_pTxtFixSpeed;
  QLineEdit* m_pTxtFixSatUse;
  QLineEdit* m_pTxtFixGsm;
  QLineEdit* m_pTxtFixWcdma;
};


#endif // #ifndef __WNDMAIN_H__
