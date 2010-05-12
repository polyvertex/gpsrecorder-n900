//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-05-12 10:17:03
//
// $Id$
//
//***************************************************************************

#ifndef __WNDSAT_H__
#define __WNDSAT_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndSat
//---------------------------------------------------------------------------
class WndSat : public QMainWindow
{
  Q_OBJECT

public :
  WndSat (QMainWindow* pParent=0);
  virtual ~WndSat (void);


private :
  void createWidgets (void);


private slots :
  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  QVector<LocationFixSat> m_vecSats;
  QVector<QProgressBar*>  m_vecSatsPB;
};


#endif // #ifndef __WNDSAT_H__
