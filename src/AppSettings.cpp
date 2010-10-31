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

static const char* SETTINGNAME_LOGSTEP             = "LogStep";
static const char* SETTINGNAME_GPSASSISTED         = "GpsAssisted";
static const char* SETTINGNAME_GPSALWAYSCONNECTED  = "GpsAlwaysConnected";
static const char* SETTINGNAME_ALWAYSCREATENEWFILE = "AlwaysCreateNewFile";
static const char* SETTINGNAME_ASKTRACKNAME        = "AskTrackName"; // obsolete
static const char* SETTINGNAME_ASKPOINTNAME        = "AskPointName";
static const char* SETTINGNAME_ASKPAUSENAME        = "AskPauseName";
static const char* SETTINGNAME_UNITSYSTEM          = "UnitSystem";
static const char* SETTINGNAME_HORIZSPEEDUNIT      = "HorizSpeedUnit";
static const char* SETTINGNAME_PREVENTBLANKSCREEN  = "PreventBlankScreen";

static const char* SETTINGNAME_LASTMEANSOFTRANSPORT      = "LastMeansOfTransport";
static const char* SETTINGNAME_LASTOTHERMEANSOFTRANSPORT = "LastOtherMeansOfTransport";

static const char* SETTINGNAME_CONVERT_TXT          = "ConvertTxt";
static const char* SETTINGNAME_CONVERT_CSV          = "ConvertCsv";
static const char* SETTINGNAME_CONVERT_GPX          = "ConvertGpx";
static const char* SETTINGNAME_CONVERT_KML          = "ConvertKml";
static const char* SETTINGNAME_CONVERT_EXPORTPAUSES = "ConvertExportPauses";

static const char* SETTINGNAME_TXT_INCLUDELOCFIX = "TxtIncludeLocationFix";

static const char* SETTINGNAME_CSV_SEPARATOR = "CsvSeparator";

static const char* SETTINGNAME_KML_ZIPPED       = "KmlZipped";
static const char* SETTINGNAME_KML_LINECOLOR    = "KmlLineColor";
static const char* SETTINGNAME_KML_LINEWIDTH    = "KmlLineWidth";
static const char* SETTINGNAME_KML_AIRCRAFTMODE = "KmlAircraftMode";
//atic const char* SETTINGNAME_KML_COLORBYSPEED = "KmlColorBySpeed";



//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
static const struct UNITSYSTEMS_
{
  uint        uiId;
  const char* pszName;
}
UNITSYSTEMS[] =
{
  { UNITSYSTEM_METRIC,   "Metric"   },
  { UNITSYSTEM_IMPERIAL, "Imperial" },
};
static const int UNITSYSTEMS_COUNT = sizeof(UNITSYSTEMS) / sizeof(UNITSYSTEMS[0]);

static const struct HORIZSPEEDUNITS_
{
  uint        uiId;
  const char* pszName;
}
HORIZSPEEDUNITS[] =
{
  { HORIZSPEEDUNIT_KMH,   "km/h"  },
  { HORIZSPEEDUNIT_MPH,   "mph"   },
  { HORIZSPEEDUNIT_MS,    "m/s"   },
  { HORIZSPEEDUNIT_KNOTS, "knots" },
};
static const int HORIZSPEEDUNITS_COUNT = sizeof(HORIZSPEEDUNITS) / sizeof(HORIZSPEEDUNITS[0]);



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

  // delete obsolete settings to cleanup resulting file/registry
  m_Settings.remove(SETTINGNAME_ASKTRACKNAME);
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
// AlwaysCreateNewFile
//---------------------------------------------------------------------------
void AppSettings::setAlwaysCreateNewFile (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_ALWAYSCREATENEWFILE, QVariant(bEnable));
}

bool AppSettings::getAlwaysCreateNewFile (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_ALWAYSCREATENEWFILE);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultAlwaysCreateNewFile();
}

//---------------------------------------------------------------------------
// AskPointName
//---------------------------------------------------------------------------
void AppSettings::setAskPointName (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_ASKPOINTNAME, QVariant(bEnable));
}

bool AppSettings::getAskPointName (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_ASKPOINTNAME);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultAskPointName();
}

//---------------------------------------------------------------------------
// AskPauseName
//---------------------------------------------------------------------------
void AppSettings::setAskPauseName (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_ASKPAUSENAME, QVariant(bEnable));
}

bool AppSettings::getAskPauseName (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_ASKPAUSENAME);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultAskPauseName();
}

//---------------------------------------------------------------------------
// UnitSystem
//---------------------------------------------------------------------------
void AppSettings::setUnitSystem (uint uiUnitSystem)
{
  m_Settings.setValue(SETTINGNAME_UNITSYSTEM, QVariant(uiUnitSystem));
}

uint AppSettings::getUnitSystem (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_UNITSYSTEM);
  if (var.canConvert(QVariant::UInt))
  {
    bool bOk = false;
    uint ui = var.toUInt(&bOk);

    if (bOk && AppSettings::unitSystemValidate(ui))
      return ui;
    else
      this->setUnitSystem(AppSettings::defaultUnitSystem());
  }

  return AppSettings::defaultUnitSystem();
}

//---------------------------------------------------------------------------
// HorizSpeedUnit
//---------------------------------------------------------------------------
void AppSettings::setHorizSpeedUnit (uint uiHorizSpeedUnit)
{
  m_Settings.setValue(SETTINGNAME_HORIZSPEEDUNIT, QVariant(uiHorizSpeedUnit));
}

uint AppSettings::getHorizSpeedUnit (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_HORIZSPEEDUNIT);
  if (var.canConvert(QVariant::UInt))
  {
    bool bOk = false;
    uint ui = var.toUInt(&bOk);

    if (bOk && AppSettings::horizSpeedUnitValidate(ui))
      return ui;
    else
      this->setHorizSpeedUnit(AppSettings::defaultHorizSpeedUnit());
  }

  return AppSettings::defaultHorizSpeedUnit();
}

//---------------------------------------------------------------------------
// PreventBlankScreen
//---------------------------------------------------------------------------
void AppSettings::setPreventBlankScreen (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_PREVENTBLANKSCREEN, QVariant(bEnable));
}

bool AppSettings::getPreventBlankScreen (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_PREVENTBLANKSCREEN);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultPreventBlankScreen();
}

//---------------------------------------------------------------------------
// LastMeansOfTransport
//---------------------------------------------------------------------------
void AppSettings::setLastMeansOfTransport (quint8 ucMeansOfTransport)
{
  m_Settings.setValue(SETTINGNAME_LASTMEANSOFTRANSPORT, QVariant((int)ucMeansOfTransport));
}

quint8 AppSettings::getLastMeansOfTransport (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_LASTMEANSOFTRANSPORT);
  if (var.canConvert(QVariant::Int))
  {
    bool bOk = false;
    int  i = var.toInt(&bOk);

    if (bOk && GPSRFile::isValidMeansOfTransport((quint8)i))
      return (quint8)i;
    else
      this->setLastMeansOfTransport(AppSettings::defaultMeansOfTransport());
  }

  return AppSettings::defaultMeansOfTransport();
}

//---------------------------------------------------------------------------
// LastOtherMeansOfTransport
//---------------------------------------------------------------------------
void AppSettings::setLastOtherMeansOfTransport (const QByteArray& strOtherMeansOfTransport)
{
  QString str = GPSRFile::validateOtherMeansOfTransport(strOtherMeansOfTransport.constData());
  m_Settings.setValue(SETTINGNAME_LASTOTHERMEANSOFTRANSPORT, QVariant(QLatin1String(qPrintable(str))));
}

QByteArray AppSettings::getLastOtherMeansOfTransport (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_LASTOTHERMEANSOFTRANSPORT);
  if (var.type() == QVariant::String)
  {
    QString str = GPSRFile::validateOtherMeansOfTransport(var.toString());
    this->setLastOtherMeansOfTransport(qPrintable(str));
    return qPrintable(str);
  }

  return QByteArray();
}

//---------------------------------------------------------------------------
// ConvertTxt
//---------------------------------------------------------------------------
void AppSettings::setConvertTxt (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_CONVERT_TXT, QVariant(bEnable));
}

bool AppSettings::getConvertTxt (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_CONVERT_TXT);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultConvertTxt();
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
// ConvertExportPauses
//---------------------------------------------------------------------------
void AppSettings::setConvertExportPauses (bool bEnable)
{
  m_Settings.setValue(SETTINGNAME_CONVERT_EXPORTPAUSES, QVariant(bEnable));
}

bool AppSettings::getConvertExportPauses (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_CONVERT_EXPORTPAUSES);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return AppSettings::defaultConvertExportPauses();
}

//---------------------------------------------------------------------------
// TxtIncludeLocationFix
//---------------------------------------------------------------------------
void AppSettings::setTxtIncludeLocationFix (bool bInclude)
{
  m_Settings.setValue(SETTINGNAME_TXT_INCLUDELOCFIX, QVariant(bInclude));
}

bool AppSettings::getTxtIncludeLocationFix (void)
{
  QVariant var = m_Settings.value(SETTINGNAME_TXT_INCLUDELOCFIX);
  if (var.canConvert(QVariant::Bool))
    return var.toBool();

  return ExporterSinkTxt::defaultIncludelocationFix();
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
  QVariant var = m_Settings.value(SETTINGNAME_CSV_SEPARATOR);
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



//---------------------------------------------------------------------------
// unitSystemValidate
//---------------------------------------------------------------------------
bool AppSettings::unitSystemValidate (uint uiUnitSystem)
{
  for (int i = 0; i < UNITSYSTEMS_COUNT; ++i)
  {
    if (uiUnitSystem == UNITSYSTEMS[i].uiId)
      return true;
  }

  return false;
}

//---------------------------------------------------------------------------
// unitSystemToName
//---------------------------------------------------------------------------
const char* AppSettings::unitSystemToName (uint uiUnitSystem)
{
  for (int i = 0; i < UNITSYSTEMS_COUNT; ++i)
  {
    if (uiUnitSystem == UNITSYSTEMS[i].uiId)
      return UNITSYSTEMS[i].pszName;
  }

  return NULL;
}

//---------------------------------------------------------------------------
// horizSpeedUnitValidate
//---------------------------------------------------------------------------
bool AppSettings::horizSpeedUnitValidate (uint uiHorizSpeedUnit)
{
  for (int i = 0; i < HORIZSPEEDUNITS_COUNT; ++i)
  {
    if (uiHorizSpeedUnit == HORIZSPEEDUNITS[i].uiId)
      return true;
  }

  return false;
}

//---------------------------------------------------------------------------
// horizSpeedUnitToName
//---------------------------------------------------------------------------
const char* AppSettings::horizSpeedUnitToName (uint uiHorizSpeedUnit)
{
  for (int i = 0; i < HORIZSPEEDUNITS_COUNT; ++i)
  {
    if (uiHorizSpeedUnit == HORIZSPEEDUNITS[i].uiId)
      return HORIZSPEEDUNITS[i].pszName;
  }

  return NULL;
}
