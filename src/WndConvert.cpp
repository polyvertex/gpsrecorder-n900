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
// Constants
//---------------------------------------------------------------------------
static const char SUPPORTED_CSV_SEPARATORS[] = ",;\t";



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
  AppSettings& settings = *App::instance()->settings();
  QHBoxLayout* pRootLayout = new QHBoxLayout;
  QVBoxLayout* pLeftLayout = new QVBoxLayout;

  // browse input file(s)
  {
    QHBoxLayout* pHBox = new QHBoxLayout;
    QPushButton* pBtnBrowseFiles = new QPushButton(tr("Files"));
    QPushButton* pBtnBrowseDir = new QPushButton(tr("Dir"));

    m_pTxtBrowse = new QLineEdit;
    m_pTxtBrowse->setReadOnly(true);

    this->connect(pBtnBrowseFiles, SIGNAL(clicked()), SLOT(onClickedBrowseFiles()));
    this->connect(pBtnBrowseDir, SIGNAL(clicked()), SLOT(onClickedBrowseDir()));

    pHBox->addWidget(m_pTxtBrowse);
    pHBox->addWidget(pBtnBrowseFiles);
    pHBox->addWidget(pBtnBrowseDir);
    pLeftLayout->addLayout(pHBox);
    pLeftLayout->addStretch(1);
  }

  // output format - csv
  {
    QFormLayout* pForm = new QFormLayout;

    m_pCboCsvSeparator = new QComboBox;
    for (int i = 0; i < (int)strlen(ExporterSinkCsv::supportedSeparators()); ++i)
    {
      QString strLabel(ExporterSinkCsv::supportedSeparators()[i]);
      if (ExporterSinkCsv::supportedSeparators()[i] == '\t')
        strLabel = "tab";
      m_pCboCsvSeparator->addItem(strLabel, (int)ExporterSinkCsv::supportedSeparators()[i]);
    }
    m_pCboCsvSeparator->setCurrentIndex(ExporterSinkCsv::separatorIndex(settings.getCsvSeparator()));

    pForm->addRow(tr("Separator :"), m_pCboCsvSeparator);

    m_pGroupBoxCsv = new QGroupBox("CSV");
    m_pGroupBoxCsv->setCheckable(true);
    m_pGroupBoxCsv->setChecked(settings.getConvertCsv());
    m_pGroupBoxCsv->setLayout(pForm);

    pLeftLayout->addWidget(m_pGroupBoxCsv);
    pLeftLayout->addStretch(1);
  }

  // output format - gpx
  {
    QFormLayout* pForm = new QFormLayout;

    m_pGroupBoxGpx = new QGroupBox("GPX");
    m_pGroupBoxGpx->setCheckable(true);
    m_pGroupBoxGpx->setChecked(settings.getConvertGpx());
    m_pGroupBoxGpx->setLayout(pForm);

    pLeftLayout->addWidget(m_pGroupBoxGpx);
    pLeftLayout->addStretch(1);
  }

  // output format - kml
  {
    QFormLayout* pForm = new QFormLayout;

    m_KmlLineColor = settings.getKmlLineColor();
    m_pBtnKmlLineColor = new QPushButton;
    m_pBtnKmlLineColor->setFlat(true);
    m_pBtnKmlLineColor->setAutoFillBackground(true);
    m_pBtnKmlLineColor->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(m_KmlLineColor.red()).arg(m_KmlLineColor.green()).arg(m_KmlLineColor.blue()));
    this->connect(m_pBtnKmlLineColor, SIGNAL(clicked()), SLOT(onClickedKmlLineColor()));

    m_pCboKmlLineWidth = new QComboBox;
    for (int i = 1; i < 6; ++i)
      m_pCboKmlLineWidth->addItem(QString("%1").arg(i));

    m_pChkKmlAircraft = new QCheckBox;
    m_pChkKmlAircraft->setCheckState(settings.getKmlAircraftMode() ? Qt::Checked : Qt::Unchecked);

    pForm->addRow(tr("Line Color :"), m_pBtnKmlLineColor);
    pForm->addRow(tr("Line Width :"), m_pCboKmlLineWidth);
    pForm->addRow(tr("Aircraft Mode :"), m_pChkKmlAircraft);

    m_pGroupBoxKml = new QGroupBox("KML");
    m_pGroupBoxKml->setCheckable(true);
    m_pGroupBoxKml->setChecked(settings.getConvertKml());
    m_pGroupBoxKml->setLayout(pForm);

    pLeftLayout->addWidget(m_pGroupBoxKml);
    pLeftLayout->addStretch(1);
  }

  // main layout setup
  {
    QScrollArea* pScrollArea = new QScrollArea;
    QWidget*     pScrollWidget = new QWidget();
    QPushButton* pBtnConvert = new QPushButton(tr("Convert"));

    this->connect(pBtnConvert, SIGNAL(clicked()), SLOT(onClickedConvert()));

    pScrollWidget->setLayout(pLeftLayout);

    pScrollArea->setWidgetResizable(true);
    pScrollArea->setWidget(pScrollWidget);
    pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pScrollArea->setProperty("FingerScrollable", true);

    pRootLayout->addWidget(pScrollArea);
    pRootLayout->addWidget(pBtnConvert, 0, Qt::AlignBottom);
  }

  // apply layout
  this->setLayout(pRootLayout);
}



//---------------------------------------------------------------------------
// onClickedBrowseFiles
//---------------------------------------------------------------------------
void WndConvert::onClickedBrowseFiles (void)
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
// onClickedBrowseDir
//---------------------------------------------------------------------------
void WndConvert::onClickedBrowseDir (void)
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
// onClickedKmlLineColor
//---------------------------------------------------------------------------
void WndConvert::onClickedKmlLineColor (void)
{
  // TODO
}

//---------------------------------------------------------------------------
// onClickedConvert
//---------------------------------------------------------------------------
void WndConvert::onClickedConvert (void)
{
  Exporter exporter;
  ExporterSinkCsv* pSinkCsv = 0;
  ExporterSinkGpx* pSinkGpx = 0;
  ExporterSinkKml* pSinkKml = 0;
  uint uiSuccessCount = 0;

  // check action
  {
    if (m_InputFiles.isEmpty())
    {
      // TODO : popup a message "No file selected !"
      return;
    }

    if (!m_pGroupBoxCsv->isChecked() &&
        !m_pGroupBoxGpx->isChecked() &&
        !m_pGroupBoxKml->isChecked())
    {
      // TODO : popup a message "No output format selected !"
      return;
    }
  }

  // store settings
  {
    AppSettings& settings = *App::instance()->settings();

    settings.setConvertCsv(m_pGroupBoxCsv->isChecked());
    settings.setConvertGpx(m_pGroupBoxGpx->isChecked());
    settings.setConvertKml(m_pGroupBoxKml->isChecked());

    if (m_pGroupBoxCsv->isChecked())
    {
      settings.setCsvSeparator((char)m_pCboCsvSeparator->itemData(m_pCboCsvSeparator->currentIndex()).toInt());
    }

    //if (m_pGroupBoxGpx->isChecked())
    //{
    //  // nothing to do for now...
    //}

    if (m_pGroupBoxKml->isChecked())
    {
      settings.setKmlLineColor(m_KmlLineColor);
      settings.setKmlLineWidth(uint(m_pCboKmlLineWidth->currentIndex() + 1));
      settings.setKmlAircraftMode(m_pChkKmlAircraft->checkState() != Qt::Unchecked);
    }

    settings.write();
  }

  // create needed export sinks
  if (m_pGroupBoxCsv->isChecked())
    pSinkCsv = new ExporterSinkCsv(&exporter);
  if (m_pGroupBoxGpx->isChecked())
    pSinkGpx = new ExporterSinkGpx(&exporter);
  if (m_pGroupBoxKml->isChecked())
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
