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
