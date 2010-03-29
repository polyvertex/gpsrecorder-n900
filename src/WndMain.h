//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-25 14:53:21
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRECORD_WNDMAIN_H__
#define __GPSRECORD_WNDMAIN_H__

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
};


#endif // #ifndef __GPSRECORD_WNDMAIN_H__
