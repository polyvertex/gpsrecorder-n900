//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-03 12:03:01
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define KML_NL  "\r\n"



//---------------------------------------------------------------------------
// ExporterSinkKml
//---------------------------------------------------------------------------
ExporterSinkKml::ExporterSinkKml (Exporter* pParent)
: ExporterSink(pParent)
{
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
    SIGNAL(sigSnappedPoint(const Exporter::SnappedPoint*)),
    SLOT(onSnappedPoint(const Exporter::SnappedPoint*)) );
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

  //m_dtBegin.setTimeSpec(Qt::UTC);
  //m_dtEnd.setTimeSpec(Qt::UTC);
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
  //if (m_dtBegin.isValid() && m_dtEnd.isValid() && m_dtEnd > m_dtBegin)
  //{
  //  fprintf(m_pFile,
  //    " <TimeSpan>" KML_NL
  //    "  <begin>%sZ</begin>" KML_NL
  //    "  <end>%sZ</end>" KML_NL
  //    " </TimeSpan>" KML_NL,
  //    qPrintable(m_dtBegin.toString(Qt::ISODate)),
  //    qPrintable(m_dtEnd.toString(Qt::ISODate)) );
  //}
  //m_dtBegin = QDateTime();
  //m_dtEnd   = QDateTime();
  fputs("</Placemark>" KML_NL, m_pFile);

  // snap the beginning of the track
  if (m_FixContBegin.hasFix())
  {
    const LocationFix& fix = *m_FixContBegin.getFix();
    fprintf(m_pFile,
      "<Placemark>" KML_NL
      " <name>Track Begin</name>" KML_NL
      " <description>" KML_NL
      "  <![CDATA[UTC Time : %s<br />" KML_NL
      "Local Time : %s" KML_NL
      "]]>" KML_NL
      " <visibility>1</visibility>" KML_NL
      " <styleUrl>#track-begin</styleUrl>" KML_NL
      " <Point>" KML_NL
      "  <extrude>1</extrude>" KML_NL
      "  <altitudeMode>%s</altitudeMode>" KML_NL
      "  <coordinates>%.6lf,%.6lf,%d</coordinates>" KML_NL
      " </Point>" KML_NL
      "</Placemark>" KML_NL,
      (fix.hasFields(FIXFIELD_TIME) ? Util::timeString(false, snapPt.uiTime, true).constData() : "?"),
      (fix.hasFields(FIXFIELD_TIME) ? Util::timeString(false, snapPt.uiTime, false).constData() : "?"),
      (m_bAircraftMode ? "absolute" : "clampToGround"),
      fix.getLongDeg(),
      fix.getLatDeg(),
      (fix.hasFields(FIXFIELD_ALT) ? fix.iAlt : 0) );
  }

  // snap the end of the track
  if (m_FixContEnd.hasFix())
  {
    const LocationFix& fix = *m_FixContEnd.getFix();
    fprintf(m_pFile,
      "<Placemark>" KML_NL
      " <name>Track End</name>" KML_NL
      " <description>" KML_NL
      "  <![CDATA[UTC Time : %s<br />" KML_NL
      "Local Time : %s" KML_NL
      "]]>" KML_NL
      " <visibility>1</visibility>" KML_NL
      " <styleUrl>#track-end</styleUrl>" KML_NL
      " <Point>" KML_NL
      "  <extrude>1</extrude>" KML_NL
      "  <altitudeMode>%s</altitudeMode>" KML_NL
      "  <coordinates>%.6lf,%.6lf,%d</coordinates>" KML_NL
      " </Point>" KML_NL
      "</Placemark>" KML_NL,
      (fix.hasFields(FIXFIELD_TIME) ? Util::timeString(false, snapPt.uiTime, true).constData() : "?"),
      (fix.hasFields(FIXFIELD_TIME) ? Util::timeString(false, snapPt.uiTime, false).constData() : "?"),
      (m_bAircraftMode ? "absolute" : "clampToGround"),
      fix.getLongDeg(),
      fix.getLatDeg(),
      (fix.hasFields(FIXFIELD_ALT) ? fix.iAlt : 0) );
  }

  // write snapped points if there is any
  for (int i = 0; i < m_vecSnappedPoints.size(); ++i)
  {
    const Exporter::SnappedPoint& snapPt = m_vecSnappedPoints[i];
    QString   strName(QString("Snap #%1").arg(i + 1));
    //QDateTime dtUTC;

    if (!snapPt.strPointName.isEmpty())
    {
      strName += " : ";
      strName += snapPt.strPointName;
    }

    //dtUTC.setTimeSpec(Qt::UTC);
    //dtUTC.setTime_t(snapPt.uiTime);

    fprintf(m_pFile,
      "<Placemark>" KML_NL
      " <name>%s</name>" KML_NL
      " <description>" KML_NL
      "  <![CDATA[UTC Time : %s<br />" KML_NL
      "Local Time : %s" KML_NL
      "]]>" KML_NL
      " </description>" KML_NL
      " <visibility>1</visibility>" KML_NL
    //" <TimeStamp>" KML_NL
    //"  <when>%sZ</when>" KML_NL
    //" </TimeStamp>" KML_NL
      " <Point>" KML_NL
      "  <extrude>1</extrude>" KML_NL
      "  <altitudeMode>%s</altitudeMode>" KML_NL
      "  <coordinates>%.6lf,%.6lf,%d</coordinates>" KML_NL
      " </Point>" KML_NL
      "</Placemark>" KML_NL,
      qPrintable(strName),
      Util::timeString(false, snapPt.uiTime, true).constData(),
      Util::timeString(false, snapPt.uiTime, false).constData(),
      //qPrintable(dtUTC.toString(Qt::ISODate)),
      (m_bAircraftMode && snapPt.bHasAlt ? "absolute" : "clampToGround"),
      snapPt.rLongDeg, snapPt.rLatDeg, snapPt.iAltM );
  }
  m_vecSnappedPoints.clear();

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

  m_FixContBegin.reset();
  m_FixContEnd.reset();
  m_vecSnappedPoints.clear();

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
void ExporterSinkKml::onSOF (const char* pszFilePath, time_t uiTime)
{
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
    Util::timeString(false, uiTime).constData(),
    this->parent()->gpsrFile().getReadChunksCount(GPSRFile::CHUNK_LOCATIONFIX),
    qPrintable(App::applicationLabel()), qPrintable(QCoreApplication::applicationVersion()), qPrintable(App::applicationUrl()),
    m_strLineColor.constData(),
    m_nLineWidth,
    Util::timeString(false, uiTime).constData(),
    (m_bAircraftMode ? "absolute" : "clampToGround") );

  // TODO : add a snap with the time to mark the beginning of the track
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

  //if (fix.hasFields(FIXFIELD_TIME))
  //{
  //  if (!m_dtBegin.isValid())
  //    m_dtBegin.setTime_t(fix.uiTime);
  //  m_dtEnd.setTime_t(fix.uiTime);
  //}

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
// onSnap
//---------------------------------------------------------------------------
void ExporterSinkKml::onSnappedPoint (const Exporter::SnappedPoint* pSnappedPoint)
{
  //m_dtEnd.setTime_t(pSnappedPoint->uiTime);
  m_vecSnappedPoints.append(*pSnappedPoint);
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
