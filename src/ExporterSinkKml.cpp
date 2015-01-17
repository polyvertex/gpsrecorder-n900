//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex@gmail.com>
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
// Created: 2010-04-03 12:03:01
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define KML_NL  "\r\n"

//#define KML_TIMESPAN



//---------------------------------------------------------------------------
// ExporterSinkKml
//---------------------------------------------------------------------------
ExporterSinkKml::ExporterSinkKml (Exporter* pParent)
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

  // read settings
  {
    AppSettings& settings = *App::instance()->settings();

    m_bZipped       = settings.getKmlZipped();
    m_strLineColor  = qPrintable(ExporterSinkKml::colorToString(settings.getKmlLineColor()));
    m_nLineWidth    = settings.getKmlLineWidth();
    m_bAircraftMode = settings.getKmlAircraftMode();
  }
}

//---------------------------------------------------------------------------
// ~ExporterSinkKml
//---------------------------------------------------------------------------
ExporterSinkKml::~ExporterSinkKml (void)
{
  this->close();
}



//---------------------------------------------------------------------------
// colorToString
//---------------------------------------------------------------------------
QString ExporterSinkKml::colorToString (const QColor& color)
{
  // in kml format, color components are ABGR ordered, so for example,
  // "FF00FF00" is green and "FF00FFFF" is yellow
  return QString().sprintf("%02X%02X%02X%02X",
    color.alpha(),
    color.blue(),
    color.green(),
    color.red());
}



//---------------------------------------------------------------------------
// writeEOF
//---------------------------------------------------------------------------
void ExporterSinkKml::writeEOF (void)
{
  Q_ASSERT(m_pFile);

  // terminate track line
  fputs(
    "  </coordinates>" KML_NL
    " </LineString>" KML_NL,
    m_pFile);
#ifdef KML_TIMESPAN
  if (m_uiTimeBegin && m_uiTimeEnd && m_uiTimeEnd > m_uiTimeBegin)
  {
    fprintf(m_pFile,
      " <TimeSpan>" KML_NL
      "  <begin>%s</begin>" KML_NL
      "  <end>%s</end>" KML_NL
      " </TimeSpan>" KML_NL,
      Util::timeStringIso8601(false, m_uiTimeBegin, m_iTimeZoneOffset).constData(),
      Util::timeStringIso8601(false, m_uiTimeEnd, m_iTimeZoneOffset).constData() );
  }
#endif
  m_uiTimeBegin = 0;
  m_uiTimeEnd   = 0;
  fputs("</Placemark>" KML_NL, m_pFile);

  // snap the beginning of the track
  if (m_FixContBegin.hasFix())
  {
    const LocationFix& fix = *m_FixContBegin.getFix();
    fprintf(m_pFile,
      "<Placemark>" KML_NL
      " <name>Begin</name>" KML_NL
      " <description>" KML_NL
      "  <![CDATA[Time : %s<br />" KML_NL
      "]]>" KML_NL
      " </description>" KML_NL
      " <visibility>1</visibility>" KML_NL
      " <styleUrl>#track-begin</styleUrl>" KML_NL
      " <Point>" KML_NL
      "  <extrude>1</extrude>" KML_NL
      "  <altitudeMode>%s</altitudeMode>" KML_NL
      "  <coordinates>%.6lf,%.6lf,%d</coordinates>" KML_NL
      " </Point>" KML_NL
      "</Placemark>" KML_NL,
      (fix.hasFields(FIXFIELD_TIME) ? Util::timeString(false, fix.uiTime, m_iTimeZoneOffset).constData() : "?"),
      (m_bAircraftMode ? "absolute" : "clampToGround"),
      fix.getLongDeg(),
      fix.getLatDeg(),
      (fix.hasFields(FIXFIELD_ALT) ? fix.iAlt : 0) );
  }

  // snap the end of the track
  if (m_FixContEnd.hasFix())
  {
    const LocationFix& fix = *m_FixContEnd.getFix();
    QByteArray strMessage;

    if (this->parent()->isIncompleteFile())
    {
      strMessage  = "<strong>";
      strMessage += QT_TR_NOOP("WARNING : Source file was incomplete !");
      strMessage += "</strong><br />" KML_NL;
    }

    fprintf(m_pFile,
      "<Placemark>" KML_NL
      " <name>End</name>" KML_NL
      " <description>" KML_NL
      "  <![CDATA[Time : %s<br />" KML_NL
      "%s]]>" KML_NL
      " </description>" KML_NL
      " <visibility>1</visibility>" KML_NL
      " <styleUrl>#track-end</styleUrl>" KML_NL
      " <Point>" KML_NL
      "  <extrude>1</extrude>" KML_NL
      "  <altitudeMode>%s</altitudeMode>" KML_NL
      "  <coordinates>%.6lf,%.6lf,%d</coordinates>" KML_NL
      " </Point>" KML_NL
      "</Placemark>" KML_NL,
      (fix.hasFields(FIXFIELD_TIME) ? Util::timeString(false, fix.uiTime, m_iTimeZoneOffset).constData() : "?"),
      strMessage.constData(),
      (m_bAircraftMode ? "absolute" : "clampToGround"),
      fix.getLongDeg(),
      fix.getLatDeg(),
      (fix.hasFields(FIXFIELD_ALT) ? fix.iAlt : 0) );
  }

  // write gizmo points
  for (int i = 0; i < m_vecGizmoPoints.size(); ++i)
  {
    const Exporter::GizmoPoint& gizmoPt = m_vecGizmoPoints[i];
    QString strName;
    QString strStyle;

    if (gizmoPt.eType == Exporter::GIZMO_TRACK)
    {
      strName  = QString("Track %1").arg(gizmoPt.uiTypeIndex + 1);
      strStyle = " <styleUrl>#track-new</styleUrl>" KML_NL;
    }
    else if (gizmoPt.eType == Exporter::GIZMO_MEANSTRANSPORT)
    {
      strName  = "Means of transport : ";
      strName += GPSRFile::fullMeansOfTransportToLabel(gizmoPt.data.ucMeansOfTransport, gizmoPt.strName.constData());
    }
    else if (gizmoPt.eType == Exporter::GIZMO_SNAP)
    {
      strName = QString("Snap %1").arg(gizmoPt.uiTypeIndex + 1);
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

    if (!gizmoPt.strName.isEmpty())
    {
      strName += " : ";
      strName += gizmoPt.strName;
    }

    fprintf(m_pFile,
      "<Placemark>" KML_NL
      " <name>%s</name>" KML_NL
      " <description>" KML_NL
      "  <![CDATA[Time : %s<br />" KML_NL
      "]]>" KML_NL
      " </description>" KML_NL
      " <visibility>1</visibility>" KML_NL
      "%s"
#ifdef KML_TIMESPAN
      " <TimeStamp>" KML_NL
      "  <when>%s</when>" KML_NL
      " </TimeStamp>" KML_NL
#endif
      " <Point>" KML_NL
      "  <extrude>1</extrude>" KML_NL
      "  <altitudeMode>%s</altitudeMode>" KML_NL
      "  <coordinates>%.6lf,%.6lf,%d</coordinates>" KML_NL
      " </Point>" KML_NL
      "</Placemark>" KML_NL,
      qPrintable(strName),
      Util::timeString(false, gizmoPt.uiTime, m_iTimeZoneOffset).constData(),
      qPrintable(strStyle),
#ifdef KML_TIMESPAN
      Util::timeString(false, gizmoPt.uiTime, m_iTimeZoneOffset).constData(),
#endif
      (m_bAircraftMode && gizmoPt.bHasAlt ? "absolute" : "clampToGround"),
      gizmoPt.rLongDeg, gizmoPt.rLatDeg, gizmoPt.iAltM );
  }
  m_vecGizmoPoints.clear();

  // write end of file
  fputs(
    "</Document>" KML_NL
    "</kml>" KML_NL,
    m_pFile);
}

//---------------------------------------------------------------------------
// close
//---------------------------------------------------------------------------
void ExporterSinkKml::close (void)
{
  QString strFilePathBak;

  // write footer
  if (m_pFile)
  {
    this->writeEOF();
    strFilePathBak = m_strFilePath;
  }

  m_iTimeZoneOffset = 0;
  m_uiTimeBegin     = 0;
  m_uiTimeEnd       = 0;
  m_FixContBegin.reset();
  m_FixContEnd.reset();
  m_vecGizmoPoints.clear();

  ExporterSink::close();

  if (m_bZipped && !strFilePathBak.isEmpty())
  {
    if (ExporterSinkKml::kmlToKmz(strFilePathBak))
      QFile::remove(strFilePathBak);
  }
}



//---------------------------------------------------------------------------
// onSOF
//---------------------------------------------------------------------------
void ExporterSinkKml::onSOF (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset)
{
  Q_ASSERT(pszFilePath);
  Q_ASSERT(pszFilePath[0]);

  this->close();
  m_strFilePath  = pszFilePath;
  m_strFilePath += '.';
  m_strFilePath += this->extension();

  m_iTimeZoneOffset = iTimeZoneOffset;

  m_pFile = fopen(qPrintable(m_strFilePath), "wb");
  if (!m_pFile)
  {
    qWarning("Could not create %s ! Error %d : %s", qPrintable(m_strFilePath), errno, strerror(errno));
    return;
  }

  // write header
  fprintf(m_pFile,
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" KML_NL
    "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" KML_NL
    "<Document>" KML_NL
    " <name>GPS Track %s</name>" KML_NL
    " <description>" KML_NL
    "  <![CDATA[%d points.<br />" KML_NL
    "<br />" KML_NL
    "KML file created by %s v%s. Please visit %s for more info." KML_NL
    "]]>" KML_NL
    " </description>" KML_NL
    " <visibility>1</visibility>" KML_NL
    " <open>1</open>" KML_NL
    " <Style id=\"track-begin\">" KML_NL
    "  <IconStyle>" KML_NL
    "   <Icon>" KML_NL
    "    <href>http://maps.google.com/mapfiles/kml/paddle/A.png</href>" KML_NL
    "   </Icon>" KML_NL
    "  </IconStyle>" KML_NL
    " </Style>" KML_NL
    " <Style id=\"track-end\">" KML_NL
    "  <IconStyle>" KML_NL
    "   <Icon>" KML_NL
    "    <href>http://maps.google.com/mapfiles/kml/paddle/B.png</href>" KML_NL
    "   </Icon>" KML_NL
    "  </IconStyle>" KML_NL
    " </Style>" KML_NL
    " <Style id=\"track-new\">" KML_NL
    "  <IconStyle>" KML_NL
    "   <Icon>" KML_NL
    "    <href>http://maps.google.com/mapfiles/kml/paddle/grn-diamond.png</href>" KML_NL
    "   </Icon>" KML_NL
    "  </IconStyle>" KML_NL
    " </Style>" KML_NL
    " <Style id=\"track-line\">" KML_NL
    "  <LineStyle>" KML_NL
    "   <color>%s</color>" KML_NL
    "   <width>%d</width>" KML_NL
    "  </LineStyle>" KML_NL
    " </Style>" KML_NL
    " <Placemark>" KML_NL
    "  <name>Track %s</name>" KML_NL
    "  <visibility>1</visibility>" KML_NL
    "  <styleUrl>#track-line</styleUrl>" KML_NL
    "  <LineString>" KML_NL
    "   <extrude>1</extrude>" KML_NL
    "   <tesselate>1</tesselate>" KML_NL
    "   <altitudeMode>%s</altitudeMode>" KML_NL
    "   <coordinates>" KML_NL,
    Util::timeString(false, uiTime, m_iTimeZoneOffset).constData(),
    this->parent()->gpsrFile().getReadChunksCount(GPSRFile::CHUNK_LOCATIONFIX),
    qPrintable(App::applicationLabel()), qPrintable(QCoreApplication::applicationVersion()), qPrintable(App::applicationUrl()),
    m_strLineColor.constData(),
    m_nLineWidth,
    Util::timeString(false, uiTime, m_iTimeZoneOffset).constData(),
    (m_bAircraftMode ? "absolute" : "clampToGround") );
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void ExporterSinkKml::onLocationFix (time_t uiTime, const LocationFixContainer& fixCont)
{
  Q_UNUSED(uiTime);

  Q_ASSERT(fixCont.hasFix());
  if (!m_pFile || !fixCont.hasFix())
    return;

  const LocationFix& fix = *fixCont.getFix();

#ifdef KML_TIMESPAN
  if (fix.hasFields(FIXFIELD_TIME))
  {
    if (!m_uiTimeBegin)
      m_uiTimeBegin = fix.uiTime;
    m_uiTimeEnd = fix.uiTime;
  }
#endif

  if (!fix.hasFields(FIXFIELD_LATLONG))
    return;

  if (!m_FixContBegin.hasFix())
    m_FixContBegin.setFix(fixCont);
  m_FixContEnd.setFix(fixCont);

  fprintf(m_pFile,
    "%.6lf,%.6lf,%d\n",
    fix.getLongDeg(),
    fix.getLatDeg(),
    fix.hasFields(FIXFIELD_ALT) ? fix.iAlt : 0);
}

//---------------------------------------------------------------------------
// onGizmoPoint
//---------------------------------------------------------------------------
void ExporterSinkKml::onGizmoPoint (const Exporter::GizmoPoint* pGizmoPoint)
{
  m_uiTimeEnd = pGizmoPoint->uiTime;
  m_vecGizmoPoints.append(*pGizmoPoint);
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkKml::onEOF (void)
{
  this->close();
}



//---------------------------------------------------------------------------
// kmlToKmz
//---------------------------------------------------------------------------
bool ExporterSinkKml::kmlToKmz (const QString& strSrcFile)
{
  QString   strDstFile;
  QFileInfo fi(strSrcFile);
  zipFile   hZip;

  strDstFile  = strSrcFile.left(strSrcFile.length() - 1);
  strDstFile += 'z';

  // create zip archive
  hZip = zipOpen(qPrintable(strDstFile), APPEND_STATUS_CREATE);
  if (!hZip)
  {
    qCritical("Could not create %s !", qPrintable(strDstFile));
    return false;
  }

  QDateTime    dt;
  zip_fileinfo zfi;
  int          nRes;

  // prepare zip entry info
  dt.setTimeSpec(Qt::LocalTime);
  zfi.tmz_date.tm_sec  = dt.time().second();
  zfi.tmz_date.tm_min  = dt.time().minute();
  zfi.tmz_date.tm_hour = dt.time().hour();
  zfi.tmz_date.tm_mday = dt.date().day();
  zfi.tmz_date.tm_mon  = dt.date().month() - 1;
  zfi.tmz_date.tm_year = dt.date().year();
  zfi.dosDate          = 0;
  zfi.internal_fa      = 0;
  zfi.external_fa      = 0;

  // open zip file entry
  nRes = zipOpenNewFileInZip(hZip, qPrintable(fi.fileName()), &zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_BEST_COMPRESSION);
  if (nRes != ZIP_OK)
  {
    qCritical("Could not create file in zip archive %s (code %d) !", qPrintable(strDstFile), nRes);

    zipClose(hZip, NULL);
    QFile::remove(strDstFile);
    return false;
  }

  // open kml source file
  QFile f(strSrcFile);
  if (!f.open(QIODevice::ReadOnly))
  {
    qCritical("Could not open KML source file %s !", qPrintable(strSrcFile));

    zipCloseFileInZip(hZip);
    zipClose(hZip, NULL);
    QFile::remove(strDstFile);
    return false;
  }

  // read, compress and write source file into zip archive
  while (!f.atEnd())
  {
    QByteArray block = f.read(0x8000); // 32K
    if (block.size() <= 0)
      continue;

    nRes = zipWriteInFileInZip(hZip, block.constData(), (uint)block.size());
    if (nRes != ZIP_OK)
    {
      qCritical("Error while writing to zip file %s (code %d) !", qPrintable(strDstFile), nRes);

      zipCloseFileInZip(hZip);
      zipClose(hZip, NULL);
      QFile::remove(strDstFile);
      return false;
    }
  }

  // close zip file entry
  nRes = zipCloseFileInZip(hZip);
  if (nRes != ZIP_OK)
  {
    qCritical("Could not close zip in-file (code %d) !", nRes);

    zipClose(hZip, NULL);
    QFile::remove(strDstFile);
    return false;
  }

  // close zip archive
  nRes = zipClose(hZip, NULL);
  if (nRes != ZIP_OK)
  {
    qCritical("Could not close zip file %s (code %d) !", qPrintable(strDstFile), nRes);

    zipClose(hZip, NULL);
    QFile::remove(strDstFile);
    return false;
  }

  return true;
}
