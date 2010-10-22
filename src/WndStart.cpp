//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
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
// Created On : 2010-10-20 18:33:21
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndStart
//---------------------------------------------------------------------------
WndStart::WndStart (QWidget* pParent/*=0*/)
: QDialog(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(pParent);

  m_bCanceled = true;

  this->setModal(true);
}

//---------------------------------------------------------------------------
// ~WndStart
//---------------------------------------------------------------------------
WndStart::~WndStart (void)
{
}



//---------------------------------------------------------------------------
// doExec
//---------------------------------------------------------------------------
bool WndStart::doExec (void)
{
  QMessageBox::StandardButton eBtn = QMessageBox::question(
    static_cast<QWidget*>(this->parent()),
    tr("Append to a file ?"),
    tr("Do you want to append your track to an existing GPSR file instead of creating a new file ?"),
    QMessageBox::Yes | QMessageBox::No,
    QMessageBox::No);

  m_eStartMode =
    (eBtn == QMessageBox::Yes) ?
    STARTMODE_APPENDTRACK :
    STARTMODE_NEWTRACK;

  this->setupControls();
  this->exec();

  return m_bCanceled ? false : true;
}



//---------------------------------------------------------------------------
// setupControls
//---------------------------------------------------------------------------
void WndStart::setupControls (void)
{
  QPushButton* pBtnStart;

  m_pTxtTrackName = new QLineEdit();
  m_pTxtTrackName->setValidator(new QTrackNameValidator);

  m_pCboMeansOfTransport = new MaemoComboBox(tr("Means of transportation"), this);
  this->connect(m_pCboMeansOfTransport, SIGNAL(sigSelected(int)), SLOT(onSelectedMeansOfTransport(int)));
  m_pCboMeansOfTransport->setValueLayout(QMaemo5ValueButton::ValueUnderText);
  m_pCboMeansOfTransport->addItem(tr("N/A"),       (int)0);
  m_pCboMeansOfTransport->addItem(tr("Foot"),      (int)GPSRFile::MEANSTRANSPORT_FOOT);
  m_pCboMeansOfTransport->addItem(tr("Roller"),    (int)GPSRFile::MEANSTRANSPORT_ROLLER);
  m_pCboMeansOfTransport->addItem(tr("Bike"),      (int)GPSRFile::MEANSTRANSPORT_BIKE);
  m_pCboMeansOfTransport->addItem(tr("Motorbike"), (int)GPSRFile::MEANSTRANSPORT_MOTORBIKE);
  m_pCboMeansOfTransport->addItem(tr("Car"),       (int)GPSRFile::MEANSTRANSPORT_CAR);
  m_pCboMeansOfTransport->addItem(tr("Boat"),      (int)GPSRFile::MEANSTRANSPORT_BOAT);
  m_pCboMeansOfTransport->addItem(tr("Plane"),     (int)GPSRFile::MEANSTRANSPORT_PLANE);
  m_pCboMeansOfTransport->addItem(tr("Other ?"),   (int)GPSRFile::MEANSTRANSPORT_OTHER);
  m_pCboMeansOfTransport->setCurrentIndex(0);

  pBtnStart = new QPushButton(tr("Start"));
  this->connect(pBtnStart, SIGNAL(clicked()), SLOT(onClickedStart()));


  if (m_eStartMode == STARTMODE_NEWTRACK)
  {
    QGridLayout* pGrid;
    QLabel*      pLabel;

    pLabel = new QLabel(tr(
      "Please enter the name of the track to be created or leave blank :"));
    pLabel->setWordWrap(true);

    pGrid = new QGridLayout;
    pGrid->addWidget(pLabel,                 0, 0, 1, 6);
    pGrid->addWidget(m_pTxtTrackName,        1, 0, 1, 6);
    pGrid->addWidget(new QWidget(),          2, 0, 1, 6);
    pGrid->addWidget(m_pCboMeansOfTransport, 3, 0, 1, 4);
    pGrid->addWidget(pBtnStart,              3, 5);

    this->setWindowTitle(tr("New track file"));
    this->setLayout(pGrid);
  }
  else if (m_eStartMode == STARTMODE_APPENDTRACK)
  {
    QGridLayout* pGrid;
    QLabel*      pLabel;
    QToolButton* pBtnBrowseFile;

    pLabel = new QLabel(tr(
      "Please enter the name of the track to be appended or leave blank, "
      "then choose an existing file to append your new track :"));
    pLabel->setWordWrap(true);

    m_pTxtFilePath = new QLineEdit();
    m_pTxtFilePath->setReadOnly(true);

    pBtnBrowseFile = new QToolButton();
    pBtnBrowseFile->setIcon(QIcon(":/icons/browse-file.png"));
    this->connect(pBtnBrowseFile, SIGNAL(clicked()), SLOT(onClickedBrowseFile()));

    pGrid = new QGridLayout;
    pGrid->addWidget(pLabel,                 0, 0, 1, 6);
    pGrid->addWidget(m_pTxtTrackName,        1, 0, 1, 6);
    pGrid->addWidget(m_pTxtFilePath,         2, 0, 1, 5);
    pGrid->addWidget(pBtnBrowseFile,         2, 5);
    pGrid->addWidget(new QWidget(),          3, 0, 1, 6);
    pGrid->addWidget(m_pCboMeansOfTransport, 4, 0, 1, 4);
    pGrid->addWidget(pBtnStart,              4, 5);

    this->setWindowTitle(tr("Append track"));
    this->setLayout(pGrid);
  }

  m_pTxtTrackName->setFocus(Qt::PopupFocusReason);
}



//---------------------------------------------------------------------------
// onClickedBrowseFile
//---------------------------------------------------------------------------
void WndStart::onClickedBrowseFile (void)
{
  QString strFilePath = QFileDialog::getOpenFileName(
    this,
    tr("Select a file to append new track"),
    App::outputDir(),
    "GPSR Files (*.gpsr)");

  m_pTxtFilePath->setText(strFilePath);
}

//---------------------------------------------------------------------------
// onSelectedMeansOfTransport
//---------------------------------------------------------------------------
void WndStart::onSelectedMeansOfTransport (int iIndex)
{
  static int iLastIndex = -1;

  if (m_pCboMeansOfTransport->itemData(iIndex).toInt() == (int)GPSRFile::MEANSTRANSPORT_OTHER)
  {
    // here, user wants to specify its own means of transportation

    QString strInput;
    bool bOk = false;

    strInput = QInputDialog::getText(
      this,
      tr("Name ?"),
      tr("Please enter your means of transportation or leave blank :"),
      QLineEdit::Normal,
      m_strOtherMeansOfTransport,
      &bOk);

    if (bOk)
    {
      strInput.replace(QRegExp("[^A-Za-z0-9_]"), " ");
      strInput = strInput.simplified();
      strInput = strInput.trimmed();
      strInput.replace(" ", "_");

      m_strOtherMeansOfTransport = strInput;

      m_pCboMeansOfTransport->setItemText(
        iIndex,
        tr("Other : ") +
        QString(" \"") +
        m_strOtherMeansOfTransport +
        QString("\""));
    }
    else if (iLastIndex >= 0)
    {
      m_pCboMeansOfTransport->setCurrentIndex(iLastIndex);
    }
  }

  iLastIndex = iIndex;
}

//---------------------------------------------------------------------------
// onClickedStart
//---------------------------------------------------------------------------
void WndStart::onClickedStart (void)
{
  m_bCanceled = false;

  m_strTrackName = m_pTxtTrackName->text();

  if (m_eStartMode == STARTMODE_NEWTRACK)
  {
    m_strFilePath  = App::outputDir();
    m_strFilePath += "/gpstrack-";
    m_strFilePath += Util::timeStringForFileName();

    if (!m_strTrackName.isEmpty())
    {
      m_strFilePath += '-';
      m_strFilePath += m_strTrackName;
    }
    m_strFilePath += ".gpsr";
  }
  else if (m_eStartMode == STARTMODE_APPENDTRACK)
  {
    m_strFilePath = m_pTxtFilePath->text();
  }

  m_ucMeansOfTransport = (quint8)m_pCboMeansOfTransport.currentItemData().toInt();
  if (m_ucMeansOfTransport != GPSRFile::MEANSTRANSPORT_OTHER)
    m_strOtherMeansOfTransport.clear();

  this->done(0);
}
