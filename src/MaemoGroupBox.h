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
// Created On : 2010-08-04 18:45:02
//
// $Id$
//
//***************************************************************************

#ifndef __MAEMOGROUPBOX_H__
#define __MAEMOGROUPBOX_H__

#include "stable.h"


//---------------------------------------------------------------------------
// MaemoGroupBox
//---------------------------------------------------------------------------
class MaemoGroupBox : public QVBoxLayout
{
  Q_OBJECT

public :
  MaemoGroupBox (QWidget* pParent=0);
  MaemoGroupBox (const QString& strText, QWidget* pParent=0);

  void setText    (const QString& strText);
  void setEnabled (bool bEnabled);

  QLabel* label (void) { return m_pLabel; }
  QFrame* frame (void) { return m_pFrame; }


private :
  void construct        (QWidget* pParent);
  void setLayoutEnabled (QLayout* pLayout, bool bEnabled);


private :
  QLabel* m_pLabel;
  QFrame* m_pFrame;
};


#endif // #ifndef __MAEMOGROUPBOX_H__
