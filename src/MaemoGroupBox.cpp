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
// Created On : 2010-08-04 18:45:07
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// MaemoGroupBox
//---------------------------------------------------------------------------
MaemoGroupBox::MaemoGroupBox (QWidget* pParent/*=0*/)
: QVBoxLayout(pParent)
{
  this->construct(pParent);
}

//---------------------------------------------------------------------------
// MaemoGroupBox
//---------------------------------------------------------------------------
MaemoGroupBox::MaemoGroupBox (const QString& strText, QWidget* pParent/*=0*/)
: QVBoxLayout(pParent)
{
  this->construct(pParent);
  this->setText(strText);
}

//---------------------------------------------------------------------------
// construct
//---------------------------------------------------------------------------
void MaemoGroupBox::construct (QWidget* pParent)
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
void MaemoGroupBox::setText (const QString& strText)
{
  m_pLabel->setText(strText);
}

//---------------------------------------------------------------------------
// setEnabled
//---------------------------------------------------------------------------
void MaemoGroupBox::setEnabled (bool bEnabled)
{
  this->setLayoutEnabled(this, bEnabled);
}



//---------------------------------------------------------------------------
// setLayoutEnabled
//---------------------------------------------------------------------------
void MaemoGroupBox::setLayoutEnabled (QLayout* pLayout, bool bEnabled)
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
