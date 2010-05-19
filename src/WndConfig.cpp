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
    QFormLayout* pForm = new QFormLayout;
    uint uiConfiguredLogStep   = settings.getLogStep();
    uint auiProposedLogSteps[] = { 1, 2, 3, 4, 5, 10, 20, 30, 60, 120 };
    int  iDefaultLogStepIdx    = 4; // 5 seconds

    m_pCboLogStep = new QComboBox;
    for (int i = 0; i < sizeof(auiProposedLogSteps)/sizeof(auiProposedLogSteps[0]); ++i)
    {
      if (auiProposedLogSteps[i] >= AppSettings::logStepBounds().first ||
          auiProposedLogSteps[i] <= AppSettings::logStepBounds().second)
      {
        QString strItem;

        strItem.sprintf(
          "%u second%s (%u)",
          auiProposedLogSteps[i],
          (auiProposedLogSteps[i] == 1) ? "" : "s",
          Location::selectBestAllowedFixStep(auiProposedLogSteps[i]));

        m_pCboLogStep->addItem(strItem, QVariant(auiProposedLogSteps[i]));
        if (auiProposedLogSteps[i] == uiConfiguredLogStep)
          m_pCboLogStep->setCurrentIndex(i);
      }
    }
    if (m_pCboLogStep->currentIndex() < 0)
      m_pCboLogStep->setCurrentIndex(iDefaultLogStepIdx);

    m_pChkGpsAssisted = new QCheckBox;
    m_pChkGpsAssisted->setCheckState(settings.getGpsAssisted() ? Qt::Checked : Qt::Unchecked);

    m_pChkGpsAlwaysConnected = new QCheckBox;
    m_pChkGpsAlwaysConnected->setCheckState(settings.getGpsAlwaysConnected() ? Qt::Checked : Qt::Unchecked);

    pForm->addRow(tr("Log Step :"), m_pCboLogStep);
    pForm->addRow(tr("Assisted GPS :"), m_pChkGpsAssisted);
    pForm->addRow(tr("GPS always connected :"), m_pChkGpsAlwaysConnected);
    pLeftLayout->addLayout(pForm);
  }

  // main layout setup
  {
    QScrollArea* pScrollArea = new QScrollArea;
    QWidget*     pScrollWidget = new QWidget();
    QPushButton* pBtnDone = new QPushButton(tr("Done"));

    this->connect(pBtnDone, SIGNAL(clicked()), SLOT(onPushedDone()));

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
// onPushedDone
//---------------------------------------------------------------------------
void WndConfig::onPushedDone (void)
{
  AppSettings& settings = *App::instance()->settings();

  settings.setLogStep(m_pCboLogStep->itemData(m_pCboLogStep->currentIndex()).toUInt());
  settings.setGpsAssisted(m_pChkGpsAssisted->checkState() != Qt::Unchecked);
  settings.setGpsAlwaysConnected(m_pChkGpsAlwaysConnected->checkState() != Qt::Unchecked);

  settings.write();
  this->done(0);
}
