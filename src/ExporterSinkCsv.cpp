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
  m_cSeparator = ExporterSinkCsv::defaultSeparator();

  this->connect(
    pParent,
    SIGNAL(sigSOF(const char*, time_t, qint32)),
    SLOT(onSOF(const char*, time_t, qint32)) );
  this->connect(
    pParent,
    SIGNAL(sigLocationFix(time_t, const LocationFixContainer&)),
    SLOT(onLocationFix(time_t, const LocationFixContainer&)) );
  this->connect(
    pParent,
    SIGNAL(sigEOF(void)),
    SLOT(onEOF(void)) );

  // read settings
  m_cSeparator = App::instance()->settings()->getCsvSeparator();
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
// separatorIndex
//---------------------------------------------------------------------------
int ExporterSinkCsv::separatorIndex (char cSep)
{
  const char* pszSeps = ExporterSinkCsv::supportedSeparators();
  char* psz = strchr(pszSeps, cSep);

  return !psz ? -1 : int(psz - pszSeps);
}



//---------------------------------------------------------------------------
// onSOF
//---------------------------------------------------------------------------
void ExporterSinkCsv::onSOF (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset)
{
  Q_UNUSED(uiTime);
  Q_UNUSED(iTimeZoneOffset);

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

  if (!fix.hasFields(FIXFIELD_LATLONG))
    return;

  fprintf(m_pFile,
    "%u%c" "%.6lf%c" "%.6lf%c" "%i%c" "%.2lf%c" "%.2lf\n",
    (fix.hasFields(FIXFIELD_TIME) ? fix.uiTime : 0), m_cSeparator,
    fix.getLatDeg(), m_cSeparator,
    fix.getLongDeg(), m_cSeparator,
    (fix.hasFields(FIXFIELD_ALT) ? fix.iAlt : 0), m_cSeparator,
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
