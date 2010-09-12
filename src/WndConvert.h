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
// Created On : 2010-04-20 15:39:18
//
// $Id$
//
//***************************************************************************

#ifndef __WNDCONVERT_H__
#define __WNDCONVERT_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndConvert
//---------------------------------------------------------------------------
class WndConvert : public QDialog
{
  Q_OBJECT

public :
  WndConvert (QWidget* pParent=0);
  virtual ~WndConvert (void);


private :
  void setupControls            (void);
  void refreshInputFilesControl (void);


private slots :
  void onClickedBrowseFiles  (void);
  void onClickedBrowseDir    (void);
  void onClickedKmlLineColor (void);
  void onStateChangedCsv     (int nNewState);
  void onStateChangedGpx     (int nNewState);
  void onStateChangedKml     (int nNewState);
  void onClickedConvert      (void);


private :
  QStringList m_InputFiles;

  QLineEdit* m_pTxtBrowse;
  QCheckBox* m_pChkExportPauses;

  // csv
  QCheckBox*     m_pChkCsv;
  MaemoGroupBox* m_pGroupBoxCsv;
  MaemoComboBox* m_pCboCsvSeparator;

  // gpx
  QCheckBox*     m_pChkGpx;
  MaemoGroupBox* m_pGroupBoxGpx;

  // kml
  QCheckBox*       m_pChkKml;
  MaemoGroupBox*   m_pGroupBoxKml;
  QCheckBox*       m_pChkKmlZipped;
  QPushButton*     m_pBtnKmlLineColor;
  QColor           m_KmlLineColor;
  MaemoComboBox*   m_pCboKmlLineWidth;
  QCheckBox*       m_pChkKmlAircraft;
};


#endif // #ifndef __WNDCONVERT_H__
