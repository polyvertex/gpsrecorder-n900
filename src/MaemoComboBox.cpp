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
  m_eAlign = Qt::AlignCenter; // the Maemo5 design spec recommends this

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
