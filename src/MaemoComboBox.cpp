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
// Created On : 2010-08-04 17:51:21
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// MaemoComboBox
//---------------------------------------------------------------------------
MaemoComboBox::MaemoComboBox (QWidget* pParent/*=0*/)
: QMaemo5ValueButton(pParent)
{
  this->construct(pParent);
}

//---------------------------------------------------------------------------
// MaemoComboBox
//---------------------------------------------------------------------------
MaemoComboBox::MaemoComboBox (const QString& strText, QWidget* pParent/*=0*/)
: QMaemo5ValueButton(strText, pParent)
{
  this->construct(pParent);
}

//---------------------------------------------------------------------------
// construct
//---------------------------------------------------------------------------
void MaemoComboBox::construct (QWidget* pParent)
{
  m_eAlign = Qt::AlignLeft;

  m_pModel = new QStandardItemModel(pParent);

  m_pSelector = new QMaemo5ListPickSelector;
  m_pSelector->setModel(m_pModel);

  this->setPickSelector(m_pSelector);
}



//---------------------------------------------------------------------------
// setTextAlignment
//---------------------------------------------------------------------------
void MaemoComboBox::setTextAlignment (Qt::Alignment eAlign)
{
  m_eAlign = eAlign;
}

//---------------------------------------------------------------------------
// addItem
//---------------------------------------------------------------------------
void MaemoComboBox::addItem (const QString& strText, const QVariant& userData/*=QVariant()*/)
{
  QStandardItem* pItem = new QStandardItem;

  pItem->setText(strText);
  pItem->setTextAlignment(m_eAlign);
  pItem->setEditable(false);
  pItem->setData(userData);

  m_pModel->appendRow(pItem);
}

//---------------------------------------------------------------------------
// currentIndex
//---------------------------------------------------------------------------
int MaemoComboBox::currentIndex (void) const
{
  return m_pSelector->currentIndex();
}

//---------------------------------------------------------------------------
// count
//---------------------------------------------------------------------------
int MaemoComboBox::count (void) const
{
  return m_pModel->rowCount();
}

//---------------------------------------------------------------------------
// setCurrentIndex
//---------------------------------------------------------------------------
void MaemoComboBox::setCurrentIndex (int iNewIndex)
{
  return m_pSelector->setCurrentIndex(iNewIndex);
}

//---------------------------------------------------------------------------
// itemData
//---------------------------------------------------------------------------
QVariant MaemoComboBox::itemData (int iIndex)
{
  return
    (iIndex >= 0 || iIndex < this->count()) ?
    m_pModel->item(iIndex)->data() :
    QVariant();
}

//---------------------------------------------------------------------------
// currentItemData
//---------------------------------------------------------------------------
QVariant MaemoComboBox::currentItemData (void)
{
  return this->itemData(m_pSelector->currentIndex());
}
