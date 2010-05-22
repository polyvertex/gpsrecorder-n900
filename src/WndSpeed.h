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
class WndSpeed : public QMainWindow
{
  Q_OBJECT

public :
  WndSpeed (QMainWindow* pParent=0);
  virtual ~WndSpeed (void);


private :
  void createWidgets (void);


private slots :
  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  QLabel* m_pLblSpeed;
};


#endif // #ifndef __WNDSPEED_H__
