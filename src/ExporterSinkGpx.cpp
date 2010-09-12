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
// Created On : 2010-04-03 12:06:34
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define GPX_NL  "\r\n"



//---------------------------------------------------------------------------
// ExporterSinkGpx
//---------------------------------------------------------------------------
ExporterSinkGpx::ExporterSinkGpx (Exporter* pParent)
: ExporterSink(pParent)
{
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
    SIGNAL(sigGizmoPoint(const Exporter::GizmoPoint*)),
    SLOT(onGizmoPoint(const Exporter::GizmoPoint*)) );
  this->connect(
    pParent,
    SIGNAL(sigEOF(void)),
    SLOT(onEOF(void)) );
}

//---------------------------------------------------------------------------
// ~ExporterSinkGpx
//---------------------------------------------------------------------------
ExporterSinkGpx::~ExporterSinkGpx (void)
{
  this->close();
}



//---------------------------------------------------------------------------
// writeEOF
//---------------------------------------------------------------------------
void ExporterSinkGpx::writeEOF (void)
{
  Q_ASSERT(m_pFile);

  // write end of track
  fprintf(m_pFile,
    "</trkseg>" GPX_NL
    "</trk>" GPX_NL );

  // write gizmo points
  for (int i = 0; i < m_vecGizmoPoints.size(); ++i)
  {
    const Exporter::GizmoPoint& gizmoPt = m_vecGizmoPoints[i];
    QString strName(QString("Snap %1").arg(i + 1));
    QString strEle;

    if (!gizmoPt.strPointName.isEmpty())
    {
      strName += " : ";
      strName += gizmoPt.strPointName;
    }

    if (gizmoPt.bHasAlt)
      strEle.sprintf(" <ele>%d</ele>" GPX_NL, gizmoPt.iAltM);

    fprintf(m_pFile,
      "<wpt lat=\"%.6lf\" lon=\"%.6lf\">" GPX_NL
      " <time>%s</time>" GPX_NL
      "%s"
      " <name>%s</name>" GPX_NL
      "</wpt>" GPX_NL,
      gizmoPt.rLatDeg, gizmoPt.rLongDeg,
      Util::timeStringIso8601(true, gizmoPt.uiTime).constData(),
      qPrintable(strEle),
      qPrintable(strName) );
  }
  m_vecGizmoPoints.clear();

  // write end of file
  fprintf(m_pFile,
    "</gpx>" GPX_NL );
}

//---------------------------------------------------------------------------
// close
//---------------------------------------------------------------------------
void ExporterSinkGpx::close (void)
{
  // write footer
  if (m_pFile)
    this->writeEOF();

  m_vecGizmoPoints.clear();

  ExporterSink::close();
}



//---------------------------------------------------------------------------
// onSOF
//---------------------------------------------------------------------------
void ExporterSinkGpx::onSOF (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset)
{
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
    "<?xml version=\"1.0\" ?>" GPX_NL
    "<gpx version=\"1.1\" creator=\"%s v%s\" xmlns=\"http://www.topografix.com/GPX/1/1\">" GPX_NL
    "<metadata>" GPX_NL
    " <time>%s</time>" GPX_NL
    "</metadata>" GPX_NL
    "<trk>" GPX_NL
    "<trkseg>" GPX_NL,
    qPrintable(App::applicationLabel()), qPrintable(QCoreApplication::applicationVersion()),
    Util::timeStringIso8601(true, uiTime).constData() );
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void ExporterSinkGpx::onLocationFix (time_t uiTime, const LocationFixContainer& fixCont)
{
  QByteArray strFixMode;
  QByteArray strTime;
  QByteArray strEle;

  Q_ASSERT(fixCont.hasFix());
  if (!m_pFile || !fixCont.hasFix())
    return;

  const LocationFix& fix = *fixCont.getFix();

  if (!fix.hasFields(FIXFIELD_LATLONG))
    return;

  switch (fix.cFixMode)
  {
    case FIXMODE_NOFIX :
      strFixMode = " <fix>none</fix>" GPX_NL;
      break;
    case FIXMODE_2D :
      strFixMode = " <fix>2d</fix>" GPX_NL;
      break;
    case FIXMODE_3D :
      strFixMode = " <fix>3d</fix>" GPX_NL;
      break;
    default :
      // as it is said in GPX format spec :
      // "To signify the fix info is unknown, leave out fixType entirely"
      break;
  }

  if (fix.hasFields(FIXFIELD_TIME))
  {
    strTime.clear();
    strTime += " <time>";
    strTime += Util::timeStringIso8601(true, uiTime);
    strTime += "</time>";
    strTime += GPX_NL;
  }

  if (fix.hasFields(FIXFIELD_ALT))
  {
    strEle.clear();
    strEle += " <ele>";
    strEle += QByteArray::number(fix.iAlt);
    strEle += "</ele>";
    strEle += GPX_NL;
  }

  fprintf(m_pFile,
    "<trkpt lat=\"%.6lf\" lon=\"%.6lf\">" GPX_NL
    "%s"
    "%s"
    "%s"
    " <sat>%i</sat>" GPX_NL
    "</trkpt>" GPX_NL,
    fix.getLatDeg(), fix.getLongDeg(),
    strTime.constData(),
    strFixMode.constData(),
    strEle.constData(),
    (int)fix.cSatUse );
}

//---------------------------------------------------------------------------
// onGizmoPoint
//---------------------------------------------------------------------------
void ExporterSinkGpx::onGizmoPoint (const Exporter::GizmoPoint* pGizmoPoint)
{
  m_vecGizmoPoints.append(*pGizmoPoint);
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkGpx::onEOF (void)
{
  this->close();
}
