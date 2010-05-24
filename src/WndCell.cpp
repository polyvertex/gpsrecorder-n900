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
// Created On : 2010-05-23 07:48:54
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndCell
//---------------------------------------------------------------------------
WndCell::WndCell (QMainWindow* pParent/*=0*/)
: QMainWindow(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(App::instance()->location());

  this->setWindowTitle(App::applicationLabel() + QString(" - ") + tr("Cell Tower"));
#if QT_VERSION > 0x040503
  this->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
  this->createWidgets();

  memset(&m_Gsm, 0, sizeof(m_Gsm));
  memset(&m_Wcdma, 0, sizeof(m_Wcdma));

  this->connect(
    App::instance()->location(),
    SIGNAL(sigLocationFix(Location*, const LocationFixContainer*, bool)),
    SLOT(onLocationFix(Location*, const LocationFixContainer*, bool)) );
}

//---------------------------------------------------------------------------
// ~WndCell
//---------------------------------------------------------------------------
WndCell::~WndCell (void)
{
}



//---------------------------------------------------------------------------
// createWidgets
//---------------------------------------------------------------------------
void WndCell::createWidgets (void)
{
  m_pLblCellModeIcon = new QLabel();
  m_pLblCellModeIcon->setFixedWidth(App::instance()->pixCellModeNone()->width() + 10);
  m_pLblCellModeIcon->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_pLblCellModeIcon->setPixmap(*App::instance()->pixCellModeNone());

  m_pLblCellMode = new QLabel();
  m_pLblCellMode->setEnabled(false);

  m_pLblMCC = new QLabel();
  m_pLblMCC->setEnabled(false);

  m_pLblMNC = new QLabel();
  m_pLblMNC->setEnabled(false);

  m_pLblLAC = new QLabel();
  m_pLblLAC->setEnabled(false);

  m_pLblCellId = new QLabel();
  m_pLblCellId->setEnabled(false);

  m_pLblUCID = new QLabel();
  m_pLblUCID->setEnabled(false);

  m_pLblStatusIcon = new QLabel();
  m_pLblStatusIcon->setFixedWidth(App::instance()->getStatePix()->width() + 20);
  m_pLblStatusIcon->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  m_pLblStatusIcon->setPixmap(*App::instance()->getStatePix());

  m_pLblCellModeUpdate = new QLabel();
  m_pLblCellModeUpdate->setAlignment(Qt::AlignRight | Qt::AlignBottom);

  // layout
  {
    QWidget*     pWidget  = new QWidget();
    QGridLayout* pGrid    = new QGridLayout();
    QLabel*      pLblNote = new QLabel();

    m_pForm = new QFormLayout();
    m_pForm->setHorizontalSpacing(20);
    m_pForm->addRow(tr("Cell Mode :"), m_pLblCellMode);
    m_pForm->addRow(tr("MCC :"),       m_pLblMCC);
    m_pForm->addRow(tr("MNC :"),       m_pLblMNC);
    m_pForm->addRow(tr("LAC :"),       m_pLblLAC);
    m_pForm->addRow(tr("Cell ID :"),   m_pLblCellId);
    m_pForm->addRow(tr("UC ID :"),     m_pLblUCID);

    pLblNote->setEnabled(false);
    pLblNote->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    pLblNote->setWordWrap(true);
    pLblNote->setText(tr("Please note that Cell Tower info is provided only when GPS data is not available !"));

    pGrid->setHorizontalSpacing(20);
    pGrid->addWidget(m_pLblCellModeIcon,   0, 0);
    pGrid->addLayout(m_pForm,              0, 1);
    pGrid->addWidget(pLblNote,             1, 1);
    pGrid->addWidget(m_pLblStatusIcon,     2, 0);
    pGrid->addWidget(m_pLblCellModeUpdate, 2, 1);

    pWidget->setLayout(pGrid);
    this->setCentralWidget(pWidget);
  }
}



//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void WndCell::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  Q_UNUSED(pLocation);
  Q_UNUSED(bAccurate);

  static int iLastMode = -1; // -1:unknown 0:gsm 1:wcdma

  const LocationFix& fix = *pFixCont->getFix();

  // status icon
  m_pLblStatusIcon->setPixmap(*App::instance()->getStatePix());

  // wcdma
  if (fix.sWCDMA.bSetup && ((iLastMode != 1) || (memcmp(&m_Wcdma, &fix.sWCDMA, sizeof(m_Wcdma)) != 0)))
  {
    iLastMode = 1;
    memcpy(&m_Wcdma, &fix.sWCDMA, sizeof(m_Wcdma));

    m_pLblCellMode->setText("WCDMA");
    m_pLblCellModeIcon->setPixmap(*App::instance()->pixCellMode3G());
    m_pLblCellModeUpdate->setText(tr("Updated at ") + QDateTime::currentDateTime().toString("hh:mm:ss"));
    m_pLblMCC->setText(QString::number((uint)fix.sWCDMA.uiMCC));
    m_pLblMNC->setText(QString::number((uint)fix.sWCDMA.uiMNC));
    m_pLblUCID->setText(QString::number((uint)fix.sWCDMA.uiUCID));
    m_pLblLAC->clear();
    m_pLblCellId->clear();

    m_pForm->itemAt(3, QFormLayout::LabelRole)->widget()->setVisible(false);
    m_pForm->itemAt(3, QFormLayout::FieldRole)->widget()->setVisible(false);
    m_pForm->itemAt(4, QFormLayout::LabelRole)->widget()->setVisible(false);
    m_pForm->itemAt(4, QFormLayout::FieldRole)->widget()->setVisible(false);
    m_pForm->itemAt(5, QFormLayout::LabelRole)->widget()->setVisible(true);
    m_pForm->itemAt(5, QFormLayout::FieldRole)->widget()->setVisible(true);
  }

  // gsm
  else if (fix.sGSM.bSetup && ((iLastMode != 0) || (memcmp(&m_Gsm, &fix.sGSM, sizeof(m_Gsm)) != 0)))
  {
    iLastMode = 0;
    memcpy(&m_Gsm, &fix.sGSM, sizeof(m_Gsm));

    m_pLblCellMode->setText("GSM");
    m_pLblCellModeIcon->setPixmap(*App::instance()->pixCellMode2G());
    m_pLblCellModeUpdate->setText(tr("Updated at ") + QDateTime::currentDateTime().toString("hh:mm:ss"));
    m_pLblMCC->setText(QString::number((uint)fix.sGSM.uiMCC));
    m_pLblMNC->setText(QString::number((uint)fix.sGSM.uiMNC));
    m_pLblLAC->setText(QString::number((uint)fix.sGSM.uiLAC));
    m_pLblCellId->setText(QString::number((uint)fix.sGSM.uiCellId));
    m_pLblUCID->clear();

    m_pForm->itemAt(3, QFormLayout::LabelRole)->widget()->setVisible(true);
    m_pForm->itemAt(3, QFormLayout::FieldRole)->widget()->setVisible(true);
    m_pForm->itemAt(4, QFormLayout::LabelRole)->widget()->setVisible(true);
    m_pForm->itemAt(4, QFormLayout::FieldRole)->widget()->setVisible(true);
    m_pForm->itemAt(5, QFormLayout::LabelRole)->widget()->setVisible(false);
    m_pForm->itemAt(5, QFormLayout::FieldRole)->widget()->setVisible(false);
  }
}
