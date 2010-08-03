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
    QVBoxLayout*             pVBox = new QVBoxLayout;
    QStandardItemModel*      pCboLogStepItemModel = new QStandardItemModel(this);
    QMaemo5ListPickSelector* pCboLogStepSelector;
    uint uiConfiguredLogStep   = settings.getLogStep();
    uint auiProposedLogSteps[] = { 1, 2, 3, 4, 5, 10, 20, 30, 60, 120, 180, 240, 300, 600, 1200, 1800, 3600, 7200, 10800 };
    int  iDefaultLogStepIdx    = 4; // 5 seconds

    for (int i = 0; i < sizeof(auiProposedLogSteps)/sizeof(auiProposedLogSteps[0]); ++i)
    {
      if (auiProposedLogSteps[i] >= AppSettings::logStepBounds().first ||
          auiProposedLogSteps[i] <= AppSettings::logStepBounds().second)
      {
        QStandardItem* pItem = new QStandardItem;
        QString        strItem;
        uint           uiBestAllowedFixStep = Location::selectBestAllowedFixStep(auiProposedLogSteps[i]);

        if (auiProposedLogSteps[i] < 60)
        {
          strItem.sprintf(
            "%u second%s (%u)",
            auiProposedLogSteps[i],
            (auiProposedLogSteps[i] == 1) ? "" : "s",
            uiBestAllowedFixStep);
        }
        else if (auiProposedLogSteps[i] < 3600)
        {
          uint uiMinutes = auiProposedLogSteps[i] / 60;

          strItem.sprintf(
            "%u minute%s (%u)",
            uiMinutes,
            (uiMinutes == 1) ? "" : "s",
            uiBestAllowedFixStep);
        }
        else
        {
          uint uiHours = auiProposedLogSteps[i] / 3600;

          strItem.sprintf(
            "%u hour%s (%u)",
            uiHours,
            (uiHours == 1) ? "" : "s",
            uiBestAllowedFixStep);
        }

        pItem->setText(strItem);
        pItem->setTextAlignment(Qt::AlignLeft);
        pItem->setEditable(false);
        pItem->setData(QVariant(auiProposedLogSteps[i]));
        pCboLogStepItemModel->appendRow(pItem);

        if (auiProposedLogSteps[i] == uiConfiguredLogStep)
          iDefaultLogStepIdx = i;
      }
    }

    pCboLogStepSelector = new QMaemo5ListPickSelector;
    pCboLogStepSelector->setModel(pCboLogStepItemModel);
    pCboLogStepSelector->setCurrentIndex(iDefaultLogStepIdx);
    m_pCboLogStep = new QMaemo5ValueButton(tr("Log step"));
    m_pCboLogStep->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    m_pCboLogStep->setPickSelector(pCboLogStepSelector);

    m_pChkGpsAssisted = new QCheckBox(tr("Assisted GPS"));
    m_pChkGpsAssisted->setCheckState(settings.getGpsAssisted() ? Qt::Checked : Qt::Unchecked);

    m_pChkGpsAlwaysConnected = new QCheckBox(tr("GPS always connected"));
    m_pChkGpsAlwaysConnected->setCheckState(settings.getGpsAlwaysConnected() ? Qt::Checked : Qt::Unchecked);

    m_pChkAskTrackName = new QCheckBox(tr("Ask for track name before create"));
    m_pChkAskTrackName->setCheckState(settings.getAskTrackName() ? Qt::Checked : Qt::Unchecked);

    pVBox->addWidget(m_pCboLogStep);
    pVBox->addWidget(m_pChkGpsAssisted);
    pVBox->addWidget(m_pChkGpsAlwaysConnected);
    pVBox->addWidget(m_pChkAskTrackName);
    pLeftLayout->addLayout(pVBox);
  }

  // main layout setup
  {
    QScrollArea* pScrollArea = new QScrollArea;
    QWidget*     pScrollWidget = new QWidget();
    QPushButton* pBtnDone = new QPushButton(tr("Done"));

    this->connect(pBtnDone, SIGNAL(clicked()), SLOT(onClickedDone()));

    pScrollWidget->setLayout(pLeftLayout);

    pScrollArea->setWidgetResizable(true);
    pScrollArea->setWidget(pScrollWidget);
    pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pScrollArea->setProperty("FingerScrollable", true);

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
  QMaemo5ListPickSelector* pCboLogStepSelector = static_cast<QMaemo5ListPickSelector*>(m_pCboLogStep->pickSelector());

  settings.setLogStep(static_cast<QStandardItemModel*>(pCboLogStepSelector->model())->item(pCboLogStepSelector->currentIndex())->data().toUInt());
  settings.setGpsAssisted(m_pChkGpsAssisted->checkState() != Qt::Unchecked);
  settings.setGpsAlwaysConnected(m_pChkGpsAlwaysConnected->checkState() != Qt::Unchecked);
  settings.setAskTrackName(m_pChkAskTrackName->checkState() != Qt::Unchecked);

  settings.write();
  this->done(0);
}
