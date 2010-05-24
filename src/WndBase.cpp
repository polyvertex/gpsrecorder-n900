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
