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
    SIGNAL(sigLocationFix(Location*, const LocationFixContainer&)),
    SLOT(onLocationFix(Location*, const LocationFixContainer&)) );
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
  Location* pLocation = App::instance()->location();

  if (pLocation->isStarted())
  {
    m_GPSRFile.close();

    pLocation->stop();
    //this->clearFixFields();

    m_pMenuStartStop->setText("Start");
  }
  else
  {
    QByteArray strPath;

    this->clearFixFields();
    pLocation->resetLastFix();
    pLocation->start();

    strPath  = App::outputDir().toAscii();
    strPath += "/";
    strPath += App::applicationName().toAscii();
    strPath += "-";
    strPath += Util::timeString(true);
    strPath += ".gpsr";
    if (m_GPSRFile.openWrite(strPath.constData(), true))
      qDebug("Opened GPSR file %s", strPath.constData());

    m_pMenuStartStop->setText("Stop");
  }
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
void WndMain::onLocationFix (Location* pLocation, const LocationFixContainer& fixCont)
{
  QString strFields;
  QString strTime;
  QString strGSM;
  QString strWCDMA;

  Q_UNUSED(pLocation);
  if (!App::instance()->location()->isStarted())
    return; // this can happen right after a stop()

  m_FixCont.setFix(fixCont);
  const LocationFix& fix = *m_FixCont.getFix();

  if (m_GPSRFile.isOpen() &&
      !m_GPSRFile.isReading() &&
      fix.uiFixMode >= FIXMODE_2D &&
      fix.hasFields(FIXFIELD_LATLONG) &&
      fix.uiHorizEP < 9000) // 90m
  {
    m_GPSRFile.writeLocationFix(time(0), m_FixCont);
  }

  if (fix.wFixFields != FIXFIELD_NONE)
  {
    if (fix.hasFields(FIXFIELD_TIME))
      strFields += "Time ";
    if (fix.hasFields(FIXFIELD_LATLONG))
      strFields += "LatLong ";
    if (fix.hasFields(FIXFIELD_ALT))
      strFields += "Alt ";
    if (fix.hasFields(FIXFIELD_TRACK))
      strFields += "Track ";
    if (fix.hasFields(FIXFIELD_SPEED))
      strFields += "Speed ";
    if (fix.hasFields(FIXFIELD_CLIMB))
      strFields += "Climb ";
  }

  if (fix.uiTime == 0)
    strTime = "0";
  else
    strTime.sprintf("%s (%u)", Util::timeString(false, fix.uiTime), fix.uiTime);

  if (fix.sGSM.bSetup)
    strGSM.sprintf("MCC:%u MNC:%u LAC:%u Cell:%u", (uint)fix.sGSM.uiMCC, (uint)fix.sGSM.uiMNC, (uint)fix.sGSM.uiLAC, (uint)fix.sGSM.uiCellId);

  if (fix.sWCDMA.bSetup)
    strWCDMA.sprintf("MCC:%u MNC:%u UCID:%u", (uint)fix.sWCDMA.uiMCC, (uint)fix.sWCDMA.uiMNC, (uint)fix.sWCDMA.uiUCID);


  m_pTxtFixFields->setText(strFields);
  m_pTxtFixMode->setText(fix.getModeStr());
  m_pTxtFixTime->setText(strTime);
  m_pTxtFixLat->setText(QString::number(fix.getLatDeg(), 'f', 6));
  m_pTxtFixLong->setText(QString::number(fix.getLongDeg(), 'f', 6));
  m_pTxtFixAlt->setText(QString::number(fix.iAlt));
  m_pTxtFixTrack->setText(QString::number(fix.getTrackDeg(), 'f', 1));
  m_pTxtFixSpeed->setText(QString::number(fix.getSpeedKmh(), 'f', 2));
  m_pTxtFixSatUse->setText(QString::number(fix.cSatUse) + "/" + QString::number(fix.cSatCount));
  m_pTxtFixGsm->setText(strGSM);
  m_pTxtFixWcdma->setText(strWCDMA);
}
