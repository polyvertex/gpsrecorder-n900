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
void Exporter::onReadChunkMessage (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMsgLen)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiTime);
  Q_UNUSED(pszMessage);
  Q_UNUSED(uiMsgLen);

  // nothing to do for now...
}

//---------------------------------------------------------------------------
// onReadChunkLocationFix
//---------------------------------------------------------------------------
void Exporter::onReadChunkLocationFix (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix)
{
  Q_UNUSED(pGPSRFile);

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
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(uiTime);

  // nothing to do for now...
}

//---------------------------------------------------------------------------
// onReadChunkUnknown
//---------------------------------------------------------------------------
void Exporter::onReadChunkUnknown (GPSRFile* pGPSRFile, GPSRFile::Chunk* pChunk)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(pChunk);

  // nothing to do for now...
  qDebug("Exported received UNKNOWN CHUNK %u !", (uint)pChunk->uiId);
}

//---------------------------------------------------------------------------
// onReadEOF
//---------------------------------------------------------------------------
void Exporter::onReadEOF (GPSRFile* pGPSRFile)
{
  Q_UNUSED(pGPSRFile);

  emit this->sigEOF();
}
