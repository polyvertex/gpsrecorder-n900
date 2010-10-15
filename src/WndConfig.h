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
