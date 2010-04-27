//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-27 13:06:48
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Static Members
//---------------------------------------------------------------------------
static const char* SETTINGNAME_LOGSTEP            = "LogStep";
//atic const char* SETTINGNAME_GPSALWAYSCONNECTED = "GpsAlwaysConnected";
//
static const char* SETTINGNAME_CONVERT_CSV = "ConvertCsv";
static const char* SETTINGNAME_CONVERT_GPX = "ConvertGpx";
static const char* SETTINGNAME_CONVERT_KML = "ConvertKml";
//
static const char* SETTINGNAME_CSV_SEPARATOR = "CsvSeparator";
//
static const char* SETTINGNAME_KML_LINECOLOR    = "KmlLineColor";
static const char* SETTINGNAME_KML_LINEWIDTH    = "KmlLineWidth";
static const char* SETTINGNAME_KML_AIRCRAFTMODE = "KmlAircraftMode";
//atic const char* SETTINGNAME_KML_COLORBYSPEED = "KmlColorBySpeed";



//---------------------------------------------------------------------------
// AppSettings
//---------------------------------------------------------------------------
AppSettings::AppSettings (void)
{
  // ensure the QSettings default constructor has loaded *our* settings
  Q_ASSERT(QCoreApplication::organizationName().isEmpty() == false);
  Q_ASSERT(QCoreApplication::applicationName().isEmpty() == false);
}

//---------------------------------------------------------------------------
// ~AppSettings
//---------------------------------------------------------------------------
AppSettings::~AppSettings (void)
{
  this->write();
}



//---------------------------------------------------------------------------
// write
//---------------------------------------------------------------------------
QSettings::Status AppSettings::write (void)
{
  QSettings::Status eStatus;

  m_Settings.sync();

  eStatus = m_Settings.status();
  if (eStatus != QSettings::NoError)
  {
    qWarning("Failed to write settings ! Code %d.", eStatus);
    return eStatus;
  }

  return eStatus;
}



//---------------------------------------------------------------------------
// LogStep
//---------------------------------------------------------------------------
void AppSettings::setLogStep (uint uiLogStepSeconds)
{
  m_Settings.setValue(SETTINGNAME_LOGSTEP, QVariant(uiLogStepSeconds));
}

uint AppSettings::getLogStep (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_LOGSTEP);
  if (var.canConvert(QVariant::UInt))
  {
    bool bOk = false;
    uint ui = var.toUInt(&bOk);
    QPair<uint,uint> bounds = AppSettings::logStepBounds();

    if (bOk && ui >= bounds.first && ui <= bounds.second)
      return ui;
    else
      this->setLogStep(AppSettings::defaultLogStep());
  }

  return AppSettings::defaultLogStep();
}

//---------------------------------------------------------------------------
// ConvertCsv
//---------------------------------------------------------------------------
void AppSettings::setConvertCsv (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_CONVERT_CSV, QVariant(bEnable));
}

bool AppSettings::getConvertCsv (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_CONVERT_CSV);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultConvertCsv();
}

//---------------------------------------------------------------------------
// ConvertGpx
//---------------------------------------------------------------------------
void AppSettings::setConvertGpx (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_CONVERT_GPX, QVariant(bEnable));
}

bool AppSettings::getConvertGpx (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_CONVERT_GPX);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultConvertGpx();
}

//---------------------------------------------------------------------------
// ConvertKml
//---------------------------------------------------------------------------
void AppSettings::setConvertKml (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_CONVERT_KML, QVariant(bEnable));
}

bool AppSettings::getConvertKml (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_CONVERT_KML);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultConvertKml();
}

//---------------------------------------------------------------------------
// CsvSeparator
//---------------------------------------------------------------------------
void AppSettings::setCsvSeparator (char cSep)
{
  m_Settings.setValue(SETTINGNAME_CSV_SEPARATOR, QVariant(QChar(cSep)));
}

char AppSettings::getCsvSeparator (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_KML_LINECOLOR);
  if (var.canConvert(QVariant::Char))
  {
    char c = var.toChar().toAscii();

    if (ExporterSinkCsv::isAllowedSeparator(c))
      return c;
    else
      this->setCsvSeparator(ExporterSinkCsv::defaultSeparator());
  }

  return ExporterSinkCsv::defaultSeparator();
}

//---------------------------------------------------------------------------
// KmlLineColor
//---------------------------------------------------------------------------
void AppSettings::setKmlLineColor (const QColor& lineColor)
{
  m_Settings.setValue(SETTINGNAME_KML_LINECOLOR, QVariant(lineColor));
}

QColor AppSettings::getKmlLineColor (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_KML_LINECOLOR);
  if (var.canConvert(QVariant::Color))
    return var.value<QColor>();

  return ExporterSinkKml::defaultLineColor();
}

//---------------------------------------------------------------------------
// KmlLineWidth
//---------------------------------------------------------------------------
void AppSettings::setKmlLineWidth (int nLineWidth)
{
  m_Settings.setValue(SETTINGNAME_KML_LINEWIDTH, QVariant(nLineWidth));
}

int AppSettings::getKmlLineWidth (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_KML_LINEWIDTH);
  if (var.canConvert(QVariant::Int))
  {
    int nVal = var.toInt();
    QPair<int,int> bounds = ExporterSinkKml::lineWidthBounds();

    if (nVal >= bounds.first && nVal <= bounds.second)
      return nVal;
    else
      this->setKmlLineWidth(ExporterSinkKml::defaultLineWidth());
  }

  return ExporterSinkKml::defaultLineWidth();
}

//---------------------------------------------------------------------------
// KmlAircraftMode
//---------------------------------------------------------------------------
void AppSettings::setKmlAircraftMode (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_KML_AIRCRAFTMODE, QVariant(bEnable));
}

bool AppSettings::getKmlAircraftMode (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_KML_AIRCRAFTMODE);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return ExporterSinkKml::defaultAircraftMode();
}
