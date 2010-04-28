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
  AppSettings& settings = *App::instance()->settings();
  QHBoxLayout* pRootLayout = new QHBoxLayout;
  QVBoxLayout* pLeftLayout = new QVBoxLayout;

  // options
  {
    QFormLayout* pForm = new QFormLayout;
    uint uiConfiguredLogStep = settings.getLogStep();

    m_pCboLogStep = new QComboBox;
    for (uint uiIdx=0, ui=AppSettings::logStepBounds().first; ui <= AppSettings::logStepBounds().second; ++ui, ++uiIdx)
    {
      m_pCboLogStep->addItem(QString("%1 seconds (%2)").arg(ui).arg(Location::selectBestAllowedFixStep(ui)), QVariant(ui));
      if (ui == uiConfiguredLogStep)
        m_pCboLogStep->setCurrentIndex((int)uiIdx);
    }

    m_pChkGpsAlwaysConnected = new QCheckBox;
    m_pChkGpsAlwaysConnected->setCheckState(settings.getGpsAlwaysConnected() ? Qt::Checked : Qt::Unchecked);

    pForm->addRow(tr("Log Step :"), m_pCboLogStep);
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
  settings.setGpsAlwaysConnected(m_pChkGpsAlwaysConnected->checkState() != Qt::Unchecked);

  settings.write();
  this->done(0);
}
