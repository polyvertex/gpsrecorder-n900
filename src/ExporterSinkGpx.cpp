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
    SIGNAL(sigSnappedPoint(const Exporter::SnappedPoint*)),
    SLOT(onSnappedPoint(const Exporter::SnappedPoint*)) );
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

  m_uiSnapCount = 1;
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
// onSnap
//---------------------------------------------------------------------------
void ExporterSinkGpx::onSnappedPoint (const Exporter::SnappedPoint* pSnappedPoint)
{
  QString strEle;
  QString strName(QString("Snap %1").arg(m_uiSnapCount++));

  if (pSnappedPoint->bHasAlt)
    strEle.sprintf(" <ele>%d</ele>" GPX_NL, pSnappedPoint->iAltM);

  if (!pSnappedPoint->strPointName.isEmpty())
  {
    strName += " : ";
    strName += pSnappedPoint->strPointName;
  }

  fprintf(m_pFile,
    "<trkpt lat=\"%.6lf\" lon=\"%.6lf\">" GPX_NL
    " <time>%s</time>" GPX_NL
    "%s"
    " <name>%s</name>" GPX_NL
    "</trkpt>" GPX_NL,
    pSnappedPoint->rLatDeg, pSnappedPoint->rLongDeg,
    Util::timeStringIso8601(true, pSnappedPoint->uiTime).constData(),
    qPrintable(strEle),
    qPrintable(strName) );
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkGpx::onEOF (void)
{
  this->close();
}
