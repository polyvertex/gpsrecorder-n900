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

// color components are ABGR ordered, so for example,
// "FF00FF00" is green and "FF00FFFF" is yellow
#define KML_COLOR  "FF00FF00"



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
}

//---------------------------------------------------------------------------
// ~ExporterSinkKml
//---------------------------------------------------------------------------
ExporterSinkKml::~ExporterSinkKml (void)
{
  this->close();
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
      "    </coordinates>" KML_NL
      "  </LineString>" KML_NL
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
  m_strFilePath.sprintf("%s.%s", pszFilePath, this->extension());

  m_pFile = fopen(qPrintable(m_strFilePath), "wb");
  if (!m_pFile)
  {
    qWarning("Could not create %s ! Error %d : %s", qPrintable(m_strFilePath), errno, strerror(errno));
    return;
  }

  // write header
  fprintf(m_pFile,
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" KML_NL
    "<kml xmlns=\"http://earth.google.com/kml/2.0\">" KML_NL
    "<Placemark>" KML_NL
    "  <description>KML file created by %s v%s. Please visit %s for more info.</description>" KML_NL
    "  <name>Track %s</name>" KML_NL
    "  <visibility>1</visibility>" KML_NL
    "  <open>1</open>" KML_NL
    "  <Style>" KML_NL
    "    <LineStyle>" KML_NL
    "      <color>" KML_COLOR "</color>" KML_NL
    "      <width>2</width>" KML_NL
    "    </LineStyle>" KML_NL
    //"    <PolyStyle>" KML_NL
    //"      <color>7F00FF00</color>" KML_NL
    //"    </PolyStyle>" KML_NL
    "  </Style>" KML_NL
    "  <LineString>" KML_NL
    "    <tesselate>1</tesselate>" KML_NL
    "    <altitudeMode>relativeToGround</altitudeMode>" KML_NL
    "    <coordinates>" KML_NL,
    qPrintable(App::applicationLabel()), qPrintable(QCoreApplication::applicationVersion()), qPrintable(App::applicationUrl()),
    Util::timeString(false, uiTime) );

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

  if (!fix.hasFields(FIXFIELD_LATLONG)) //if (!fix.hasFields(FIXFIELD_TIME | FIXFIELD_LATLONG | FIXFIELD_ALT | FIXFIELD_TRACK | FIXFIELD_SPEED))
    return;

  fprintf(m_pFile, "%.6lf,%.6lf,1\n", fix.getLongDeg(), fix.getLatDeg());
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkKml::onEOF (void)
{
  this->close();
}
