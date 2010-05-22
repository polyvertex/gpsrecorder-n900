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

  m_uiSpeedLastUpdate = 0;

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
  QGridLayout* pGrid    = new QGridLayout();
  QLabel*      pLblUnit;

  {
    QFont font;

    font.setPixelSize(200);
    font.setBold(true);

    m_pLblSpeed = new QLabel("0.00");
    m_pLblSpeed->setEnabled(false);
    m_pLblSpeed->setAlignment(Qt::AlignCenter);
    m_pLblSpeed->setFont(font);
  }

  m_pLblSpeedUpdate = new QLabel();
  m_pLblSpeedUpdate->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

  pLblUnit = new QLabel("km/h");
  pLblUnit->setAlignment(Qt::AlignRight | Qt::AlignBottom);

  pGrid->addWidget(m_pLblSpeed,       0, 0, 1, 2);
  pGrid->addWidget(m_pLblSpeedUpdate, 1, 0);
  pGrid->addWidget(pLblUnit,          1, 1);
  pWidget->setLayout(pGrid);
  this->setCentralWidget(pWidget);
}



//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void WndSpeed::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  Q_UNUSED(pLocation);

  const LocationFix& fix = *pFixCont->getFix();
  time_t uiNow = time(0);

  // update label only if necessary
  if (bAccurate && fix.hasFields(FIXFIELD_SPEED))
  {
    QDateTime dt;
    QString str;

    m_uiSpeedLastUpdate = uiNow;
    dt.setTime_t(uiNow);

    m_pLblSpeed->setEnabled(true);
    m_pLblSpeed->setText(QString::number(fix.getSpeedKmh(), 'f', 2));
    m_pLblSpeedUpdate->setText(
      QString("%1%2    %3%4")
      .arg(tr("Updated at "))
      .arg(dt.toString("hh:mm:ss"))
      .arg(QChar(L'\x00b1'))
      .arg(QString::number(fix.getSpeedKmhEp(), 'f', 2)) );
  }
  else if (m_uiSpeedLastUpdate)
  {
    // here, data is too old

    m_pLblSpeed->setEnabled(false);
    m_pLblSpeedUpdate->setText(tr("LOST at ") + QDateTime::currentDateTime().toString("hh:mm:ss"));

    m_uiSpeedLastUpdate = 0;
  }
}
