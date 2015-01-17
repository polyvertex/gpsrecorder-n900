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
// Created: 2010-08-04 18:45:07
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// QMaemoGroupBox
//---------------------------------------------------------------------------
QMaemoGroupBox::QMaemoGroupBox (QWidget* pParent/*=0*/)
: QVBoxLayout(pParent)
{
  this->construct(pParent);
}

//---------------------------------------------------------------------------
// QMaemoGroupBox
//---------------------------------------------------------------------------
QMaemoGroupBox::QMaemoGroupBox (const QString& strText, QWidget* pParent/*=0*/)
: QVBoxLayout(pParent)
{
  this->construct(pParent);
  this->setText(strText);
}

//---------------------------------------------------------------------------
// construct
//---------------------------------------------------------------------------
void QMaemoGroupBox::construct (QWidget* pParent)
{
  m_pLabel = new QLabel;
  m_pLabel->setAlignment(Qt::AlignHCenter);

  m_pFrame = new QFrame;
  m_pFrame->setFrameShape(QFrame::HLine);

  this->setSpacing(1);
  this->addWidget(m_pLabel);
  this->addWidget(m_pFrame);
}



//---------------------------------------------------------------------------
// setText
//---------------------------------------------------------------------------
void QMaemoGroupBox::setText (const QString& strText)
{
  m_pLabel->setText(strText);
}

//---------------------------------------------------------------------------
// setEnabled
//---------------------------------------------------------------------------
void QMaemoGroupBox::setEnabled (bool bEnabled)
{
  this->setLayoutEnabled(this, bEnabled);
}



//---------------------------------------------------------------------------
// setLayoutEnabled
//---------------------------------------------------------------------------
void QMaemoGroupBox::setLayoutEnabled (QLayout* pLayout, bool bEnabled)
{
  // recursively enable/disable all widgets and layouts inside this layout
  for (int i = 0; i < pLayout->count(); ++i)
  {
    QLayoutItem* pLayoutItem = pLayout->itemAt(i);

    if (pLayoutItem->widget())
      pLayoutItem->widget()->setEnabled(bEnabled);
    else if (pLayoutItem->layout())
      this->setLayoutEnabled(pLayoutItem->layout(), bEnabled);
  }
}
