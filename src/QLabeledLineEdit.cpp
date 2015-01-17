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
// Created: 2010-10-22 14:37:43
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// QLabeledLineEdit
//---------------------------------------------------------------------------
QLabeledLineEdit::QLabeledLineEdit (QWidget* pParent/*=0*/)
: QLineEdit(pParent)
{
  this->updateDisplay();
}

//---------------------------------------------------------------------------
// QLabeledLineEdit
//---------------------------------------------------------------------------
QLabeledLineEdit::QLabeledLineEdit (const QString& strContent, QWidget* pParent/*=0*/)
: QLineEdit(pParent)
{
  m_strContent = strContent;

  this->updateDisplay();
}

//---------------------------------------------------------------------------
// QLabeledLineEdit
//---------------------------------------------------------------------------
QLabeledLineEdit::QLabeledLineEdit (const QString& strLabel, const QString& strContent, QWidget* pParent/*=0*/)
: QLineEdit(pParent)
{
  m_strLabel   = strLabel;
  m_strContent = strContent;

  this->updateDisplay();
}

//---------------------------------------------------------------------------
// setLabel
//---------------------------------------------------------------------------
void QLabeledLineEdit::setLabel (const QString& strLabel)
{
  m_strLabel = strLabel;
  this->updateDisplay();
}

//---------------------------------------------------------------------------
// updateDisplay
//---------------------------------------------------------------------------
void QLabeledLineEdit::updateDisplay (void)
{
  static QString strStyleNormalColor;

  if (strStyleNormalColor.isEmpty())
  {
    QRegExp rx("color:\\s*([^ ;\\n\\t}]+)");

    if (rx.indexIn(this->styleSheet()) >= 0)
    {
      QStringList matches = rx.capturedTexts();
      if (matches.size() >= 2)
        strStyleNormalColor = matches[1];
    }

    if (strStyleNormalColor.isEmpty())
      strStyleNormalColor = "black";
  }

  while (this->disconnect(SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged_QLLE(const QString&)))) { ; }

  if (m_strContent.isEmpty() && !m_strLabel.isEmpty())
  {
    this->setText(m_strLabel);
    this->setStyleSheet("color:grey");

    if (this->hasFocus())
      this->selectAll();
  }
  else
  {
    this->setText(m_strContent);
    this->setStyleSheet(QString("color:") + strStyleNormalColor);
  }

  this->connect(
    this,
    SIGNAL(textChanged(const QString&)),
    SLOT(onTextChanged_QLLE(const QString&)));
}

//---------------------------------------------------------------------------
// keyPressEvent
//---------------------------------------------------------------------------
void QLabeledLineEdit::keyPressEvent (QKeyEvent* pKeyEvent)
{
  this->updateDisplay();
  QLineEdit::keyPressEvent(pKeyEvent);
}

//---------------------------------------------------------------------------
// focusOutEvent
//---------------------------------------------------------------------------
void QLabeledLineEdit::focusOutEvent (QFocusEvent* pFocusEvent)
{
  pFocusEvent->accept(); // not sure if it is required...
  this->updateDisplay();
  QLineEdit::focusOutEvent(pFocusEvent);
}

//---------------------------------------------------------------------------
// onTextChanged_QLLE
//---------------------------------------------------------------------------
void QLabeledLineEdit::onTextChanged_QLLE (const QString& strText)
{
  m_strContent = strText;
  this->updateDisplay();
}
