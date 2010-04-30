//***************************************************************************
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
  void onPushedDone (void);


private :
  QComboBox* m_pCboLogStep;
  QCheckBox* m_pChkGpsAssisted;
  QCheckBox* m_pChkGpsAlwaysConnected;
};


#endif // #ifndef __WNDCONFIG_H__
