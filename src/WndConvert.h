//***************************************************************************
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
  void setupControls (void);


private slots :
  void onClickedBrowseFiles  (void);
  void onClickedBrowseDir    (void);
  void onClickedKmlLineColor (void);
  void onClickedConvert      (void);


private :
  QStringList m_InputFiles;

  QLineEdit* m_pTxtBrowse;

  // csv
  QGroupBox* m_pGroupBoxCsv;
  QComboBox* m_pCboCsvSeparator;

  // gpx
  QGroupBox* m_pGroupBoxGpx;

  // kml
  QGroupBox*   m_pGroupBoxKml;
  QPushButton* m_pBtnKmlLineColor;
  QColor       m_KmlLineColor;
  QComboBox*   m_pCboKmlLineWidth;
  QCheckBox*   m_pChkKmlAircraft;
};


#endif // #ifndef __WNDCONVERT_H__
