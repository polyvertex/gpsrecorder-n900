//***************************************************************************
//
// GPS Recorder
// Show, log and convert GPS data from Maemo's GPS subsystem.
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
// Created On : 2010-05-12 10:17:07
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define MAXSATS         16
#define MAXSATS_COLUMN  (MAXSATS / 2)



//---------------------------------------------------------------------------
// WndSat
//---------------------------------------------------------------------------
WndSat::WndSat (QMainWindow* pParent/*=0*/)
: QMainWindow(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(App::instance()->location());

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
// ~WndSat
//---------------------------------------------------------------------------
WndSat::~WndSat (void)
{
}



//---------------------------------------------------------------------------
// createWidgets
//---------------------------------------------------------------------------
void WndSat::createWidgets (void)
{
  QWidget* pWidget = new QWidget();
  QGridLayout* pGrid = new QGridLayout();

  pGrid->setHorizontalSpacing(15);

  for (int i = 0; i < MAXSATS; ++i)
  {
    QProgressBar* pPBar = new QProgressBar();

    pPBar->setEnabled(false);
    pPBar->setVisible(false);

    pPBar->setMinimumSize(0, 70);
    pPBar->setMinimum(0);
    pPBar->setMaximum(100);
    pPBar->setValue(0);

    m_vecSatsPB.append(pPBar);
    pGrid->addWidget(pPBar, (i % MAXSATS_COLUMN), (i < MAXSATS_COLUMN ? 0 : 1));
  }

  pWidget->setLayout(pGrid);
  this->setCentralWidget(pWidget);
}



//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void WndSat::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  Q_UNUSED(pLocation);
  Q_UNUSED(bAccurate);

  const LocationFix& fix = *pFixCont->getFix();

  this->setUpdatesEnabled(false);
  this->setWindowTitle(App::applicationLabel() + QString(" - %1/%2 sat").arg((int)fix.cSatUse).arg((int)fix.cSatCount));

  while ((int)fix.cSatCount > m_vecSats.count())
  {
    m_vecSats.append(LocationFixSat());
    memset(&m_vecSats.last(), 0, sizeof(m_vecSats.last()));
  }

  for (quint8 cSatIdx = 0; cSatIdx < fix.cSatCount; ++cSatIdx)
  {
    const LocationFixSat& fixSat = *fix.getSat(cSatIdx);

    if (memcmp(&m_vecSats[cSatIdx], &fixSat, sizeof(fixSat)) == 0)
      continue;
    m_vecSats[cSatIdx] = fixSat;

    if (cSatIdx < MAXSATS)
    {
      m_vecSatsPB[cSatIdx]->setVisible(true);
      m_vecSatsPB[cSatIdx]->setEnabled(fixSat.bInUse ? true : false); // this has no visual effect on Maemo5
      m_vecSatsPB[cSatIdx]->setValue((int)fixSat.cSignalStrength);
      m_vecSatsPB[cSatIdx]->setFormat(
        QString("#%1  %2 dBHz%3")
        .arg(fixSat.iPRN)
        .arg((int)fixSat.cSignalStrength)
        .arg(fixSat.bInUse ? QString("  (%3°, %4°)").arg((int)fixSat.cElevation).arg((int)fixSat.wAzimuth) : "") );
    }
  }

  for (int i = (int)fix.cSatCount; i < MAXSATS; ++i)
    m_vecSatsPB[i]->setVisible(false);

  this->setUpdatesEnabled(true);
}
