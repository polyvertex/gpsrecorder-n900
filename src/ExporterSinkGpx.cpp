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
    QString strName;
    QString strEle;

    if (gizmoPt.eType == Exporter::GIZMO_TRACK)
    {
      strName = QString("Track %1").arg(gizmoPt.uiTypeIndex + 1);
    }
    else if (gizmoPt.eType == Exporter::GIZMO_PAUSE)
    {
      if (!m_bExportPause)
        continue;
      strName = QString("Pause %1").arg(gizmoPt.uiTypeIndex + 1);
    }
    else if (gizmoPt.eType == Exporter::GIZMO_RESUME)
    {
      if (!m_bExportPause)
        continue;
      strName = "Resume";
    }
    else if (gizmoPt.eType != Exporter::GIZMO_MEANSTRANSPORT)
    {
      strName = QString("Snap %1").arg(gizmoPt.uiTypeIndex + 1);
    }

    if (!gizmoPt.strName.isEmpty())
    {
      strName += " : ";
      strName += gizmoPt.strName;
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
