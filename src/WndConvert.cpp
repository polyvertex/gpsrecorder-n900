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

  m_InputFiles.append(App::outputDir());
  this->setupControls();
  this->refreshInputFilesControl();
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
  QGridLayout* pRootLayout = new QGridLayout;
  QVBoxLayout* pLeftLayout = new QVBoxLayout;

  // browse input file(s)
  {
    QHBoxLayout* pHBox = new QHBoxLayout();
    QToolButton* pBtnBrowseFiles = new QToolButton();
    QToolButton* pBtnBrowseDir = new QToolButton();

    m_pTxtBrowse = new QLineEdit;
    m_pTxtBrowse->setReadOnly(true);

    pBtnBrowseFiles->setIcon(QIcon(":/icons/browse-file.png"));
    pBtnBrowseDir->setIcon(QIcon(":/icons/browse-folder.png"));
    this->connect(pBtnBrowseFiles, SIGNAL(clicked()), SLOT(onClickedBrowseFiles()));
    this->connect(pBtnBrowseDir, SIGNAL(clicked()), SLOT(onClickedBrowseDir()));

    pHBox->addWidget(m_pTxtBrowse);
    pHBox->addWidget(pBtnBrowseFiles);
    pHBox->addWidget(pBtnBrowseDir);
    pLeftLayout->addLayout(pHBox);
    pLeftLayout->addSpacing(10);
  }

  // output format - csv
  {
    m_pCboCsvSeparator = new MaemoComboBox(tr("Fields separator"), this);
    m_pCboCsvSeparator->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    for (int i = 0; i < (int)strlen(ExporterSinkCsv::supportedSeparators()); ++i)
    {
      QString strLabel(ExporterSinkCsv::supportedSeparators()[i]);
      if (ExporterSinkCsv::supportedSeparators()[i] == '\t')
        strLabel = "tab";
      m_pCboCsvSeparator->addItem(strLabel, (int)ExporterSinkCsv::supportedSeparators()[i]);
    }
    m_pCboCsvSeparator->setCurrentIndex(ExporterSinkCsv::separatorIndex(settings.getCsvSeparator()));

    m_pGroupBoxCsv = new MaemoGroupBox(tr("CSV options"));
    m_pGroupBoxCsv->addWidget(m_pCboCsvSeparator);
    m_pGroupBoxCsv->setEnabled(settings.getConvertCsv());

    pLeftLayout->addLayout(m_pGroupBoxCsv);
    pLeftLayout->addSpacing(10);
  }

  // output format - gpx
  //{
  //  QFormLayout* pForm = new QFormLayout;
  //
  //  m_pGroupBoxGpx = new MaemoGroupBox(tr("GPX options"));
  //  m_pGroupBoxGpx->setEnabled(settings.getConvertGpx());
  //  m_pGroupBoxGpx->addLayout(pForm);
  //
  //  pLeftLayout->addLayout(m_pGroupBoxGpx);
  //  pLeftLayout->addSpacing(10);
  //}

  // output format - kml
  {
    QFormLayout* pForm = new QFormLayout;

    m_KmlLineColor = settings.getKmlLineColor();
    m_pBtnKmlLineColor = new QPushButton(tr("Line color"));
    //m_pBtnKmlLineColor->setFlat(true);
    m_pBtnKmlLineColor->setAutoFillBackground(true);
    m_pBtnKmlLineColor->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(m_KmlLineColor.red()).arg(m_KmlLineColor.green()).arg(m_KmlLineColor.blue()));
    this->connect(m_pBtnKmlLineColor, SIGNAL(clicked()), SLOT(onClickedKmlLineColor()));

    m_pCboKmlLineWidth = new MaemoComboBox(tr("Line width"), this);
    m_pCboKmlLineWidth->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    for (int i = 1; i < 6; ++i)
      m_pCboKmlLineWidth->addItem(QString("%1").arg(i));
    m_pCboKmlLineWidth->setCurrentIndex(settings.getKmlLineWidth() - 1);

    m_pChkKmlAircraft = new QCheckBox(tr("Aircraft mode"));
    m_pChkKmlAircraft->setCheckState(settings.getKmlAircraftMode() ? Qt::Checked : Qt::Unchecked);

    m_pChkKmlZipped = new QCheckBox(tr("Zipped KML (KMZ)"));
    m_pChkKmlZipped->setCheckState(settings.getKmlZipped() ? Qt::Checked : Qt::Unchecked);

    m_pGroupBoxKml = new MaemoGroupBox(tr("KML options"));
    m_pGroupBoxKml->addWidget(m_pBtnKmlLineColor);
    m_pGroupBoxKml->addWidget(m_pCboKmlLineWidth);
    m_pGroupBoxKml->addWidget(m_pChkKmlAircraft);
    m_pGroupBoxKml->addWidget(m_pChkKmlZipped);
    m_pGroupBoxKml->setEnabled(settings.getConvertKml());

    pLeftLayout->addLayout(m_pGroupBoxKml);
    pLeftLayout->addSpacing(10);
  }

  // main layout setup
  {
    QScrollArea* pScrollArea = new QScrollArea;
    QWidget*     pScrollWidget = new QWidget();
    QPushButton* pBtnConvert = new QPushButton(tr("Convert"));

    m_pChkCsv = new QCheckBox("CSV");
    m_pChkCsv->setCheckState(settings.getConvertCsv() ? Qt::Checked : Qt::Unchecked);
    this->connect(m_pChkCsv, SIGNAL(stateChanged(int)), SLOT(onStateChangedCsv(int)));

    m_pChkGpx = new QCheckBox("GPX");
    m_pChkGpx->setCheckState(settings.getConvertGpx() ? Qt::Checked : Qt::Unchecked);
    this->connect(m_pChkGpx, SIGNAL(stateChanged(int)), SLOT(onStateChangedGpx(int)));

    m_pChkKml = new QCheckBox("KML");
    m_pChkKml->setCheckState(settings.getConvertKml() ? Qt::Checked : Qt::Unchecked);
    this->connect(m_pChkKml, SIGNAL(stateChanged(int)), SLOT(onStateChangedKml(int)));

    this->connect(pBtnConvert, SIGNAL(clicked()), SLOT(onClickedConvert()));

    pLeftLayout->setSpacing(0);
    pScrollWidget->setLayout(pLeftLayout);

    pScrollArea->setWidgetResizable(true);
    pScrollArea->setWidget(pScrollWidget);
    pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pScrollArea->setProperty("FingerScrollable", true);

    pRootLayout->setSpacing(1);
    pRootLayout->setColumnMinimumWidth(1, 150);
    pRootLayout->addWidget(pScrollArea, 0, 0, 5, 1);
    pRootLayout->addWidget(m_pChkCsv,   0, 1);
    pRootLayout->addWidget(m_pChkGpx,   1, 1);
    pRootLayout->addWidget(m_pChkKml,   2, 1);
    pRootLayout->addWidget(pBtnConvert, 4, 1);
  }

  // apply layout
  this->setLayout(pRootLayout);
}

//---------------------------------------------------------------------------
// refreshInputFilesControl
//---------------------------------------------------------------------------
void WndConvert::refreshInputFilesControl (void)
{
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
    QString str(tr("%1 files selected").arg(m_InputFiles.count()));
    m_pTxtBrowse->setText(str);
  }
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

  this->refreshInputFilesControl();
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

  m_InputFiles.clear();
  if (strDir.isEmpty())
  {
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
  QColor newColor;

  newColor = QColorDialog::getColor(m_KmlLineColor, this);
  if (newColor.isValid())
  {
    m_KmlLineColor = newColor;
    m_pBtnKmlLineColor->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(m_KmlLineColor.red()).arg(m_KmlLineColor.green()).arg(m_KmlLineColor.blue()));
  }
}

//---------------------------------------------------------------------------
// onStateChangedCsv
//---------------------------------------------------------------------------
void WndConvert::onStateChangedCsv (int nNewState)
{
  m_pGroupBoxCsv->setEnabled((nNewState == Qt::Unchecked) ? false : true);
}

//---------------------------------------------------------------------------
// onStateChangedGpx
//---------------------------------------------------------------------------
void WndConvert::onStateChangedGpx (int nNewState)
{
  Q_UNUSED(nNewState);
  //m_pGroupBoxGpx->setEnabled((nNewState == Qt::Unchecked) ? false : true);
}

//---------------------------------------------------------------------------
// onStateChangedKml
//---------------------------------------------------------------------------
void WndConvert::onStateChangedKml (int nNewState)
{
  m_pGroupBoxKml->setEnabled((nNewState == Qt::Unchecked) ? false : true);
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
      QMessageBox::warning(this, "", tr("No file selected or directory is empty !"));
      return;
    }

    if (m_pChkCsv->checkState() == Qt::Unchecked &&
        m_pChkGpx->checkState() == Qt::Unchecked &&
        m_pChkKml->checkState() == Qt::Unchecked)
    {
      QMessageBox::warning(this, "", tr("No output format selected !"));
      return;
    }
  }

  // store settings
  {
    AppSettings& settings = *App::instance()->settings();

    settings.setConvertCsv((m_pChkCsv->checkState() == Qt::Unchecked) ? false : true);
    settings.setConvertGpx((m_pChkGpx->checkState() == Qt::Unchecked) ? false : true);
    settings.setConvertKml((m_pChkKml->checkState() == Qt::Unchecked) ? false : true);

    if (m_pChkCsv->checkState() != Qt::Unchecked)
    {
      settings.setCsvSeparator((char)m_pCboCsvSeparator->currentItemData().toInt());
    }

    //if (m_pChkGpx->checkState() != Qt::Unchecked)
    //{
    //  // nothing to do for now...
    //}

    if (m_pChkKml->checkState() != Qt::Unchecked)
    {
      settings.setKmlZipped(m_pChkKmlZipped->checkState() != Qt::Unchecked);
      settings.setKmlLineColor(m_KmlLineColor);
      settings.setKmlLineWidth(uint(m_pCboKmlLineWidth->currentIndex() + 1));
      settings.setKmlAircraftMode(m_pChkKmlAircraft->checkState() != Qt::Unchecked);
    }

    settings.write();
  }

  // create needed export sinks
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

  // inform user
  {
    QString strMsg;

    if (!uiSuccessCount)
      strMsg = tr("No file was exported !");
    else if (uiSuccessCount == 1)
      strMsg = tr("1 file exported successfully.");
    else if (uiSuccessCount > 1)
      strMsg = tr("%1 files exported successfully.").arg(uiSuccessCount);

    QMessageBox::information(this, "", strMsg);
  }

  // exit dialog
  this->done(0);
}
