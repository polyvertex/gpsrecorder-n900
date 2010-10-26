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

#include "stable.h"


//---------------------------------------------------------------------------
// QMeansOfTransportation
//---------------------------------------------------------------------------
QMeansOfTransportation::QMeansOfTransportation (QWidget* pParent/*=0*/)
: QMaemoComboBox(tr("Means of transportation"), pParent)
{
  AppSettings& settings = *App::instance()->settings();
  QString strOtherLabel;

  strOtherLabel = tr("Other : ");
  if (settings.getLastOtherMeansOfTransport().isEmpty())
  {
    strOtherLabel += '?';
  }
  else
  {
    strOtherLabel +=
      QString("\"%1\"")
      .arg(settings.getLastOtherMeansOfTransport().constData());
  }

  this->addItem(tr("N/A"),       (int)GPSRFile::MEANSTRANSPORT__unused__);
  this->addItem(tr("Foot"),      (int)GPSRFile::MEANSTRANSPORT_FOOT);
  this->addItem(tr("Roller"),    (int)GPSRFile::MEANSTRANSPORT_ROLLER);
  this->addItem(tr("Bike"),      (int)GPSRFile::MEANSTRANSPORT_BIKE);
  this->addItem(tr("Motorbike"), (int)GPSRFile::MEANSTRANSPORT_MOTORBIKE);
  this->addItem(tr("Car"),       (int)GPSRFile::MEANSTRANSPORT_CAR);
  this->addItem(tr("Boat"),      (int)GPSRFile::MEANSTRANSPORT_BOAT);
  this->addItem(tr("Plane"),     (int)GPSRFile::MEANSTRANSPORT_PLANE);
  this->addItem(strOtherLabel,   (int)GPSRFile::MEANSTRANSPORT_OTHER);

  this->connect(this, SIGNAL(sigSelected(int)), SLOT(onSelected(int)));
  this->setValueLayout(QMaemo5ValueButton::ValueUnderText);
  this->setCurrentIndex(0);
}



//---------------------------------------------------------------------------
// setMeansOfTransportation
//---------------------------------------------------------------------------
bool QMeansOfTransportation::setMeansOfTransportation (quint8 ucMeansOfTransportation)
{
  for (int i = 0; i < this->count(); ++i)
  {
    if (this->itemData(i).toInt() == (int)ucMeansOfTransportation)
    {
      this->setCurrentIndex(0);
      return true;
    }
  }

  return false;
}

//---------------------------------------------------------------------------
// getMeansOfTransportation
//---------------------------------------------------------------------------
quint8 QMeansOfTransportation::getMeansOfTransportation (void)
{
  return (quint8)this->currentItemData().toInt();
}

//---------------------------------------------------------------------------
// getOtherMeansOfTransportation
//---------------------------------------------------------------------------
QString QMeansOfTransportation::getOtherMeansOfTransportation (void)
{
  if ((this->getMeansOfTransportation() == GPSRFile::MEANSTRANSPORT_OTHER) &&
    App::instance() &&
    App::instance()->settings())
  {
    return App::instance()->settings()->getLastOtherMeansOfTransport().constData();
  }

  return QString();
}



//---------------------------------------------------------------------------
// onSelected
//---------------------------------------------------------------------------
void QMeansOfTransportation::onSelected (int iIndex)
{
  static int iLastIndex = -1;

  if (this->itemData(iIndex).toInt() == (int)GPSRFile::MEANSTRANSPORT_OTHER)
  {
    // here, user wants to specify its own means of transportation

    AppSettings& settings = *App::instance()->settings();
    QString      strInput;
    bool         bOk = false;

    strInput = QInputDialog::getText(
      static_cast<QWidget*>(this->parent()),
      tr("Name ?"),
      tr("Please enter your means of transportation or leave blank :"),
      QLineEdit::Normal,
      settings.getLastOtherMeansOfTransport().constData(),
      &bOk);

    if (bOk)
    {
      settings.setLastOtherMeansOfTransport(qPrintable(strInput));

      this->setItemText(
        iIndex,
        QString(tr("Other : \"%1\"")).arg(settings.getLastOtherMeansOfTransport().constData()));
    }
    else if (iLastIndex >= 0)
    {
      this->setCurrentIndex(iLastIndex);
    }
  }

  iLastIndex = iIndex;
}
