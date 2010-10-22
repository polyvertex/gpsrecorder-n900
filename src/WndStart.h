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
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-10-20 18:33:21
//
// $Id$
//
//***************************************************************************

#ifndef __WNDSTART_H__
#define __WNDSTART_H__

#include "stable.h"


//---------------------------------------------------------------------------
// WndStart
//---------------------------------------------------------------------------
class WndStart : public QDialog
{
  Q_OBJECT

public :
  enum StartMode
  {
    STARTMODE_NEWTRACK,
    STARTMODE_APPENDTRACK,
  };


public :
  WndStart (QWidget* pParent=0);
  virtual ~WndStart (void);

  // run dialog
  // use this method instead of QDialog::exec() !
  bool doExec (void);

  // result available for all start modes
  StartMode      startMode                 (void) const { return m_eStartMode; }
  const QString& trackName                 (void) const { return m_strTrackName; }
  const QString& filePath                  (void) const { return m_strFilePath; }
  quint8         meansOfTransport          (void) const { return m_ucMeansOfTransport; }
  const QString& otherMeansOfTransportName (void) const { return m_strOtherMeansOfTransport; }


private :
  void setupControls (void);


private slots :
  void onClickedBrowseFile        (void);
  void onSelectedMeansOfTransport (int iIndex);
  void onClickedStart             (void);


private :
  StartMode m_eStartMode;

  // result
  bool    m_bCanceled;
  QString m_strTrackName;
  QString m_strFilePath;
  quint8  m_ucMeansOfTransport;
  QString m_strOtherMeansOfTransport;

  // dialog widgets
  QLineEdit*      m_pTxtTrackName;
  QMaemoComboBox* m_pCboMeansOfTransport;
  QLineEdit*      m_pTxtFilePath;
};


#endif // #ifndef __WNDSTART_H__
