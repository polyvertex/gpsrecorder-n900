//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-03 11:46:13
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Exporter
//---------------------------------------------------------------------------
Exporter::Exporter (void)
: QObject(0)
{
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadError(GPSRFile*, GPSRFile::Error)),
    SLOT(onReadError(GPSRFile*, GPSRFile::Error)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadSOF(GPSRFile*, time_t, quint8)),
    SLOT(onReadSOF(GPSRFile*, time_t, quint8)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkMessage(GPSRFile*, time_t, const char*, uint)),
    SLOT(onReadChunkMessage(GPSRFile*, time_t, const char*, uint)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkLocationFix(GPSRFile*, time_t, const LocationFix&)),
    SLOT(onReadChunkLocationFix(GPSRFile*, time_t, const LocationFix&)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkLocationFixLost(GPSRFile*, time_t)),
    SLOT(onReadChunkLocationFixLost(GPSRFile*, time_t)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkSnap(GPSRFile*, time_t)),
    SLOT(onReadChunkSnap(GPSRFile*, time_t)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkNamedSnap(GPSRFile*, time_t, const char*, uint)),
    SLOT(onReadChunkNamedSnap(GPSRFile*, time_t, const char*, uint)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkUnknown(GPSRFile*, GPSRFile::Chunk*)),
    SLOT(onReadChunkUnknown(GPSRFile*, GPSRFile::Chunk*)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadEOF(GPSRFile*)),
    SLOT(onReadEOF(GPSRFile*)) );
}

//---------------------------------------------------------------------------
// ~Exporter
//---------------------------------------------------------------------------
Exporter::~Exporter (void)
{
}



//---------------------------------------------------------------------------
// clear
//---------------------------------------------------------------------------
void Exporter::clear (void)
{
  m_strOutputBasePath.clear();
  m_GPSRFile.close();
  m_FixCont.reset();
  m_vecSnappedPoints.clear();
}



//---------------------------------------------------------------------------
// exportFile
//---------------------------------------------------------------------------
bool Exporter::exportFile (const QString& strFile)
{
  if (strFile.endsWith(".gpsr", Qt::CaseInsensitive))
    m_strOutputBasePath = strFile.left(strFile.length() - 5);
  else
    m_strOutputBasePath = strFile;

  if (!m_GPSRFile.openRead(qPrintable(strFile)))
    return false;

  // do not handle this gpsr file if there is no location fix
  // we need to be sure there is at least one location fix to handle 'snap'
  // chunks correctly
  if (m_GPSRFile.getReadChunksCount(GPSRFile::CHUNK_LOCATIONFIX) <= 0)
    return false;

  while (m_GPSRFile.readNext()) { ; }

  this->clear();

  return true;
}

//---------------------------------------------------------------------------
// exportFiles
//---------------------------------------------------------------------------
uint Exporter::exportFiles (const QStringList& filesList)
{
  uint uiSuccessCount = 0;

  for (int i = 0; i < filesList.size(); ++i)
  {
    if (this->exportFile(filesList[i]))
      ++uiSuccessCount;
  }

  return uiSuccessCount;
}

//---------------------------------------------------------------------------
// exportDir
//---------------------------------------------------------------------------
uint Exporter::exportDir (const QString& strDirectory, const QString& strSuffix/*=".gpsr"*/)
{
  if (!Util::fileIsDir(qPrintable(strDirectory)))
  {
    qWarning("Not a directory : %s !", qPrintable(strDirectory));
    return 0;
  }

  QDirIterator dirIt(strDirectory);
  QString strFile;
  uint uiSuccessCount = 0;

  while (dirIt.hasNext())
  {
    strFile = dirIt.next();
    if (!strFile.endsWith(strSuffix, Qt::CaseInsensitive))
      continue;

    if (this->exportFile(strFile))
      ++uiSuccessCount;
  }

  return uiSuccessCount;
}



//---------------------------------------------------------------------------
// onReadError
//---------------------------------------------------------------------------
void Exporter::onReadError (GPSRFile* pGPSRFile, GPSRFile::Error eError)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(eError);

  qWarning("GPSR read error %d !", eError);
}

//---------------------------------------------------------------------------
// onReadSOF
//---------------------------------------------------------------------------
void Exporter::onReadSOF (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiTime);
  Q_UNUSED(ucFormatVersion);

  emit this->sigSOF(qPrintable(m_strOutputBasePath), uiTime);
}

//---------------------------------------------------------------------------
// onReadChunkMessage
//---------------------------------------------------------------------------
void Exporter::onReadChunkMessage (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMessageLen)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiTime);
  Q_UNUSED(pszMessage);
  Q_UNUSED(uiMessageLen);

  // nothing to do for now...
}

//---------------------------------------------------------------------------
// onReadChunkLocationFix
//---------------------------------------------------------------------------
void Exporter::onReadChunkLocationFix (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix)
{
  Q_UNUSED(pGPSRFile);

  // emit snapped points if there is any
  for (int i = 0; i < m_vecSnappedPoints.size(); ++i)
    this->emitSnappedPoint(m_vecSnappedPoints[i], (m_FixCont.hasFix() ? m_FixCont.getFix() : 0), &fix);
  m_vecSnappedPoints.clear();

  // emit fix
  m_FixCont.setFix(fix);
  emit this->sigLocationFix(uiTime, m_FixCont);
}

//---------------------------------------------------------------------------
// onReadChunkLocationFixLost
//---------------------------------------------------------------------------
void Exporter::onReadChunkLocationFixLost (GPSRFile* pGPSRFile, time_t uiTime)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiTime);

  // nothing to do for now...
}

//---------------------------------------------------------------------------
// onReadChunkSnap
//---------------------------------------------------------------------------
void Exporter::onReadChunkSnap (GPSRFile* pGPSRFile, time_t uiTime)
{
  this->onReadChunkNamedSnap(pGPSRFile, uiTime, "", 0);
}

//---------------------------------------------------------------------------
// onReadChunkNamedSnap
//---------------------------------------------------------------------------
void Exporter::onReadChunkNamedSnap (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPointName, uint uiPointNameLen)
{
  SnappedPoint* pSnappedPoint;

  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiPointNameLen);

  m_vecSnappedPoints.append(SnappedPoint());
  pSnappedPoint               = &m_vecSnappedPoints.last();
  pSnappedPoint->uiTime       = uiTime;
  pSnappedPoint->strPointName = pszPointName ? pszPointName : "";
  pSnappedPoint->rLatDeg      = 0.0;
  pSnappedPoint->rLongDeg     = 0.0;
  pSnappedPoint->bHasAlt      = false;
  pSnappedPoint->iAltM        = 0;
}

//---------------------------------------------------------------------------
// onReadChunkUnknown
//---------------------------------------------------------------------------
void Exporter::onReadChunkUnknown (GPSRFile* pGPSRFile, GPSRFile::Chunk* pChunk)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(pChunk);

  // nothing to do for now...
}

//---------------------------------------------------------------------------
// onReadEOF
//---------------------------------------------------------------------------
void Exporter::onReadEOF (GPSRFile* pGPSRFile)
{
  Q_UNUSED(pGPSRFile);

  // emit remaining snapped points if there is any
  for (int i = 0; i < m_vecSnappedPoints.size(); ++i)
    this->emitSnappedPoint(m_vecSnappedPoints[i], (m_FixCont.hasFix() ? m_FixCont.getFix() : 0), 0);
  m_vecSnappedPoints.clear();

  // emit eof
  emit this->sigEOF();
}



//---------------------------------------------------------------------------
// emitSnappedPoint
//---------------------------------------------------------------------------
void Exporter::emitSnappedPoint (SnappedPoint& snapPt, const LocationFix* pFixA, const LocationFix* pFixB)
{
  Q_ASSERT(pFixA || pFixB);

  if (!pFixA && !pFixB)
  {
    return;
  }
  else if (!pFixA || !pFixB)
  {
    // here, we cannot interpolate point position because one of bounding
    // fixes is missing. thus, the only thing we can do is to take the
    // position of the only one bounding fix we have...

    const LocationFix* pFix = pFixA ? pFixA : pFixB;

    // fix position is normally garanteed to be setup by LocationMaemo but
    // here, we can't really know who created this gpsr file :)
    Q_ASSERT(pFix->hasFields(FIXFIELD_LATLONG));
    if (!pFix->hasFields(FIXFIELD_LATLONG))
      return;

    snapPt.rLatDeg  = pFix->getLatDeg();
    snapPt.rLongDeg = pFix->getLongDeg();
    snapPt.bHasAlt  = pFix->hasFields(FIXFIELD_ALT);
    snapPt.iAltM    = snapPt.bHasAlt ? pFix->iAlt : 0;

    emit this->sigSnappedPoint(&snapPt);
  }
  else
  {
    double rTimePos;

    // compute normalized time position between A and B
    if (pFixA->hasFields(FIXFIELD_TIME) && pFixB->hasFields(FIXFIELD_TIME) &&
        snapPt.uiTime >= pFixA->uiTime && snapPt.uiTime <= pFixB->uiTime)
    {
      rTimePos = double(snapPt.uiTime - pFixA->uiTime) / double(pFixB->uiTime - pFixA->uiTime);
    }
    else
    {
      // here, dumbly consider snapped point is in the middle between A and B
      rTimePos = 0.5;
    }

    // interpolate position
    snapPt.rLatDeg  = pFixA->getLatDeg() + ((pFixB->getLatDeg() - pFixA->getLatDeg()) * rTimePos);
    snapPt.rLongDeg = pFixA->getLongDeg() + ((pFixB->getLongDeg() - pFixA->getLongDeg()) * rTimePos);

    // interpolate altitude if we can
    if (pFixA->hasFields(FIXFIELD_ALT) && pFixB->hasFields(FIXFIELD_ALT))
    {
      double rAltM = double(pFixA->iAlt) + (double(pFixB->iAlt - pFixA->iAlt) * rTimePos);

      snapPt.bHasAlt = true;
      snapPt.iAltM   = qint32(rAltM);
    }
    else if (pFixA->hasFields(FIXFIELD_ALT))
    {
      snapPt.bHasAlt = true;
      snapPt.iAltM   = pFixA->iAlt;
    }
    else if (pFixB->hasFields(FIXFIELD_ALT))
    {
      snapPt.bHasAlt = true;
      snapPt.iAltM   = pFixB->iAlt;
    }
    else
    {
      snapPt.bHasAlt = false;
      snapPt.iAltM   = 0;
    }

    // emit snapped point
    emit this->sigSnappedPoint(&snapPt);
  }
}
