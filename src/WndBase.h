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
// Created On : 2010-05-24 13:34:01
//
// $Id$
//
//***************************************************************************

#ifndef __WNDBASE_H__
#define __WNDBASE_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndBase
//---------------------------------------------------------------------------
class WndBase : public QMainWindow
{
  Q_OBJECT

public :
  WndBase (QMainWindow* pParent=0);
  virtual ~WndBase (void);


protected :
  virtual void showEvent (QShowEvent* pEvent);

  virtual void createWidgets (void) = 0;
};


#endif // #ifndef __WNDBASE_H__
