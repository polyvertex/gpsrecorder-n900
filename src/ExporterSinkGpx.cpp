//***************************************************************************
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
#define GPX_NL  "\n"



//---------------------------------------------------------------------------
// ExporterSinkGpx
//---------------------------------------------------------------------------
ExporterSinkGpx::ExporterSinkGpx (Exporter* pParent)
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
// ~ExporterSinkGpx
//---------------------------------------------------------------------------
ExporterSinkGpx::~ExporterSinkGpx (void)
{
  this->close();
}



//---------------------------------------------------------------------------
// close
//---------------------------------------------------------------------------
void ExporterSinkGpx::close (void)
{
  // write footer
  if (m_pFile)
  {
    fprintf(m_pFile,
      "</trkseg>" GPX_NL
      "</trk>" GPX_NL
      "</gpx>" GPX_NL );
  }

  ExporterSink::close();
}



//---------------------------------------------------------------------------
// onSOF
//---------------------------------------------------------------------------
void ExporterSinkGpx::onSOF (const char* pszFilePath, time_t uiTime)
{
  Q_UNUSED(uiTime);

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
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" GPX_NL
    "<gpx creator=\"%s v%s\" xmlns=\"http://www.topografix.com/GPX/1/0\">" GPX_NL
    "<trk>" GPX_NL
    "<trkseg>" GPX_NL,
    qPrintable(App::applicationLabel()), qPrintable(QCoreApplication::applicationVersion()) );
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void ExporterSinkGpx::onLocationFix (time_t uiTime, const LocationFixContainer& fixCont)
{
  Q_UNUSED(uiTime);

  Q_ASSERT(fixCont.hasFix());
  if (!m_pFile || !fixCont.hasFix())
    return;

  const LocationFix& fix = *fixCont.getFix();

  if (!fix.hasFields(FIXFIELD_TIME | FIXFIELD_LATLONG))
    return;

  QDateTime dt;
  dt.setTime_t(uiTime);

  fprintf(m_pFile,
    " <trkpt lat=\"%.6lf\" lon=\"%.6lf\">"
    "<time>%s</time>"
    "</trkpt>" GPX_NL,
    fix.getLatDeg(), fix.getLongDeg(),
    qPrintable(dt.toString(Qt::ISODate)) );
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkGpx::onEOF (void)
{
  this->close();
}
