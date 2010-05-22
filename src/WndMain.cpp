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
// Created On : 2010-03-25 14:53:26
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndMain
//---------------------------------------------------------------------------
WndMain::WndMain (QMainWindow* pParent/*=0*/)
: QMainWindow(pParent)
{
  Q_ASSERT(App::instance());
  Q_ASSERT(App::instance()->location());
  Q_ASSERT(App::instance()->wndSat());

  this->setWindowTitle(App::applicationLabel());
#if QT_VERSION > 0x040503
  this->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif

  {
    this->menuBar()->clear();
    m_pMenuStartStop = this->menuBar()->addAction(tr("Start"), this, SLOT(onPushedStartStop()));
    m_pMenuSnap      = this->menuBar()->addAction(tr("Snap"), this, SLOT(onPushedSnap()));
    m_pMenuConfig    = this->menuBar()->addAction(tr("Settings"), this, SLOT(onPushedConfig()));
    m_pMenuConvert   = this->menuBar()->addAction(tr("Convert"), this, SLOT(onPushedConvert()));
    m_pMenuAbout     = this->menuBar()->addAction(tr("About"), this, SLOT(onPushedAbout()));

    m_pMenuSnap->setEnabled(false);
  }

  this->createWidgets();
  this->clearFixFields();
  this->showHome();

  this->connect(
    App::instance()->location(),
    SIGNAL(sigLocationFix(Location*, const LocationFixContainer*, bool)),
    SLOT(onLocationFix(Location*, const LocationFixContainer*, bool)) );
}

//---------------------------------------------------------------------------
// ~WndMain
//---------------------------------------------------------------------------
WndMain::~WndMain (void)
{
}



//---------------------------------------------------------------------------
// closeEvent
//---------------------------------------------------------------------------
void WndMain::closeEvent (QCloseEvent* pEvent)
{
  if (App::instance() && App::instance()->getState() == App::STATE_STARTED)
  {
    QMessageBox::StandardButton eBtn = QMessageBox::question(
      this,
      tr("Quit ?"),
      tr("You are about to stop recording a track by closing this application ! Is it what you want ?"),
      QMessageBox::Yes | QMessageBox::Cancel,
      QMessageBox::Cancel);

    if (eBtn != QMessageBox::Yes && eBtn != QMessageBox::Ok)
    {
      pEvent->ignore();
      return;
    }
    else
    {
      this->onPushedStartStop();
    }
  }

  pEvent->accept();
}



//---------------------------------------------------------------------------
// createWidgets
//---------------------------------------------------------------------------
void WndMain::createWidgets (void)
{
  m_pLblStatus = new QLabel();
  m_pLblStatus->setDisabled(true);

  m_pLblFixFields = new QLabel();
  m_pLblFixFields->setDisabled(true);

  m_pLblFixMode = new QLabel();
  m_pLblFixMode->setDisabled(true);

  m_pLblFixTime = new QLabel();
  m_pLblFixTime->setDisabled(true);

  m_pLblFixSatUse = new QLabel();
  m_pLblFixSatUse->setDisabled(true);

  m_pLblFixLat = new QLabel();
  m_pLblFixLat->setDisabled(true);

  m_pLblFixLong = new QLabel();
  m_pLblFixLong->setDisabled(true);

  m_pLblFixAlt = new QLabel();
  m_pLblFixAlt->setDisabled(true);

  m_pLblFixTrack = new QLabel();
  m_pLblFixTrack->setDisabled(true);

  m_pLblFixSpeed = new QLabel();
  m_pLblFixSpeed->setDisabled(true);

  m_pLblFixGsm = new QLabel();
  m_pLblFixGsm->setDisabled(true);

  m_pLblFixWcdma = new QLabel();
  m_pLblFixWcdma->setDisabled(true);
}

//---------------------------------------------------------------------------
// showHome
//---------------------------------------------------------------------------
void WndMain::showHome (void)
{
  // TODO : wallpaper + app name + app version + "record" button
  this->showFix();
}

//---------------------------------------------------------------------------
// showFix
//---------------------------------------------------------------------------
void WndMain::showFix (void)
{
  QWidget*     pWidget  = new QWidget();
  QGridLayout* pGrid    = new QGridLayout();
  QFormLayout* pForm1   = new QFormLayout();
  QFormLayout* pForm2   = new QFormLayout();
  QFormLayout* pForm3   = new QFormLayout();
  QWidget*     pBlank   = new QWidget();
  QHBoxLayout* pButtons = new QHBoxLayout();

  pForm1->addRow(tr("Status :"), m_pLblStatus);
  pForm1->addRow(tr("Fields :"), m_pLblFixFields);
  pForm1->addRow(tr("Mode :"),   m_pLblFixMode);
  pForm1->addRow(tr("Time :"),   m_pLblFixTime);
  pForm1->addRow(tr("SatUse :"), m_pLblFixSatUse);

  pForm2->addRow(tr("Lat :"),    m_pLblFixLat);
  pForm2->addRow(tr("Long :"),   m_pLblFixLong);
  pForm2->addRow(tr("Alt :"),    m_pLblFixAlt);
  pForm2->addRow(tr("Track :"),  m_pLblFixTrack);
  pForm2->addRow(tr("Speed :"),  m_pLblFixSpeed);

  pForm3->addRow(tr("GSM :"),    m_pLblFixGsm);
  pForm3->addRow(tr("WCDMA :"),  m_pLblFixWcdma);

  {
    QPushButton* pBtnSat   = new QPushButton(tr("Satellites"));
    QPushButton* pBtnSpeed = new QPushButton(tr("Speed"));

    this->connect(pBtnSat, SIGNAL(clicked()), SLOT(onPushedSat()));
    this->connect(pBtnSpeed, SIGNAL(clicked()), SLOT(onPushedSpeed()));

    pButtons->addWidget(pBtnSat);
    pButtons->addWidget(pBtnSpeed);
  }

  pGrid->setHorizontalSpacing(5);
  pGrid->addLayout(pForm1,   0, 0);
  pGrid->addLayout(pForm2,   0, 1);
  pGrid->addLayout(pForm3,   1, 0, 1, 2, Qt::AlignVCenter);
  pGrid->addLayout(pButtons, 2, 0, 1, 2, Qt::AlignBottom);

  pWidget->setLayout(pGrid);
  this->setCentralWidget(pWidget);
}



//---------------------------------------------------------------------------
// onPushedStartStop
//---------------------------------------------------------------------------
void WndMain::onPushedStartStop (void)
{
  App* pApp = App::instance();

  if (pApp->getState() == App::STATE_STARTED)
  {
    //this->clearFixFields();

    pApp->setState(App::STATE_STOPPED);
    m_pLblStatus->setText(tr("Stopped"));

    m_pMenuStartStop->setText(tr("Start"));
    m_pMenuSnap->setEnabled(false);
    m_pMenuConvert->setEnabled(true);
  }
  else
  {
    this->clearFixFields();

    pApp->setState(App::STATE_STARTED);
    m_pLblStatus->setText(tr("Started"));

    m_pMenuStartStop->setText(tr("Stop"));
    m_pMenuSnap->setEnabled(true);
    m_pMenuConvert->setEnabled(false);
  }
}

//---------------------------------------------------------------------------
// onPushedSnap
//---------------------------------------------------------------------------
void WndMain::onPushedSnap (void)
{
  GPSRFile* pGPSRFile = App::instance()->outFile();
  QString   strName;
  time_t    uiTime = time(0);

  Q_ASSERT(pGPSRFile);
  Q_ASSERT(pGPSRFile->isOpen());
  Q_ASSERT(pGPSRFile->isWriting());
  if (!pGPSRFile || !pGPSRFile->isOpen() || !pGPSRFile->isWriting())
    return;

  // ask for point name
  strName = QInputDialog::getText(
    this,
    tr("Point name ?"),
    tr("Please enter the name of the point to snap or leave blank :"),
    QLineEdit::Normal).trimmed();

  // snap point
  pGPSRFile->writeNamedSnap(uiTime, qPrintable(strName));

  // TODO : popup a message to inform user (no confirmation needed !)
}

//---------------------------------------------------------------------------
// onPushedConfig
//---------------------------------------------------------------------------
void WndMain::onPushedConfig (void)
{
  WndConfig wndConfig(this);
  wndConfig.exec();
}

//---------------------------------------------------------------------------
// onPushedConvert
//---------------------------------------------------------------------------
void WndMain::onPushedConvert (void)
{
  WndConvert wndConvert(this);
  wndConvert.exec();

  if (!App::instance()->location()->isStarted())
    this->clearFixFields();
}

//---------------------------------------------------------------------------
// onPushedSat
//---------------------------------------------------------------------------
void WndMain::onPushedSat (void)
{
  App::instance()->wndSat()->show();
}

//---------------------------------------------------------------------------
// onPushedSpeed
//---------------------------------------------------------------------------
void WndMain::onPushedSpeed (void)
{
  App::instance()->wndSpeed()->show();
}

//---------------------------------------------------------------------------
// onPushedAbout
//---------------------------------------------------------------------------
void WndMain::onPushedAbout (void)
{
  App::instance()->wndAbout()->show();
}



//---------------------------------------------------------------------------
// clearFixFields
//---------------------------------------------------------------------------
void WndMain::clearFixFields (void)
{
  m_pLblFixFields->clear();
  m_pLblFixMode->clear();
  m_pLblFixTime->clear();
  m_pLblFixLat->clear();
  m_pLblFixLong->clear();
  m_pLblFixAlt->clear();
  m_pLblFixTrack->clear();
  m_pLblFixSpeed->clear();
  m_pLblFixSatUse->clear();
  m_pLblFixGsm->clear();
  m_pLblFixWcdma->clear();

  memset(&m_CellInfoGsm, 0, sizeof(m_CellInfoGsm));
  memset(&m_CellInfoWcdma, 0, sizeof(m_CellInfoWcdma));
  m_uiCellInfoGsmTime   = 0;
  m_uiCellInfoWcdmaTime = 0;
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void WndMain::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  const LocationFix& fix = *pFixCont->getFix();
  QString str;

  this->setUpdatesEnabled(false);

  // status
  str.sprintf("%s, %s",
    App::instance()->getStateStr(),
    (bAccurate ? QT_TR_NOOP("Fixed") : pLocation->isAcquiring() ? QT_TR_NOOP("Acquiring") : QT_TR_NOOP("Lost")) );
  m_pLblStatus->setText(str);

  // fix fields
  str.clear();
  if (fix.wFixFields != FIXFIELD_NONE)
  {
    if (fix.hasFields(FIXFIELD_TIME))
      str += tr("Time ");
    if (fix.hasFields(FIXFIELD_LATLONG))
      str += tr("LatLong ");
    if (fix.hasFields(FIXFIELD_ALT))
      str += tr("Alt ");
    if (fix.hasFields(FIXFIELD_TRACK))
      str += tr("Track ");
    if (fix.hasFields(FIXFIELD_SPEED))
      str += tr("Speed ");
    if (fix.hasFields(FIXFIELD_CLIMB))
      str += tr("Climb ");
  }
  m_pLblFixFields->setText(str);

  // fix mode
  m_pLblFixMode->setText(fix.getModeStr());

  // fix time
  str.clear();
  if (fix.uiTime > 0)
    str = Util::timeString(false, fix.uiTime);
  m_pLblFixTime->setText(str);

  // fix properties
  m_pLblFixSatUse->setText(QString::number(fix.cSatUse) + " / " + QString::number(fix.cSatCount));
  m_pLblFixLat->setText(QString::number(fix.getLatDeg(), 'f', 6) + QChar(L'\x00b0'));
  m_pLblFixLong->setText(QString::number(fix.getLongDeg(), 'f', 6) + QChar(L'\x00b0'));
  m_pLblFixAlt->setText(QString::number(fix.iAlt) + " m");
  m_pLblFixTrack->setText(QString::number(fix.getTrackDeg(), 'f', 1));
  m_pLblFixSpeed->setText(QString::number(fix.getSpeedKmh(), 'f', 2));

  // gsm cell info
  str.clear();
  if (fix.sGSM.bSetup)
  {
    memcpy(&m_CellInfoGsm, &fix.sGSM, sizeof(m_CellInfoGsm));
    m_uiCellInfoGsmTime = fix.uiTime;

    str.sprintf("MCC:%u MNC:%u LAC:%u Cell:%u", (uint)fix.sGSM.uiMCC, (uint)fix.sGSM.uiMNC, (uint)fix.sGSM.uiLAC, (uint)fix.sGSM.uiCellId);
  }
  m_pLblFixGsm->setText(str);

  // wcdma cell info
  str.clear();
  if (fix.sWCDMA.bSetup)
  {
    memcpy(&m_CellInfoWcdma, &fix.sWCDMA, sizeof(m_CellInfoWcdma));
    m_uiCellInfoWcdmaTime = fix.uiTime;

    str.sprintf("MCC:%u MNC:%u UCID:%u", (uint)fix.sWCDMA.uiMCC, (uint)fix.sWCDMA.uiMNC, (uint)fix.sWCDMA.uiUCID);
  }
  m_pLblFixWcdma->setText(str);


  this->setUpdatesEnabled(true);
}
