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
  void onPushedBrowse  (void);
  void onPushedConvert (void);


private :
  QStringList m_InputFiles;

  QLineEdit* m_pTxtBrowse;
  QCheckBox* m_pChkCsv;
  QCheckBox* m_pChkGpx;
  QCheckBox* m_pChkKml;
};


#endif // #ifndef __WNDCONVERT_H__
