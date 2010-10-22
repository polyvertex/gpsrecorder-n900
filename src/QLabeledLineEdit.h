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
// Created On : 2010-10-22 14:37:43
//
// $Id$
//
//***************************************************************************

#ifndef __QLABELEDLINEEDIT_H__
#define __QLABELEDLINEEDIT_H__

#include "stable.h"


//---------------------------------------------------------------------------
// QLabeledLineEdit
//
// This is a utility class to allow displaying a grayed out dummy label when
// the 'real' content is empty.
// As soon as user is typing something in this widget, the label will vanish
// to let real content to be displayed.
// This works barely like 'search' input controls on some websites except for
// the fact that dummy content do not vanish when widget gains focus : only
// when content is not empty.
//
// CAUTION : To ensure you get the content value, you *must* use the
// content() method instead of the regular QLineEdit::text() method because
// the latest will sometimes return the label() value !
//---------------------------------------------------------------------------
class QLabeledLineEdit : public QLineEdit
{
  Q_OBJECT

public :
  QLabeledLineEdit (QWidget* pParent=0);
  QLabeledLineEdit (const QString& strContent, QWidget* pParent=0);
  QLabeledLineEdit (const QString& strLabel, const QString& strContent, QWidget* pParent=0);

  QString label    (void) const { return m_strLabel; }
  void    setLabel (const QString& strLabel);
  QString content  (void) const { return m_strContent; }


private :
  void updateDisplay (void);


protected :
  // from QWidget
  void keyPressEvent (QKeyEvent* pKeyEvent);
  void focusOutEvent (QFocusEvent* pFocusEvent);


private slots :
  void onTextChanged_QLLE (const QString& strText);


private :
  QString m_strLabel;
  QString m_strContent;
};


#endif // #ifndef __QLABELEDLINEEDIT_H__
