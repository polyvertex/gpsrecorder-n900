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
// Created On : 2010-05-23 07:48:50
//
// $Id$
//
//***************************************************************************

#ifndef __WNDCELL_H__
#define __WNDCELL_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndCell
//---------------------------------------------------------------------------
class WndCell : public QMainWindow
{
  Q_OBJECT

public :
  WndCell (QMainWindow* pParent=0);
  virtual ~WndCell (void);


private :
  void createWidgets (void);


private slots :
  void onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


private :
  LocationFixCellInfoGsm   m_Gsm;
  LocationFixCellInfoWcdma m_Wcdma;

  QLabel* m_pLblCellModeIcon;

  QFormLayout* m_pForm;
  QLabel*      m_pLblCellMode; // 0: "gsm" or "wcdma"
  QLabel*      m_pLblMCC;      // 1: gsm/wcdma
  QLabel*      m_pLblMNC;      // 2: gsm/wcdma
  QLabel*      m_pLblLAC;      // 3: gsm only
  QLabel*      m_pLblCellId;   // 4: gsm only
  QLabel*      m_pLblUCID;     // 5: wcdma only

  QLabel* m_pLblStatusIcon;
  QLabel* m_pLblCellModeUpdate;
};


#endif // #ifndef __WNDCELL_H__
