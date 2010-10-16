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
: WndBase(pParent)
{
  this->setWindowTitle(App::applicationLabel() + QString(" - ") + tr("Satellites"));
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
