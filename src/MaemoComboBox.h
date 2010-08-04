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
// Created On : 2010-08-04 17:51:17
//
// $Id$
//
//***************************************************************************

#ifndef __MAEMOCOMBOBOX_H__
#define __MAEMOCOMBOBOX_H__

#include "stable.h"


//---------------------------------------------------------------------------
// MaemoComboBox
//---------------------------------------------------------------------------
class MaemoComboBox : public QMaemo5ValueButton
{
  Q_OBJECT

public :
  MaemoComboBox (QWidget* pParent=0);
  MaemoComboBox (const QString& strText, QWidget* pParent=0);

  void setTextAlignment (Qt::Alignment eAlign);

  void addItem         (const QString& strText, const QVariant& userData=QVariant());
  void setCurrentIndex (int iNewIndex);

  int      count           (void) const;
  int      currentIndex    (void) const;
  QVariant itemData        (int iIndex);
  QVariant currentItemData (void);


private :
  void construct (QWidget* pParent);


private :
  Qt::Alignment            m_eAlign;
  QStandardItemModel*      m_pModel;
  QMaemo5ListPickSelector* m_pSelector;
};


#endif // #ifndef __MAEMOCOMBOBOX_H__
