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
: WndBase(pParent)
{
  m_uiSpeedLastUpdate = 0;

  this->setWindowTitle(App::applicationLabel() + QString(" - ") + tr("Speed"));
  this->createWidgets();

  this->connect(
    App::instance(),
    SIGNAL(sigAppStatePixChanged(QPixmap*)),
    SLOT(onAppStatePixChanged(QPixmap*)) );

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

  {
    QFont font;

    font.setPixelSize(200);
    font.setBold(true);

    m_pLblSpeed = new QLabel("0.00");
    m_pLblSpeed->setEnabled(false);
    m_pLblSpeed->setAlignment(Qt::AlignCenter);
    m_pLblSpeed->setFont(font);
  }

  m_pLblStatusIcon = new QLabel();
  m_pLblStatusIcon->setFixedWidth(App::instance()->getStatePix()->width() + 20);
  m_pLblStatusIcon->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  m_pLblStatusIcon->setPixmap(*App::instance()->getStatePix());

  m_pLblSpeedUpdate = new QLabel();
  m_pLblSpeedUpdate->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

  m_pLblSpeedUnit = new QLabel();
  m_pLblSpeedUnit->setAlignment(Qt::AlignRight | Qt::AlignBottom);

  pGrid->setHorizontalSpacing(10);
  pGrid->addWidget(m_pLblSpeed,       0, 0, 1, 3);
  pGrid->addWidget(m_pLblStatusIcon,  1, 0);
  pGrid->addWidget(m_pLblSpeedUpdate, 1, 1);
  pGrid->addWidget(m_pLblSpeedUnit,   1, 2);
  pWidget->setLayout(pGrid);
  this->setCentralWidget(pWidget);
}



//---------------------------------------------------------------------------
// onAppStatePixChanged
//---------------------------------------------------------------------------
void WndSpeed::onAppStatePixChanged (QPixmap* pNewStatePixmap)
{
  m_pLblStatusIcon->setPixmap(*pNewStatePixmap);
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void WndSpeed::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  Q_UNUSED(pLocation);

  const LocationFix& fix = *pFixCont->getFix();
  AppSettings& settings = *App::instance()->settings();
  time_t uiNow = time(0);

  // update label only if necessary
  if (bAccurate && fix.hasFields(FIXFIELD_SPEED))
  {
    QDateTime dt;
    QString str;

    m_uiSpeedLastUpdate = uiNow;
    dt.setTime_t(uiNow);

    m_pLblSpeed->setEnabled(true);
    m_pLblSpeed->setText(QString::number(fix.getSpeed(settings.getHorizSpeedUnit()), 'f', 2));
    m_pLblSpeedUpdate->setText(
      QString("%1%2    %3%4")
      .arg(tr("Updated at "))
      .arg(dt.toString("hh:mm:ss"))
      .arg(QChar(L'\x00b1'))
      .arg(QString::number(fix.getSpeedEp(settings.getHorizSpeedUnit()), 'f', 2)) );
    m_pLblSpeedUnit->setText(fix.getSpeedSuffix(settings.getHorizSpeedUnit()));
  }
  else if (m_uiSpeedLastUpdate)
  {
    // here, data is too old

    m_pLblSpeed->setEnabled(false);
    m_pLblSpeedUpdate->setText(tr("LOST at ") + QDateTime::currentDateTime().toString("hh:mm:ss"));
    m_pLblSpeedUnit->setText(fix.getSpeedSuffix(settings.getHorizSpeedUnit()));

    m_uiSpeedLastUpdate = 0;
  }
}
