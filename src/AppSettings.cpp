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
// Created On : 2010-04-27 13:06:48
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Static Members
//---------------------------------------------------------------------------
static const char* SETTINGNAME_OUTPUTDIR = "OutputDir";
//
static const char* SETTINGNAME_LOGSTEP            = "LogStep";
static const char* SETTINGNAME_GPSASSISTED        = "GpsAssisted";
static const char* SETTINGNAME_GPSALWAYSCONNECTED = "GpsAlwaysConnected";
static const char* SETTINGNAME_ASKTRACKNAME       = "AskTrackName";
//
static const char* SETTINGNAME_CONVERT_CSV = "ConvertCsv";
static const char* SETTINGNAME_CONVERT_GPX = "ConvertGpx";
static const char* SETTINGNAME_CONVERT_KML = "ConvertKml";
//
static const char* SETTINGNAME_CSV_SEPARATOR = "CsvSeparator";
//
static const char* SETTINGNAME_KML_ZIPPED       = "KmlZipped";
static const char* SETTINGNAME_KML_LINECOLOR    = "KmlLineColor";
static const char* SETTINGNAME_KML_LINEWIDTH    = "KmlLineWidth";
static const char* SETTINGNAME_KML_AIRCRAFTMODE = "KmlAircraftMode";
//atic const char* SETTINGNAME_KML_COLORBYSPEED = "KmlColorBySpeed";



//---------------------------------------------------------------------------
// AppSettings
//---------------------------------------------------------------------------
AppSettings::AppSettings (QObject* pParent/*=0*/)
: QObject(pParent)
{
  // ensure the QSettings default constructor has loaded *our* settings
  Q_ASSERT(QCoreApplication::organizationName().isEmpty() == false);
  Q_ASSERT(QCoreApplication::applicationName().isEmpty() == false);

  // refresh cloned log step value
  this->getLogStep_Impl();
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

  // refresh cloned log step value
  this->getLogStep_Impl();

  emit sigSettingsWritten();

  return eStatus;
}



//---------------------------------------------------------------------------
// OutputDirectory
//---------------------------------------------------------------------------
QByteArray AppSettings::readOutputDirectory (void)
{
  // ensure the QSettings default constructor has loaded *our* settings
  Q_ASSERT(QCoreApplication::organizationName().isEmpty() == false);
  Q_ASSERT(QCoreApplication::applicationName().isEmpty() == false);

  QSettings settings;

  QVariant var = settings.value(SETTINGNAME_OUTPUTDIR);
  if (var.type() == QVariant::String)
    return qPrintable(var.toString());

  return QByteArray();
}

void AppSettings::writeOutputDirectory (const QByteArray& strOutputDir)
{
  // ensure the QSettings default constructor has loaded *our* settings
  Q_ASSERT(QCoreApplication::organizationName().isEmpty() == false);
  Q_ASSERT(QCoreApplication::applicationName().isEmpty() == false);

  QSettings settings;
  settings.setValue(SETTINGNAME_OUTPUTDIR, QVariant(QLatin1String(strOutputDir.constData())));
}

//---------------------------------------------------------------------------
// LogStep
//---------------------------------------------------------------------------
void AppSettings::setLogStep (uint uiLogStepSeconds)
{
  m_Settings.setValue(SETTINGNAME_LOGSTEP, QVariant(uiLogStepSeconds));
}

uint AppSettings::getLogStep_Impl (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_LOGSTEP);
  if (var.canConvert(QVariant::UInt))
  {
    bool bOk = false;
    uint ui = var.toUInt(&bOk);
    QPair<uint,uint> bounds = AppSettings::logStepBounds();

    if (bOk && ui >= bounds.first && ui <= bounds.second)
    {
      m_uiLogStepClone = ui;
      return ui;
    }
    else
    {
      this->setLogStep(AppSettings::defaultLogStep());
    }
  }

  m_uiLogStepClone = AppSettings::defaultLogStep();
  return AppSettings::defaultLogStep();
}

//---------------------------------------------------------------------------
// GpsAssisted
//---------------------------------------------------------------------------
void AppSettings::setGpsAssisted (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_GPSASSISTED, QVariant(bEnable));
}

bool AppSettings::getGpsAssisted (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_GPSASSISTED);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultGpsAssisted();
}

//---------------------------------------------------------------------------
// GpsAlwaysConnected
//---------------------------------------------------------------------------
void AppSettings::setGpsAlwaysConnected (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_GPSALWAYSCONNECTED, QVariant(bEnable));
}

bool AppSettings::getGpsAlwaysConnected (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_GPSALWAYSCONNECTED);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultGpsAlwaysConnected();
}

//---------------------------------------------------------------------------
// AskTrackName
//---------------------------------------------------------------------------
void AppSettings::setAskTrackName (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_ASKTRACKNAME, QVariant(bEnable));
}

bool AppSettings::getAskTrackName (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_ASKTRACKNAME);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultAskTrackName();
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
// KmlZipped
//---------------------------------------------------------------------------
void AppSettings::setKmlZipped (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_KML_ZIPPED, QVariant(bEnable));
}

bool AppSettings::getKmlZipped (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_KML_ZIPPED);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return ExporterSinkKml::defaultZipped();
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
