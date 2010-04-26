//***************************************************************************
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
  this->setWindowTitle(tr("Configuration"));

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
  QHBoxLayout* pRootLayout = new QHBoxLayout;
  QVBoxLayout* pLeftLayout = new QVBoxLayout;

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
  // exit dialog
  this->done(0);
}
