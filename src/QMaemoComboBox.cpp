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
// QMaemoComboBox
//---------------------------------------------------------------------------
QMaemoComboBox::QMaemoComboBox (QWidget* pParent/*=0*/)
: QMaemo5ValueButton(pParent)
{
  this->construct(pParent);
}

//---------------------------------------------------------------------------
// QMaemoComboBox
//---------------------------------------------------------------------------
QMaemoComboBox::QMaemoComboBox (const QString& strText, QWidget* pParent/*=0*/)
: QMaemo5ValueButton(strText, pParent)
{
  this->construct(pParent);
}

//---------------------------------------------------------------------------
// construct
//---------------------------------------------------------------------------
void QMaemoComboBox::construct (QWidget* pParent)
{
  m_eAlign = Qt::AlignCenter; // the Maemo5 design spec recommends this

  m_pModel = new QStandardItemModel(pParent);

  m_pSelector = new QMaemo5ListPickSelector;
  m_pSelector->setModel(m_pModel);
  this->connect(m_pSelector, SIGNAL(selected(const QString&)), SLOT(onSelected(const QString&)));

  this->setPickSelector(m_pSelector);
}



//---------------------------------------------------------------------------
// setTextAlignment
//---------------------------------------------------------------------------
void QMaemoComboBox::setTextAlignment (Qt::Alignment eAlign)
{
  m_eAlign = eAlign;
}

//---------------------------------------------------------------------------
// addItem
//---------------------------------------------------------------------------
void QMaemoComboBox::addItem (const QString& strText, const QVariant& userData/*=QVariant()*/)
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
int QMaemoComboBox::currentIndex (void) const
{
  return m_pSelector->currentIndex();
}

//---------------------------------------------------------------------------
// count
//---------------------------------------------------------------------------
int QMaemoComboBox::count (void) const
{
  return m_pModel->rowCount();
}

//---------------------------------------------------------------------------
// setCurrentIndex
//---------------------------------------------------------------------------
void QMaemoComboBox::setCurrentIndex (int iNewIndex)
{
  return m_pSelector->setCurrentIndex(iNewIndex);
}

//---------------------------------------------------------------------------
// itemText
//---------------------------------------------------------------------------
QString QMaemoComboBox::itemText (int iIndex)
{
  return
    (iIndex >= 0 || iIndex < this->count()) ?
    m_pModel->item(iIndex)->text() :
    QString();
}

//---------------------------------------------------------------------------
// currentItemText
//---------------------------------------------------------------------------
QString QMaemoComboBox::currentItemText (void)
{
  return this->itemText(m_pSelector->currentIndex());
}

//---------------------------------------------------------------------------
// setItemText
//---------------------------------------------------------------------------
void QMaemoComboBox::setItemText (int iIndex, const QString& strText)
{
  if (iIndex >= 0 || iIndex < this->count())
    m_pModel->item(iIndex)->setText(strText);
}

//---------------------------------------------------------------------------
// setCurrentItemText
//---------------------------------------------------------------------------
void QMaemoComboBox::setCurrentItemText (const QString& strText)
{
  this->setItemText(m_pSelector->currentIndex(), strText);
}

//---------------------------------------------------------------------------
// itemData
//---------------------------------------------------------------------------
QVariant QMaemoComboBox::itemData (int iIndex)
{
  return
    (iIndex >= 0 || iIndex < this->count()) ?
    m_pModel->item(iIndex)->data() :
    QVariant();
}

//---------------------------------------------------------------------------
// currentItemData
//---------------------------------------------------------------------------
QVariant QMaemoComboBox::currentItemData (void)
{
  return this->itemData(m_pSelector->currentIndex());
}



//---------------------------------------------------------------------------
// onSelected
//---------------------------------------------------------------------------
void QMaemoComboBox::onSelected (const QString& strText)
{
  Q_UNUSED(strText);

  this->sigSelected(this->currentIndex());

  // if a called slot has modified the text of the current item, the
  // valueText of this QMaemo5ValueButton widget will not be refreshed
  // automatically so we have to do it by ourselves.
  if (this->currentItemText() != strText)
    QTimer::singleShot(250, this, SLOT(onRefreshAfterSelected()));
}

//---------------------------------------------------------------------------
// onRefreshAfterSelected
//---------------------------------------------------------------------------
void QMaemoComboBox::onRefreshAfterSelected (void)
{
  this->setValueText(this->currentItemText());
}
