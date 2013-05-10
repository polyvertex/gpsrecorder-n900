//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex+gpsrecorder [AT] gmail [DOT] com>
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
// Created On : 2010-05-22 10:04:19
//
// $Id$
//
//***************************************************************************

#ifndef __WNDSPEED_H__
#define __WNDSPEED_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndSpeed
//---------------------------------------------------------------------------
class WndSpeed : public WndBase
{
  Q_OBJECT

public :
  WndSpeed (QMainWindow* pParent=0);
  virtual ~WndSpeed (void);


private :
  void createWidgets (void);


private slots :
  void onAppStatePixChanged (QPixmap* pNewStatePixmap);

  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  time_t m_uiSpeedLastUpdate;

  QLabel* m_pLblSpeed;
  QLabel* m_pLblSpeedUpdate;
  QLabel* m_pLblSpeedUnit;
  QLabel* m_pLblStatusIcon;
};


#endif // #ifndef __WNDSPEED_H__
