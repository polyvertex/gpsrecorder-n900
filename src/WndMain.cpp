//***************************************************************************
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

  this->setWindowTitle(App::applicationLabel());

  {
    this->menuBar()->clear();
    m_pMenuStartStop = this->menuBar()->addAction("Start", this, SLOT(onPushedStartStop()));
    m_pMenuSnap      = this->menuBar()->addAction("Snap", this, SLOT(onPushedSnap()));
    m_pMenuExport    = this->menuBar()->addAction("Export", this, SLOT(onPushedExport()));

    m_pMenuSnap->setEnabled(false);
  }

  m_pLblStatus = new QLabel();
  m_pLblStatus->setDisabled(true);

  m_pLblFixFields = new QLabel();
  m_pLblFixFields->setDisabled(true);

  m_pLblFixMode = new QLabel();
  m_pLblFixMode->setDisabled(true);

  m_pLblFixTime = new QLabel();
  m_pLblFixTime->setDisabled(true);

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

  m_pLblFixSatUse = new QLabel();
  m_pLblFixSatUse->setDisabled(true);

  m_pLblFixGsm = new QLabel();
  m_pLblFixGsm->setDisabled(true);

  m_pLblFixWcdma = new QLabel();
  m_pLblFixWcdma->setDisabled(true);

  m_uiStartTime = 0;
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
  QWidget* pWidget = new QWidget();
  QFormLayout* pForm = new QFormLayout();

  pForm->addRow("Status :", m_pLblStatus);
  pForm->addRow("Fields :", m_pLblFixFields);
  pForm->addRow("Mode :",   m_pLblFixMode);
  pForm->addRow("Time :",   m_pLblFixTime);
  pForm->addRow("Lat :",    m_pLblFixLat);
  pForm->addRow("Long :",   m_pLblFixLong);
  pForm->addRow("Alt :",    m_pLblFixAlt);
  pForm->addRow("Track :",  m_pLblFixTrack);
  pForm->addRow("Speed :",  m_pLblFixSpeed);
  pForm->addRow("SatUse :", m_pLblFixSatUse);
  pForm->addRow("GSM :",    m_pLblFixGsm);
  pForm->addRow("WCDMA :",  m_pLblFixWcdma);

  pWidget->setLayout(pForm);
  this->setCentralWidget(pWidget);

  /*
  QWidget* pRoot = new QWidget();
  QGridLayout* pGrid = new QGridLayout;
  // ...
  pRoot->setLayout(pGrid);
  this->setCentralWidget(pRoot);
  */
}



//---------------------------------------------------------------------------
// onPushedStartStop
//---------------------------------------------------------------------------
void WndMain::onPushedStartStop (void)
{
  App* pApp = App::instance();
  Location* pLocation = pApp->location();

  if (pLocation->isStarted())
  {
    pLocation->stop();
    //this->clearFixFields();

    pApp->setState(App::STATE_STOPPED);
    m_pLblStatus->setText("Stopped");

    m_pMenuStartStop->setText("Start");
    m_pMenuSnap->setEnabled(false);
    m_pMenuExport->setEnabled(true);
  }
  else
  {
    pApp->setState(App::STATE_STARTED);
    m_pLblStatus->setText("Started");

    this->clearFixFields();
    m_uiStartTime = time(0);

    pLocation->resetLastFix();
    pLocation->start();

    m_pMenuStartStop->setText("Stop");
    m_pMenuSnap->setEnabled(true);
    m_pMenuExport->setEnabled(false);
  }
}

//---------------------------------------------------------------------------
// onPushedSnap
//---------------------------------------------------------------------------
void WndMain::onPushedSnap (void)
{
  GPSRFile* pGPSRFile = App::instance()->outFile();

  Q_ASSERT(pGPSRFile);
  Q_ASSERT(pGPSRFile->isOpen());
  Q_ASSERT(pGPSRFile->isWriting());
  if (!pGPSRFile || !pGPSRFile->isOpen() || !pGPSRFile->isWriting())
    return;

  pGPSRFile->writeSnap(time(0));

  // TODO : popup a message to inform user (no confirmation needed !)
}

//---------------------------------------------------------------------------
// onPushedExport
//---------------------------------------------------------------------------
void WndMain::onPushedExport (void)
{
  Exporter::exportStatic();
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
  QString str;

  Q_UNUSED(pLocation);

  const LocationFix& fix = *pFixCont->getFix();

  // status
  str.sprintf("%s, %s",
    App::instance()->getStateStr(),
    (bAccurate ? "Fixed" : pLocation->isAcquiring() ? "Acquiring" : "Lost") );
  m_pLblStatus->setText(str);

  // fix fields
  str.clear();
  if (fix.wFixFields != FIXFIELD_NONE)
  {
    if (fix.hasFields(FIXFIELD_TIME))
      str += "Time ";
    if (fix.hasFields(FIXFIELD_LATLONG))
      str += "LatLong ";
    if (fix.hasFields(FIXFIELD_ALT))
      str += "Alt ";
    if (fix.hasFields(FIXFIELD_TRACK))
      str += "Track ";
    if (fix.hasFields(FIXFIELD_SPEED))
      str += "Speed ";
    if (fix.hasFields(FIXFIELD_CLIMB))
      str += "Climb ";
  }
  m_pLblFixFields->setText(str);

  // fix mode
  m_pLblFixMode->setText(fix.getModeStr());

  // fix time
  str.clear();
  if (fix.uiTime > 0)
    str.sprintf("%s (%u)", Util::timeString(false, fix.uiTime), fix.uiTime);
  m_pLblFixTime->setText(str);

  // fix properties
  m_pLblFixLat->setText(QString::number(fix.getLatDeg(), 'f', 6));
  m_pLblFixLong->setText(QString::number(fix.getLongDeg(), 'f', 6));
  m_pLblFixAlt->setText(QString::number(fix.iAlt));
  m_pLblFixTrack->setText(QString::number(fix.getTrackDeg(), 'f', 1));
  m_pLblFixSpeed->setText(QString::number(fix.getSpeedKmh(), 'f', 2));
  m_pLblFixSatUse->setText(QString::number(fix.cSatUse) + "/" + QString::number(fix.cSatCount));

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
}
