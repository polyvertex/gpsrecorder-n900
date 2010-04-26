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

  // apply settings to the widgets
  {
    QSettings& settings = *App::instance()->settings();
    QVariant var;

    var = settings.value(App::SETTINGNAME_CONVERT_CSV);
    m_pChkCsv->setCheckState(!var.canConvert(QVariant::Bool) ? Qt::Checked : (var.toBool() ? Qt::Checked : Qt::Unchecked));

    var = settings.value(App::SETTINGNAME_CONVERT_GPX);
    m_pChkGpx->setCheckState(!var.canConvert(QVariant::Bool) ? Qt::Checked : (var.toBool() ? Qt::Checked : Qt::Unchecked));

    var = settings.value(App::SETTINGNAME_CONVERT_KML);
    m_pChkKml->setCheckState(!var.canConvert(QVariant::Bool) ? Qt::Checked : (var.toBool() ? Qt::Checked : Qt::Unchecked));
  }

  // apply layout
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

  // check convert action
  {
    if (m_InputFiles.isEmpty())
    {
      // TODO : popup a message "No file selected !"
      return;
    }

    if (m_pChkCsv->checkState() == Qt::Unchecked &&
        m_pChkGpx->checkState() == Qt::Unchecked &&
        m_pChkKml->checkState() == Qt::Unchecked)
    {
      // TODO : popup a message "No output format selected !"
      return;
    }
  }

  // keep convert options
  {
    QSettings& settings = *App::instance()->settings();

    settings.setValue(App::SETTINGNAME_CONVERT_CSV, QVariant(m_pChkCsv->checkState() != Qt::Unchecked));
    settings.setValue(App::SETTINGNAME_CONVERT_GPX, QVariant(m_pChkGpx->checkState() != Qt::Unchecked));
    settings.setValue(App::SETTINGNAME_CONVERT_KML, QVariant(m_pChkKml->checkState() != Qt::Unchecked));
  }

  // create export sinks
  if (m_pChkCsv->checkState() != Qt::Unchecked)
    pSinkCsv = new ExporterSinkCsv(&exporter);
  if (m_pChkGpx->checkState() != Qt::Unchecked)
    pSinkGpx = new ExporterSinkGpx(&exporter);
  if (m_pChkKml->checkState() != Qt::Unchecked)
    pSinkKml = new ExporterSinkKml(&exporter);

  // do export
  if (m_InputFiles.count() == 1 && Util::fileIsDir(qPrintable(m_InputFiles[0])))
    uiSuccessCount = exporter.exportDir(m_InputFiles[0]);
  else
    uiSuccessCount = exporter.exportFiles(m_InputFiles);

  // free export sinks
  delete pSinkCsv;
  delete pSinkGpx;
  delete pSinkKml;

  // TODO : popup a message to show the uiSuccessCount value

  // exit dialog
  this->done(0);
}
