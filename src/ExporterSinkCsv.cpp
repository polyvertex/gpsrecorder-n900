//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-03 11:58:47
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSinkCsv
//---------------------------------------------------------------------------
ExporterSinkCsv::ExporterSinkCsv (Exporter* pParent)
: ExporterSink(pParent)
{
  m_cSeparator = ',';

  this->connect(
    pParent,
    SIGNAL(sigSOF(const char*, time_t)),
    SLOT(onSOF(const char*, time_t)) );
  this->connect(
    pParent,
    SIGNAL(sigLocationFix(time_t, const LocationFixContainer&)),
    SLOT(onLocationFix(time_t, const LocationFixContainer&)) );
  this->connect(
    pParent,
    SIGNAL(sigEOF(void)),
    SLOT(onEOF(void)) );
}

//---------------------------------------------------------------------------
// ~ExporterSinkCsv
//---------------------------------------------------------------------------
ExporterSinkCsv::~ExporterSinkCsv (void)
{
  this->close();
}



//---------------------------------------------------------------------------
// close
//---------------------------------------------------------------------------
void ExporterSinkCsv::close (void)
{
  ExporterSink::close();
}



//---------------------------------------------------------------------------
// onSOF
//---------------------------------------------------------------------------
void ExporterSinkCsv::onSOF (const char* pszFilePath, time_t uiTime)
{
  Q_UNUSED(uiTime);

  Q_ASSERT(pszFilePath);
  Q_ASSERT(pszFilePath[0]);

  this->close();
  m_strFilePath  = pszFilePath;
  m_strFilePath += '.';
  m_strFilePath += this->extension();

  m_pFile = fopen(qPrintable(m_strFilePath), "wb");
  if (!m_pFile)
  {
    qWarning("Could not create %s ! Error %d : %s", qPrintable(m_strFilePath), errno, strerror(errno));
    return;
  }

  // write header
  fprintf(m_pFile,
    "Time" "%c" "Latitude" "%c" "Longitude" "%c" "AltitudeM" "%c" "Heading" "%c" "SpeedKmh\n",
    m_cSeparator, m_cSeparator, m_cSeparator, m_cSeparator, m_cSeparator);
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void ExporterSinkCsv::onLocationFix (time_t uiTime, const LocationFixContainer& fixCont)
{
  Q_UNUSED(uiTime);

  Q_ASSERT(fixCont.hasFix());
  if (!m_pFile || !fixCont.hasFix())
    return;

  const LocationFix& fix = *fixCont.getFix();

  if (!fix.hasFields(FIXFIELD_TIME | FIXFIELD_LATLONG | FIXFIELD_ALT | FIXFIELD_TRACK | FIXFIELD_SPEED))
    return;

  fprintf(m_pFile,
    "%u%c" "%.6lf%c" "%.6lf%c" "%i%c" "%.2lf%c" "%.2lf\n",
    fix.uiTime, m_cSeparator,
    fix.getLatDeg(), m_cSeparator,
    fix.getLongDeg(), m_cSeparator,
    fix.iAlt, m_cSeparator,
    fix.getTrackDeg(), m_cSeparator,
    fix.getSpeedKmh() );
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkCsv::onEOF (void)
{
  this->close();
}
