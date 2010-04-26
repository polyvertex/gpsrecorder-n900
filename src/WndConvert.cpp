//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-20 15:39:23
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndConvert
//---------------------------------------------------------------------------
WndConvert::WndConvert (QWidget* pParent/*=0*/)
: QDialog(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(App::instance()->location());
  Q_ASSERT(pParent);

  this->setModal(true);
  this->setWindowTitle(tr("Convert"));

  this->setupControls();
}

//---------------------------------------------------------------------------
// ~WndConvert
//---------------------------------------------------------------------------
WndConvert::~WndConvert (void)
{
}



//---------------------------------------------------------------------------
// setupControls
//---------------------------------------------------------------------------
void WndConvert::setupControls (void)
{
  QHBoxLayout* pRootLayout = new QHBoxLayout;
  QVBoxLayout* pLeftLayout = new QVBoxLayout;

  // browse input file(s)
  {
    QHBoxLayout* pHBox = new QHBoxLayout;
    QPushButton* pBtnBrowseFiles = new QPushButton(tr("Files"));
    QPushButton* pBtnBrowseDir = new QPushButton(tr("Dir"));

    m_pTxtBrowse = new QLineEdit;
    m_pTxtBrowse->setReadOnly(true);

    this->connect(pBtnBrowseFiles, SIGNAL(clicked()), SLOT(onPushedBrowseFiles()));
    this->connect(pBtnBrowseDir, SIGNAL(clicked()), SLOT(onPushedBrowseDir()));

    pHBox->addWidget(m_pTxtBrowse);
    pHBox->addWidget(pBtnBrowseFiles);
    pHBox->addWidget(pBtnBrowseDir);
    pLeftLayout->addLayout(pHBox);
  }

  // output formats
  {
    QHBoxLayout* pHBox = new QHBoxLayout;

    m_pChkCsv = new QCheckBox("CSV");
    m_pChkGpx = new QCheckBox("GPX");
    m_pChkKml = new QCheckBox("KML");

    m_pChkCsv->setCheckState(Qt::Checked);
    m_pChkGpx->setCheckState(Qt::Checked);
    m_pChkKml->setCheckState(Qt::Checked);

    pHBox->addWidget(m_pChkCsv);
    pHBox->addWidget(m_pChkGpx);
    pHBox->addWidget(m_pChkKml);
    pLeftLayout->addLayout(pHBox);
  }

  // main layout setup
  {
    QScrollArea* pScrollArea = new QScrollArea;
    QWidget*     pScrollWidget = new QWidget();
    QPushButton* pBtnConvert = new QPushButton(tr("Convert"));

    this->connect(pBtnConvert, SIGNAL(clicked()), SLOT(onPushedConvert()));

    pScrollWidget->setLayout(pLeftLayout);

    pScrollArea->setWidgetResizable(true);
    pScrollArea->setWidget(pScrollWidget);
    pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pScrollArea->setProperty("FingerScrollable", true);

    pRootLayout->addWidget(pScrollArea);
    pRootLayout->addWidget(pBtnConvert, 0, Qt::AlignBottom);
  }

  this->setLayout(pRootLayout);
}



//---------------------------------------------------------------------------
// onPushedBrowseFiles
//---------------------------------------------------------------------------
void WndConvert::onPushedBrowseFiles (void)
{
  m_InputFiles = QFileDialog::getOpenFileNames(
    this,
    tr("Select one or more files to convert"),
    App::outputDir(),
    "GPSR Files (*.gpsr)");

  if (m_InputFiles.isEmpty())
  {
    m_pTxtBrowse->clear();
  }
  else if (m_InputFiles.count() == 1)
  {
    m_pTxtBrowse->setText(m_InputFiles[0]);
  }
  else if (m_InputFiles.count() > 1)
  {
    QString str;

    str.sprintf(QT_TR_NOOP("%d files selected"), m_InputFiles.count());
    m_pTxtBrowse->setText(str);
  }
}

//---------------------------------------------------------------------------
// onPushedBrowseDir
//---------------------------------------------------------------------------
void WndConvert::onPushedBrowseDir (void)
{
  QString strDir = QFileDialog::getExistingDirectory(
    this,
    tr("Select a directory to convert"),
    App::outputDir(),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (strDir.isEmpty())
  {
    m_InputFiles.clear();
    m_pTxtBrowse->clear();
  }
  else
  {
    m_InputFiles.append(strDir);
    m_pTxtBrowse->setText(strDir);
  }
}

//---------------------------------------------------------------------------
// onPushedConvert
//---------------------------------------------------------------------------
void WndConvert::onPushedConvert (void)
{
  Exporter exporter;
  ExporterSinkCsv* pSinkCsv = 0;
  ExporterSinkGpx* pSinkGpx = 0;
  ExporterSinkKml* pSinkKml = 0;
  uint uiSuccessCount = 0;

  if (m_InputFiles.isEmpty())
  {
    // TODO : popup a message "No file selected !"
    return;
  }

  if (m_pChkCsv->checkState() != Qt::Unchecked)
    pSinkCsv = new ExporterSinkCsv(&exporter);
  if (m_pChkGpx->checkState() != Qt::Unchecked)
    pSinkGpx = new ExporterSinkGpx(&exporter);
  if (m_pChkKml->checkState() != Qt::Unchecked)
    pSinkKml = new ExporterSinkKml(&exporter);

  if (m_InputFiles.count() == 1 && Util::fileIsDir(qPrintable(m_InputFiles[0])))
  {
    uiSuccessCount = exporter.exportDir(m_InputFiles[0]);
  }
  else
  {
    uiSuccessCount = exporter.exportFiles(m_InputFiles);
  }

  delete pSinkCsv;
  delete pSinkGpx;
  delete pSinkKml;

  // TODO : popup a message to show the uiSuccessCount value

  emit this->done(0);
}
