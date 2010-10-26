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
// QTrackNameValidator
//---------------------------------------------------------------------------
class QTrackNameValidator : public QValidator
{
public :
  QTrackNameValidator (QObject* pParent=0)
  : QValidator(pParent)
  {
    this->setLocale(QLocale::c());
  }

  void fixup (QString& strInput) const
  {
    strInput.replace(QRegExp("[^A-Za-z0-9_]"), "_");
  }

  State validate (QString& strInput, int& iPos) const
  {
    return
      strInput.contains(QRegExp("[^A-Za-z0-9_]")) ?
      QValidator::Invalid :
      QValidator::Acceptable;
  }
};



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
// doExec
//---------------------------------------------------------------------------
bool WndStart::doExec (void)
{
  QMessageBox::StandardButton eBtn = QMessageBox::question(
    static_cast<QWidget*>(this->parent()),
    tr("Create a new file ?"),
    tr("Do you want to create a new file for your track ?\nIf you answer No, your track will be appended to an existing GPSR file of your choice."),
    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
    QMessageBox::Cancel);

  if (eBtn == QMessageBox::Cancel)
  {
    m_bCanceled = true;
  }
  else
  {
    m_eStartMode =
      (eBtn == QMessageBox::Yes) ?
      STARTMODE_NEWTRACK :
      STARTMODE_APPENDTRACK;

    this->setupControls();
    this->exec();
  }

  return m_bCanceled ? false : true;
}



//---------------------------------------------------------------------------
// setupControls
//---------------------------------------------------------------------------
void WndStart::setupControls (void)
{
  AppSettings& settings = *App::instance()->settings();
  QPushButton* pBtnStart;

  m_pTxtTrackName = new QLabeledLineEdit(tr("TrackName"), "");
  m_pTxtTrackName->setValidator(new QTrackNameValidator);

  m_pCboMeansOfTransport = new QMeansOfTransportation(this);

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

    m_pTxtFilePath = new QLabeledLineEdit(tr("Existing GPSR file path"), "");
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
// onClickedStart
//---------------------------------------------------------------------------
void WndStart::onClickedStart (void)
{
  AppSettings& settings = *App::instance()->settings();

  m_strTrackName = m_pTxtTrackName->content();

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
    m_strFilePath = m_pTxtFilePath->content();
    if (m_strFilePath.isEmpty())
    {
      QMaemo5InformationBox::information(
        this,
        tr("You must choose an existing GPSR file to append your track !"));
      return;
    }
  }

  m_ucMeansOfTransport       = m_pCboMeansOfTransport->meansOfTransport();
  m_strOtherMeansOfTransport = m_pCboMeansOfTransport->otherMeansOfTransport();

  m_bCanceled = false;
  this->done(0);
}
