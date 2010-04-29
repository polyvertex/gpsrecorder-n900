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
// close
//---------------------------------------------------------------------------
void ExporterSinkKml::close (void)
{
  // write footer
  if (m_pFile)
  {
    fprintf(m_pFile,
      "  </coordinates>" KML_NL
      " </LineString>" KML_NL
      "</Placemark>" KML_NL
      "</kml>" KML_NL );
  }

  ExporterSink::close();
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
    "<Placemark>" KML_NL
    " <name>GPS Track %s</name>" KML_NL
    " <description>" KML_NL
    "  <![CDATA[" KML_NL
    "   %d waypoints.<br />" KML_NL
    "   <br />" KML_NL
    "   KML file created by %s v%s. Please visit %s for more info." KML_NL
    "  ]]>" KML_NL
    " </description>" KML_NL
    " <visibility>1</visibility>" KML_NL
    " <open>1</open>" KML_NL
    " <Style>" KML_NL
    "  <LineStyle>" KML_NL
    "   <color>%s</color>" KML_NL
    "   <width>%d</width>" KML_NL
    "  </LineStyle>" KML_NL
    " </Style>" KML_NL
    " <LineString>" KML_NL
    "  <extrude>1</extrude>" KML_NL
    "  <tesselate>1</tesselate>" KML_NL
    "  <altitudeMode>%s</altitudeMode>" KML_NL
    "  <coordinates>" KML_NL,
    Util::timeString(false, uiTime),
    this->parent()->gpsrFile().getReadChunksCount(GPSRFile::CHUNK_LOCATIONFIX),
    qPrintable(App::applicationLabel()), qPrintable(QCoreApplication::applicationVersion()), qPrintable(App::applicationUrl()),
    m_strLineColor.constData(),
    m_nLineWidth,
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

  if (!fix.hasFields(FIXFIELD_LATLONG | FIXFIELD_ALT)) //if (!fix.hasFields(FIXFIELD_TIME | FIXFIELD_LATLONG | FIXFIELD_ALT | FIXFIELD_TRACK | FIXFIELD_SPEED))
    return;

  fprintf(m_pFile,
    "%.6lf,%.6lf,%d\n",
    fix.getLongDeg(),
    fix.getLatDeg(),
    fix.iAlt);
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkKml::onEOF (void)
{
  QString strFilePath = m_strFilePath;

  this->close();

  if (m_bZipped)
  {
    // TODO
  }
}
