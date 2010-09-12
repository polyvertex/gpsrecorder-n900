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
// Created On : 2010-04-22 07:39:18
//
// $Id$
//
//***************************************************************************

#ifndef __WNDCONFIG_H__
#define __WNDCONFIG_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndConfig
//---------------------------------------------------------------------------
class WndConfig : public QDialog
{
  Q_OBJECT

public :
  WndConfig (QWidget* pParent=0);
  virtual ~WndConfig (void);


private :
  void setupControls (void);


private slots :
  void onClickedDone (void);


private :
  MaemoComboBox* m_pCboLogStep;
  QCheckBox*     m_pChkGpsAssisted;
  QCheckBox*     m_pChkGpsAlwaysConnected;
  QCheckBox*     m_pChkAskTrackName;
  QCheckBox*     m_pChkAskPointName;
  QCheckBox*     m_pChkAskPauseName;
  MaemoComboBox* m_pCboUnitSystem;
  MaemoComboBox* m_pCboHorizSpeedUnit;
  QCheckBox*     m_pChkPreventBlankScreen;
};


#endif // #ifndef __WNDCONFIG_H__
