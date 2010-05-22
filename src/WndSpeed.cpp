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
// Created On : 2010-05-22 10:04:27
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndSpeed
//---------------------------------------------------------------------------
WndSpeed::WndSpeed (QMainWindow* pParent/*=0*/)
: QMainWindow(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(App::instance()->location());

  this->setWindowTitle(App::applicationLabel() + QString(" - ") + tr("Speed"));
#if QT_VERSION > 0x040503
  this->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
  this->createWidgets();

  this->connect(
    App::instance()->location(),
    SIGNAL(sigLocationFix(Location*, const LocationFixContainer*, bool)),
    SLOT(onLocationFix(Location*, const LocationFixContainer*, bool)) );
}

//---------------------------------------------------------------------------
// ~WndSpeed
//---------------------------------------------------------------------------
WndSpeed::~WndSpeed (void)
{
}



//---------------------------------------------------------------------------
// createWidgets
//---------------------------------------------------------------------------
void WndSpeed::createWidgets (void)
{
  QWidget*     pWidget  = new QWidget();
  QVBoxLayout* pVBox    = new QVBoxLayout();
  QLabel*      pLblUnit;

  pLblUnit = new QLabel("km/h");
  pLblUnit->setAlignment(Qt::AlignRight | Qt::AlignBottom);

  {
    QFont font;

    font.setPixelSize(200);
    font.setBold(true);

    m_pLblSpeed = new QLabel("0.00");
    m_pLblSpeed->setAlignment(Qt::AlignCenter);
    m_pLblSpeed->setFont(font);
  }

  pVBox->addWidget(m_pLblSpeed);
  pVBox->addWidget(pLblUnit);
  pWidget->setLayout(pVBox);
  this->setCentralWidget(pWidget);
}



//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void WndSpeed::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  Q_UNUSED(pLocation);
  Q_UNUSED(bAccurate);

  const LocationFix& fix = *pFixCont->getFix();
  m_pLblSpeed->setText(QString::number((fix.hasFields(FIXFIELD_SPEED) ? fix.getSpeedKmh() : 0.0), 'f', 2));
}
