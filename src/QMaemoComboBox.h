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
// Created On : 2010-08-04 17:51:17
//
// $Id$
//
//***************************************************************************

#ifndef __MAEMOCOMBOBOX_H__
#define __MAEMOCOMBOBOX_H__

#include "stable.h"


//---------------------------------------------------------------------------
// QMaemoComboBox
//---------------------------------------------------------------------------
class QMaemoComboBox : public QMaemo5ValueButton
{
  Q_OBJECT

public :
  QMaemoComboBox (QWidget* pParent=0);
  QMaemoComboBox (const QString& strText, QWidget* pParent=0);

  void setTextAlignment (Qt::Alignment eAlign);

  void addItem         (const QString& strText, const QVariant& userData=QVariant());
  void setCurrentIndex (int iNewIndex);

  int count        (void) const;
  int currentIndex (void) const;

  QString itemText           (int iIndex);
  QString currentItemText    (void);
  void    setItemText        (int iIndex, const QString& strText);
  void    setCurrentItemText (const QString& strText);

  QVariant itemData        (int iIndex);
  QVariant currentItemData (void);


signals :
  void sigSelected (int iIndex);


private :
  void construct (QWidget* pParent);


private slots :
  void onSelected             (const QString& strText);
  void onRefreshAfterSelected (void);


private :
  Qt::Alignment            m_eAlign;
  QStandardItemModel*      m_pModel;
  QMaemo5ListPickSelector* m_pSelector;
};


#endif // #ifndef __MAEMOCOMBOBOX_H__
