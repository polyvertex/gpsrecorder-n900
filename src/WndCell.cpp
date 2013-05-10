//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex+gpsrecorder [AT] gmail [DOT] com>
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
: WndBase(pParent)
{
  this->setWindowTitle(App::applicationLabel() + QString(" - ") + tr("Cell Tower"));
  this->createWidgets();

  memset(&m_Gsm, 0, sizeof(m_Gsm));
  memset(&m_Wcdma, 0, sizeof(m_Wcdma));

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
// onAppStatePixChanged
//---------------------------------------------------------------------------
void WndCell::onAppStatePixChanged (QPixmap* pNewStatePixmap)
{
  m_pLblStatusIcon->setPixmap(*pNewStatePixmap);
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
