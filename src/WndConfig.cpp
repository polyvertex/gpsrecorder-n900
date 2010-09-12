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
// Created On : 2010-04-22 07:39:24
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndConfig
//---------------------------------------------------------------------------
WndConfig::WndConfig (QWidget* pParent/*=0*/)
: QDialog(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(pParent);

  this->setModal(true);
  this->setWindowTitle(tr("Settings"));

  this->setupControls();
}

//---------------------------------------------------------------------------
// ~WndConfig
//---------------------------------------------------------------------------
WndConfig::~WndConfig (void)
{
}



//---------------------------------------------------------------------------
// setupControls
//---------------------------------------------------------------------------
void WndConfig::setupControls (void)
{
  AppSettings& settings = *App::instance()->settings();
  QHBoxLayout* pRootLayout = new QHBoxLayout;
  QVBoxLayout* pLeftLayout = new QVBoxLayout;

  // options
  {
    QVBoxLayout* pVBox;
    uint uiConfiguredLogStep   = settings.getLogStep();
    uint auiProposedLogSteps[] = { 1, 2, 3, 4, 5, 10, 20, 30, 60, 120, 180, 240, 300, 600, 1200, 1800, 3600, 7200, 10800 };
    int  iDefaultLogStepIdx    = 4; // 5 seconds

    pVBox = new QVBoxLayout;
    pVBox->setSpacing(1);

    m_pCboLogStep = new MaemoComboBox(tr("Log step"), this);
    m_pCboLogStep->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    m_pCboLogStep->setTextAlignment(Qt::AlignLeft);

    for (int i = 0; i < sizeof(auiProposedLogSteps)/sizeof(auiProposedLogSteps[0]); ++i)
    {
      if (auiProposedLogSteps[i] >= AppSettings::logStepBounds().first ||
          auiProposedLogSteps[i] <= AppSettings::logStepBounds().second)
      {
        QString strItem;
        uint    uiBestAllowedFixStep = Location::selectBestAllowedFixStep(auiProposedLogSteps[i]);

        if (auiProposedLogSteps[i] < 60)
        {
          strItem.sprintf(
            "%u second%s (refresh : %us)",
            auiProposedLogSteps[i],
            (auiProposedLogSteps[i] == 1) ? "" : "s",
            uiBestAllowedFixStep);
        }
        else if (auiProposedLogSteps[i] < 3600)
        {
          uint uiMinutes = auiProposedLogSteps[i] / 60;

          strItem.sprintf(
            "%u minute%s (refresh : %us)",
            uiMinutes,
            (uiMinutes == 1) ? "" : "s",
            uiBestAllowedFixStep);
        }
        else
        {
          uint uiHours = auiProposedLogSteps[i] / 3600;

          strItem.sprintf(
            "%u hour%s (refresh : %us)",
            uiHours,
            (uiHours == 1) ? "" : "s",
            uiBestAllowedFixStep);
        }

        m_pCboLogStep->addItem(strItem, QVariant(auiProposedLogSteps[i]));
        if (auiProposedLogSteps[i] == uiConfiguredLogStep)
          iDefaultLogStepIdx = i;
      }
    }
    m_pCboLogStep->setCurrentIndex(iDefaultLogStepIdx);

    m_pChkGpsAssisted = new QCheckBox(tr("Assisted GPS"));
    m_pChkGpsAssisted->setCheckState(settings.getGpsAssisted() ? Qt::Checked : Qt::Unchecked);

    m_pChkGpsAlwaysConnected = new QCheckBox(tr("GPS always connected"));
    m_pChkGpsAlwaysConnected->setCheckState(settings.getGpsAlwaysConnected() ? Qt::Checked : Qt::Unchecked);

    m_pChkAskTrackName = new QCheckBox(tr("Ask for track name before create"));
    m_pChkAskTrackName->setCheckState(settings.getAskTrackName() ? Qt::Checked : Qt::Unchecked);

    m_pChkAskPointName = new QCheckBox(tr("Ask for point name before snap"));
    m_pChkAskPointName->setCheckState(settings.getAskPointName() ? Qt::Checked : Qt::Unchecked);

    m_pChkAskPauseName = new QCheckBox(tr("Ask for a name when pausing"));
    m_pChkAskPauseName->setCheckState(settings.getAskPauseName() ? Qt::Checked : Qt::Unchecked);

    m_pCboUnitSystem = new MaemoComboBox(tr("Unit system"), this);
    m_pCboUnitSystem->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    m_pCboUnitSystem->addItem(AppSettings::unitSystemToName(UNITSYSTEM_METRIC),    QVariant(UNITSYSTEM_METRIC));
    m_pCboUnitSystem->addItem(AppSettings::unitSystemToName(UNITSYSTEM_IMPERIAL),  QVariant(UNITSYSTEM_IMPERIAL));
    m_pCboUnitSystem->setCurrentIndex(settings.getUnitSystem());

    m_pCboHorizSpeedUnit = new MaemoComboBox(tr("Horizontal speed unit"), this);
    m_pCboHorizSpeedUnit->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    m_pCboHorizSpeedUnit->addItem(AppSettings::horizSpeedUnitToName(HORIZSPEEDUNIT_KMH),   QVariant(HORIZSPEEDUNIT_KMH));
    m_pCboHorizSpeedUnit->addItem(AppSettings::horizSpeedUnitToName(HORIZSPEEDUNIT_MPH),   QVariant(HORIZSPEEDUNIT_MPH));
    m_pCboHorizSpeedUnit->addItem(AppSettings::horizSpeedUnitToName(HORIZSPEEDUNIT_MS),    QVariant(HORIZSPEEDUNIT_MS));
    m_pCboHorizSpeedUnit->addItem(AppSettings::horizSpeedUnitToName(HORIZSPEEDUNIT_KNOTS), QVariant(HORIZSPEEDUNIT_KNOTS));
    m_pCboHorizSpeedUnit->setCurrentIndex(settings.getHorizSpeedUnit());

    m_pChkPreventBlankScreen = new QCheckBox(tr("Prevent screen blanking"));
    m_pChkPreventBlankScreen->setCheckState(settings.getPreventBlankScreen() ? Qt::Checked : Qt::Unchecked);

    pVBox->addWidget(m_pCboLogStep);
    pVBox->addWidget(m_pChkGpsAssisted);
    pVBox->addWidget(m_pChkGpsAlwaysConnected);
    pVBox->addWidget(m_pChkAskTrackName);
    pVBox->addWidget(m_pChkAskPointName);
    pVBox->addWidget(m_pChkAskPauseName);
    pVBox->addWidget(m_pCboUnitSystem);
    pVBox->addWidget(m_pCboHorizSpeedUnit);
    pVBox->addWidget(m_pChkPreventBlankScreen);
    pLeftLayout->addLayout(pVBox);
  }

  // main layout setup
  {
    QScrollArea* pScrollArea = new QScrollArea;
    QWidget*     pScrollWidget = new QWidget;
    QPushButton* pBtnDone = new QPushButton(tr("Done"));

    this->connect(pBtnDone, SIGNAL(clicked()), SLOT(onClickedDone()));

    pLeftLayout->setSpacing(0);
    pScrollWidget->setLayout(pLeftLayout);

    pScrollArea->setWidgetResizable(true);
    pScrollArea->setWidget(pScrollWidget);
    pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pScrollArea->setProperty("FingerScrollable", true);

    pRootLayout->setSpacing(1);
    pRootLayout->addWidget(pScrollArea);
    pRootLayout->addWidget(pBtnDone, 0, Qt::AlignBottom);
  }

  // apply layout
  this->setLayout(pRootLayout);
}



//---------------------------------------------------------------------------
// onClickedDone
//---------------------------------------------------------------------------
void WndConfig::onClickedDone (void)
{
  AppSettings& settings = *App::instance()->settings();

  settings.setLogStep(m_pCboLogStep->currentItemData().toUInt());
  settings.setGpsAssisted(m_pChkGpsAssisted->checkState() != Qt::Unchecked);
  settings.setGpsAlwaysConnected(m_pChkGpsAlwaysConnected->checkState() != Qt::Unchecked);
  settings.setAskTrackName(m_pChkAskTrackName->checkState() != Qt::Unchecked);
  settings.setAskPointName(m_pChkAskPointName->checkState() != Qt::Unchecked);
  settings.setAskPauseName(m_pChkAskPauseName->checkState() != Qt::Unchecked);
  settings.setUnitSystem(m_pCboUnitSystem->currentItemData().toUInt());
  settings.setHorizSpeedUnit(m_pCboHorizSpeedUnit->currentItemData().toUInt());
  settings.setPreventBlankScreen(m_pChkPreventBlankScreen->checkState() != Qt::Unchecked);

  settings.write();
  this->done(0);
}
