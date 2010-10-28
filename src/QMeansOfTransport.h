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
// Created On : 2010-10-26 17:13:01
//
// $Id$
//
//***************************************************************************

#ifndef __QMEANSOFTRANSPORT_H__
#define __QMEANSOFTRANSPORT_H__

#include "stable.h"


//---------------------------------------------------------------------------
// QMeansOfTransport
//---------------------------------------------------------------------------
class QMeansOfTransport : public QMaemoComboBox
{
  Q_OBJECT

public :
  QMeansOfTransport (QWidget* pParent=0);

  bool selectCurrentMeansOfTransport (quint8 ucMeansOfTransport);
  void setOtherMeansOfTransport      (const QString& strOtherMOT);

  quint8  meansOfTransport      (void);
  QString otherMeansOfTransport (void) const { return m_strOtherMOT; }


  static QString buildLabel      (quint8 ucMeansOfTransport, const QString& strOtherMeansOfTransport);
  static QString buildOtherLabel (const QString& strOtherMeansOfTransport);


private slots :
  void onSelected (int iIndex);


private :
  QString m_strOtherMOT;
  int     m_iOtherMotIndex;
};


#endif // #ifndef __QMEANSOFTRANSPORT_H__
