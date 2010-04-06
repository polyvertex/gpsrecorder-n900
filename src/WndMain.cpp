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

  this->menuBar()->clear();
  m_pMenuStartStop = this->menuBar()->addAction("Start", this, SLOT(onPushedStartStop()));
  m_pMenuExport    = this->menuBar()->addAction("Export", this, SLOT(onPushedExport()));

  m_pTxtStatus = new QLineEdit();
  m_pTxtStatus->setReadOnly(true);
  m_pTxtStatus->setDisabled(true);

  m_pTxtFixFields = new QLineEdit();
  m_pTxtFixFields->setReadOnly(true);
  m_pTxtFixFields->setDisabled(true);

  m_pTxtFixMode = new QLineEdit();
  m_pTxtFixMode->setReadOnly(true);
  m_pTxtFixMode->setDisabled(true);

  m_pTxtFixTime = new QLineEdit();
  m_pTxtFixTime->setReadOnly(true);
  m_pTxtFixTime->setDisabled(true);

  m_pTxtFixLat = new QLineEdit();
  m_pTxtFixLat->setReadOnly(true);
  m_pTxtFixLat->setDisabled(true);

  m_pTxtFixLong = new QLineEdit();
  m_pTxtFixLong->setReadOnly(true);
  m_pTxtFixLong->setDisabled(true);

  m_pTxtFixAlt = new QLineEdit();
  m_pTxtFixAlt->setReadOnly(true);
  m_pTxtFixAlt->setDisabled(true);

  m_pTxtFixTrack = new QLineEdit();
  m_pTxtFixTrack->setReadOnly(true);
  m_pTxtFixTrack->setDisabled(true);

  m_pTxtFixSpeed = new QLineEdit();
  m_pTxtFixSpeed->setReadOnly(true);
  m_pTxtFixSpeed->setDisabled(true);

  m_pTxtFixSatUse = new QLineEdit();
  m_pTxtFixSatUse->setReadOnly(true);
  m_pTxtFixSatUse->setDisabled(true);

  m_pTxtFixGsm = new QLineEdit();
  m_pTxtFixGsm->setReadOnly(true);
  m_pTxtFixGsm->setDisabled(true);

  m_pTxtFixWcdma = new QLineEdit();
  m_pTxtFixWcdma->setReadOnly(true);
  m_pTxtFixWcdma->setDisabled(true);

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

  pForm->addRow("Status :", m_pTxtStatus);
  pForm->addRow("Fields :", m_pTxtFixFields);
  pForm->addRow("Mode :",   m_pTxtFixMode);
  pForm->addRow("Time :",   m_pTxtFixTime);
  pForm->addRow("Lat :",    m_pTxtFixLat);
  pForm->addRow("Long :",   m_pTxtFixLong);
  pForm->addRow("Alt :",    m_pTxtFixAlt);
  pForm->addRow("Track :",  m_pTxtFixTrack);
  pForm->addRow("Speed :",  m_pTxtFixSpeed);
  pForm->addRow("SatUse :", m_pTxtFixSatUse);
  pForm->addRow("GSM :",    m_pTxtFixGsm);
  pForm->addRow("WCDMA :",  m_pTxtFixWcdma);

  pWidget->setLayout(pForm);
  this->setCentralWidget(pWidget);
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
    m_pTxtStatus->setText("Stopped");

    m_pMenuStartStop->setText("Start");
    m_pMenuExport->setEnabled(true);
  }
  else
  {
    pApp->setState(App::STATE_STARTED);
    m_pTxtStatus->setText("Started");

    this->clearFixFields();
    pLocation->resetLastFix();
    pLocation->start();

    m_pMenuStartStop->setText("Stop");
    m_pMenuExport->setEnabled(false);
  }
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
  m_pTxtFixFields->clear();
  m_pTxtFixMode->clear();
  m_pTxtFixTime->clear();
  m_pTxtFixLat->clear();
  m_pTxtFixLong->clear();
  m_pTxtFixAlt->clear();
  m_pTxtFixTrack->clear();
  m_pTxtFixSpeed->clear();
  m_pTxtFixSatUse->clear();
  m_pTxtFixGsm->clear();
  m_pTxtFixWcdma->clear();
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void WndMain::onLocationFix (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate)
{
  QString str;

  Q_UNUSED(pLocation);

  const LocationFix& fix = *pFixCont->getFix();

  str.sprintf("%s, %s",
    App::instance()->getStateStr(),
    (bAccurate ? "Fixed" : pLocation->isAcquiring() ? "Acquiring" : "Lost") );
  m_pTxtStatus->setText(str);

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
  m_pTxtFixFields->setText(str);

  m_pTxtFixMode->setText(fix.getModeStr());

  str.clear();
  if (fix.uiTime > 0)
    str.sprintf("%s (%u)", Util::timeString(false, fix.uiTime), fix.uiTime);
  m_pTxtFixTime->setText(str);

  m_pTxtFixLat->setText(QString::number(fix.getLatDeg(), 'f', 6));
  m_pTxtFixLong->setText(QString::number(fix.getLongDeg(), 'f', 6));
  m_pTxtFixAlt->setText(QString::number(fix.iAlt));
  m_pTxtFixTrack->setText(QString::number(fix.getTrackDeg(), 'f', 1));
  m_pTxtFixSpeed->setText(QString::number(fix.getSpeedKmh(), 'f', 2));
  m_pTxtFixSatUse->setText(QString::number(fix.cSatUse) + "/" + QString::number(fix.cSatCount));

  str.clear();
  if (fix.sGSM.bSetup)
    str.sprintf("MCC:%u MNC:%u LAC:%u Cell:%u", (uint)fix.sGSM.uiMCC, (uint)fix.sGSM.uiMNC, (uint)fix.sGSM.uiLAC, (uint)fix.sGSM.uiCellId);
  m_pTxtFixGsm->setText(str);

  str.clear();
  if (fix.sWCDMA.bSetup)
    str.sprintf("MCC:%u MNC:%u UCID:%u", (uint)fix.sWCDMA.uiMCC, (uint)fix.sWCDMA.uiMNC, (uint)fix.sWCDMA.uiUCID);
  m_pTxtFixWcdma->setText(str);
}
