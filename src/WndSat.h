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
class WndSat : public WndBase
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
