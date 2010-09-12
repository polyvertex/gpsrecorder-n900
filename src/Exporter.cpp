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
  this->clear();

  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadError(GPSRFile*, GPSRFile::Error)),
    SLOT(onReadError(GPSRFile*, GPSRFile::Error)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadSOF(GPSRFile*, time_t, quint8, qint32)),
    SLOT(onReadSOF(GPSRFile*, time_t, quint8, qint32)) );
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
    SIGNAL(sigReadChunkPaused(GPSRFile*, time_t, const char*, uint)),
    SLOT(onReadChunkPaused(GPSRFile*, time_t, const char*, uint)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkResumed(GPSRFile*, time_t)),
    SLOT(onReadChunkResumed(GPSRFile*, time_t)) );
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
  m_vecGizmoPoints.clear();
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
void Exporter::onReadSOF (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion, qint32 iTimeZoneOffset)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiTime);
  Q_UNUSED(ucFormatVersion);

  emit this->sigSOF(qPrintable(m_strOutputBasePath), uiTime, iTimeZoneOffset);
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

  // emit gizmo points
  for (int i = 0; i < m_vecGizmoPoints.size(); ++i)
    this->emitGizmoPoint(m_vecGizmoPoints[i], (m_FixCont.hasFix() ? m_FixCont.getFix() : 0), &fix);
  m_vecGizmoPoints.clear();

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
  GizmoPoint* pGizmoPoint;

  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiPointNameLen);

  m_vecGizmoPoints.append(GizmoPoint());
  pGizmoPoint               = &m_vecGizmoPoints.last();
  pGizmoPoint->uiTime       = uiTime;
  pGizmoPoint->eType        = GIZMO_SNAP;
  pGizmoPoint->strPointName = pszPointName ? pszPointName : "";
  pGizmoPoint->rLatDeg      = 0.0;
  pGizmoPoint->rLongDeg     = 0.0;
  pGizmoPoint->bHasAlt      = false;
  pGizmoPoint->iAltM        = 0;
}

//---------------------------------------------------------------------------
// onReadChunkPaused
//---------------------------------------------------------------------------
void Exporter::onReadChunkPaused (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPauseName, uint uiPauseNameLen)
{
  // TODO
}

//---------------------------------------------------------------------------
// onReadChunkResumed
//---------------------------------------------------------------------------
void Exporter::onReadChunkResumed (GPSRFile* pGPSRFile, time_t uiTime)
{
  // TODO
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

  // emit remaining gizmo points
  for (int i = 0; i < m_vecGizmoPoints.size(); ++i)
    this->emitGizmoPoint(m_vecGizmoPoints[i], (m_FixCont.hasFix() ? m_FixCont.getFix() : 0), 0);
  m_vecGizmoPoints.clear();

  // emit eof
  emit this->sigEOF();
}



//---------------------------------------------------------------------------
// emitGizmoPoint
//---------------------------------------------------------------------------
void Exporter::emitGizmoPoint (GizmoPoint& gizmoPt, const LocationFix* pFixA, const LocationFix* pFixB)
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

    gizmoPt.rLatDeg  = pFix->getLatDeg();
    gizmoPt.rLongDeg = pFix->getLongDeg();
    gizmoPt.bHasAlt  = pFix->hasFields(FIXFIELD_ALT);
    gizmoPt.iAltM    = gizmoPt.bHasAlt ? pFix->iAlt : 0;

    emit this->sigGizmoPoint(&gizmoPt);
  }
  else
  {
    double rTimePos;

    // compute normalized time position between A and B
    if (pFixA->hasFields(FIXFIELD_TIME) && pFixB->hasFields(FIXFIELD_TIME) &&
        gizmoPt.uiTime >= pFixA->uiTime && gizmoPt.uiTime <= pFixB->uiTime)
    {
      rTimePos = double(gizmoPt.uiTime - pFixA->uiTime) / double(pFixB->uiTime - pFixA->uiTime);
    }
    else
    {
      // here, dumbly consider snapped point is in the middle between A and B
      rTimePos = 0.5;
    }

    // interpolate position
    gizmoPt.rLatDeg  = pFixA->getLatDeg() + ((pFixB->getLatDeg() - pFixA->getLatDeg()) * rTimePos);
    gizmoPt.rLongDeg = pFixA->getLongDeg() + ((pFixB->getLongDeg() - pFixA->getLongDeg()) * rTimePos);

    // interpolate altitude if we can
    if (pFixA->hasFields(FIXFIELD_ALT) && pFixB->hasFields(FIXFIELD_ALT))
    {
      double rAltM = double(pFixA->iAlt) + (double(pFixB->iAlt - pFixA->iAlt) * rTimePos);

      gizmoPt.bHasAlt = true;
      gizmoPt.iAltM   = qint32(rAltM);
    }
    else if (pFixA->hasFields(FIXFIELD_ALT))
    {
      gizmoPt.bHasAlt = true;
      gizmoPt.iAltM   = pFixA->iAlt;
    }
    else if (pFixB->hasFields(FIXFIELD_ALT))
    {
      gizmoPt.bHasAlt = true;
      gizmoPt.iAltM   = pFixB->iAlt;
    }
    else
    {
      gizmoPt.bHasAlt = false;
      gizmoPt.iAltM   = 0;
    }

    // emit gizmo point
    emit this->sigGizmoPoint(&gizmoPt);
  }
}
