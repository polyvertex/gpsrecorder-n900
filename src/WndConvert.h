//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex@gmail.com>
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
// Created: 2010-04-20 15:39:18
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
  void onStateChangedTxt     (int nNewState);
  void onStateChangedCsv     (int nNewState);
  void onStateChangedGpx     (int nNewState);
  void onStateChangedKml     (int nNewState);
  void onClickedConvert      (void);


private :
  QStringList m_InputFiles;

  QLineEdit* m_pTxtBrowse;
  QCheckBox* m_pChkExportPauses;

  // txt
  QCheckBox*      m_pChkTxt;
  QMaemoGroupBox* m_pGroupBoxTxt;
  QCheckBox*      m_pChkTxtIncludeLocFix;

  // csv
  QCheckBox*      m_pChkCsv;
  QMaemoGroupBox* m_pGroupBoxCsv;
  QMaemoComboBox* m_pCboCsvSeparator;

  // gpx
  QCheckBox*      m_pChkGpx;
  QMaemoGroupBox* m_pGroupBoxGpx;

  // kml
  QCheckBox*      m_pChkKml;
  QMaemoGroupBox* m_pGroupBoxKml;
  QCheckBox*      m_pChkKmlZipped;
  QPushButton*    m_pBtnKmlLineColor;
  QColor          m_KmlLineColor;
  QMaemoComboBox* m_pCboKmlLineWidth;
  QCheckBox*      m_pChkKmlAircraft;
};


#endif // #ifndef __WNDCONVERT_H__
