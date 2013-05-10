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
// Created On : 2010-05-24 13:34:08
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndBase
//---------------------------------------------------------------------------
WndBase::WndBase (QMainWindow* pParent/*=0*/)
: QMainWindow(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(App::instance()->location());

#if QT_VERSION > 0x040503
  this->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
}

//---------------------------------------------------------------------------
// ~WndBase
//---------------------------------------------------------------------------
WndBase::~WndBase (void)
{
}



//---------------------------------------------------------------------------
// showEvent
//---------------------------------------------------------------------------
void WndBase::showEvent (QShowEvent* pEvent)
{
  // this allows to repaint widgets which were updated while they were
  // hidden.
  // this also allows to avoid graphical artefacts, especially for Maemo's
  // stacked windows where flickering occurs when show() is called.
  QTimer::singleShot(1000, this, SLOT(update()));

  QMainWindow::showEvent(pEvent);
}
