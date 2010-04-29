//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-29 14:03:23
//
// $Id$
//
//***************************************************************************

#include "stable.h"

#if Q_BYTE_ORDER != Q_LITTLE_ENDIAN
#error GPSRFile class does not support BIG endian platforms yet !
#endif


//---------------------------------------------------------------------------
// GPSRFile
//---------------------------------------------------------------------------
GPSRFile::GPSRFile (void)
{
  m_pFile = 0;

  m_bWriting = false;
  m_bError   = false;
  m_bEOF     = false;

  m_bDiscoveryRead = false;
  m_nReadIndex     = -1;
}

//---------------------------------------------------------------------------
// ~GPSRFile
//---------------------------------------------------------------------------
GPSRFile::~GPSRFile (void)
{
  this->close();
}



//---------------------------------------------------------------------------
// close
//---------------------------------------------------------------------------
void GPSRFile::close (void)
{
  if (m_pFile)
  {
    qDebug("Closing GPSR file %s", m_strFilePath.constData());

    fclose(m_pFile);
    m_pFile = 0;
  }

  m_strFilePath.clear();

  m_bWriting = false;
  m_bError   = false;
  m_bEOF     = false;

  m_bDiscoveryRead = false;
  m_nReadIndex     = -1;
  m_vecReadChunks.clear();
}

//---------------------------------------------------------------------------
// openWrite
//---------------------------------------------------------------------------
bool GPSRFile::openWrite (const char* pszFile, bool bTruncate)
{
  if (this->isOpen())
  {
    if (this->isWriting())
      return true;
    this->close();
  }

  if (!bTruncate && Util::fileExists(pszFile))
  {
    m_bError = true;
    return false;
  }

  m_pFile = fopen(pszFile, "wb");
  if (!m_pFile)
  {
    m_bError = true;
    qWarning("Could not create %s ! Error %d : %s", pszFile, errno, strerror(errno));
    return false;
  }

  m_strFilePath = pszFile;
  m_bWriting    = true;
  m_bError      = false;

  // write header
  {
    Header* pHeader;

    m_Swap.reserve(sizeof(*pHeader) +1);
    pHeader = (Header*)m_Swap.data();

    pHeader->aucMagic[0] = 'G';
    pHeader->aucMagic[1] = 'P';
    pHeader->aucMagic[2] = 'S';
    pHeader->aucMagic[3] = 'R';
    pHeader->ucFormat    = FORMAT_VERSION;
    pHeader->uiTime      = time(0);

    this->writeData((char*)pHeader, sizeof(*pHeader));
  }

  qDebug("Created GPSR file %s", pszFile);

  return true;
}

//---------------------------------------------------------------------------
// openRead
//---------------------------------------------------------------------------
bool GPSRFile::openRead (const char* pszFile)
{
  if (this->isOpen())
  {
    if (!this->isWriting())
      return true;
    this->close();
  }

  // open files
  m_pFile = fopen(pszFile, "rb");
  if (!m_pFile)
  {
    m_bError = true;
    qWarning("Could not open %s ! Error %d : %s", pszFile, errno, strerror(errno));
    return false;
  }

  // setup members
  m_strFilePath = pszFile;
  m_bWriting    = false;
  m_bError      = false;
  m_bEOF        = false;

  // first, silently read the entire file in one pass to discover its content
  // and to give us the ability to navigate easily through chunks
  {
    bool bRes;

    m_bDiscoveryRead = true;
    m_nReadIndex     = -1;
    m_vecReadChunks.clear();

    if (!this->seekFirst())
      return false;

    while (bRes = this->readNext()) { ; }
    if (!bRes && m_bError)
      return false;

    m_bDiscoveryRead = false;
  }

  qDebug("Opened  GPSR file %s (%d chunks).", pszFile, m_vecReadChunks.count());

  return this->seekFirst();
}



//---------------------------------------------------------------------------
// writeMessage
//---------------------------------------------------------------------------
void GPSRFile::writeMessage (time_t uiTime, const char* pszMessage)
{
  Chunk* pChunk;
  uint   uiChunkSize;
  uint   uiMsgSize;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(this->isWriting());
  if (!this->isOpen() || !this->isWriting())
    return;

  Q_ASSERT(pszMessage);
  Q_ASSERT(pszMessage[0]);
  if (!pszMessage || !pszMessage[0])
    return;

  uiMsgSize   = strlen(pszMessage) + 1; // +1 for the trailing '\0'
  uiChunkSize = sizeof(Chunk) + uiMsgSize;

  m_Swap.reserve(uiChunkSize +1);
  pChunk = (Chunk*)m_Swap.data();

  pChunk->aucMagic = '@';
  pChunk->uiId     = CHUNK_MESSAGE;
  pChunk->uiSize   = uiChunkSize;
  pChunk->uiTime   = uiTime ? uiTime : time(0);

  memcpy(pChunk->aData, pszMessage, uiMsgSize);
  pChunk->aData[uiMsgSize - 1] = 0;

  this->writeData((char*)pChunk, uiChunkSize);
}

//---------------------------------------------------------------------------
// writeLocationFix
//---------------------------------------------------------------------------
void GPSRFile::writeLocationFix (time_t uiTime, const LocationFixContainer& fixCont)
{
  Chunk* pChunk;
  uint   uiChunkSize;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(this->isWriting());
  if (!this->isOpen() || !this->isWriting())
    return;

  Q_ASSERT(fixCont.hasFix());
  if (!fixCont.hasFix())
    return;

  uiChunkSize = sizeof(Chunk) + fixCont.getFixSize();

  m_Swap.reserve(uiChunkSize +1);
  pChunk = (Chunk*)m_Swap.data();

  pChunk->aucMagic = '@';
  pChunk->uiId     = CHUNK_LOCATIONFIX;
  pChunk->uiSize   = uiChunkSize;
  pChunk->uiTime   = uiTime ? uiTime : time(0);

  memcpy(pChunk->aData, fixCont.getFix(), fixCont.getFixSize());

  this->writeData((char*)pChunk, uiChunkSize);
}

//---------------------------------------------------------------------------
// writeLocationFixLost
//---------------------------------------------------------------------------
void GPSRFile::writeLocationFixLost (time_t uiTime)
{
  Chunk* pChunk;
  uint   uiChunkSize;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(this->isWriting());
  if (!this->isOpen() || !this->isWriting())
    return;

  uiChunkSize = sizeof(Chunk);

  m_Swap.reserve(uiChunkSize +1);
  pChunk = (Chunk*)m_Swap.data();

  pChunk->aucMagic = '@';
  pChunk->uiId     = CHUNK_LOCATIONFIX_LOST;
  pChunk->uiSize   = uiChunkSize;
  pChunk->uiTime   = uiTime ? uiTime : time(0);

  this->writeData((char*)pChunk, uiChunkSize);
}

//---------------------------------------------------------------------------
// writeSnap
//---------------------------------------------------------------------------
void GPSRFile::writeSnap (time_t uiTime)
{
  Chunk* pChunk;
  uint   uiChunkSize;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(this->isWriting());
  if (!this->isOpen() || !this->isWriting())
    return;

  uiChunkSize = sizeof(Chunk);

  m_Swap.reserve(uiChunkSize +1);
  pChunk = (Chunk*)m_Swap.data();

  pChunk->aucMagic = '@';
  pChunk->uiId     = CHUNK_SNAP;
  pChunk->uiSize   = uiChunkSize;
  pChunk->uiTime   = uiTime ? uiTime : time(0);

  this->writeData((char*)pChunk, uiChunkSize);
}



//---------------------------------------------------------------------------
// seekFirst
//---------------------------------------------------------------------------
bool GPSRFile::seekFirst (void)
{
  Header* pHeader;

  if (m_bError)
    return false;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isWriting());
  if (!this->isOpen() || this->isWriting())
  {
    this->signalReadError(ERROR_NOTOPEN);
    return false;
  }

  // seek to start
  rewind(m_pFile);
  if (ferror(m_pFile) || feof(m_pFile))
  {
    if (feof(m_pFile))
      m_bEOF = true;
    this->signalReadError(ERROR_READ);
    return false;
  }
  m_bEOF = false;
  m_nReadIndex = -1;

  // prepare swap buffer
  m_Swap.reserve(sizeof(*pHeader) +1);
  pHeader = (Header*)m_Swap.data();

  // read file header
  if (!readSize((char*)pHeader, sizeof(*pHeader), &m_bEOF))
    return false;

  // validate header
  {
    if (pHeader->aucMagic[0] != 'G' ||
        pHeader->aucMagic[1] != 'P' ||
        pHeader->aucMagic[2] != 'S' ||
        pHeader->aucMagic[3] != 'R')
    {
      this->signalReadError(ERROR_FORMAT);
      return false;
    }

    if (pHeader->ucFormat != FORMAT_VERSION)
    {
      this->signalReadError(ERROR_FORMAT_VERSION);
      return false;
    }
  }

  if (!m_bDiscoveryRead)
    emit sigReadSOF(this, pHeader->uiTime, pHeader->ucFormat);

  return true;
}

//---------------------------------------------------------------------------
// readNext
//---------------------------------------------------------------------------
bool GPSRFile::readNext (void)
{
  return this->readChunk(m_nReadIndex + 1);
}

//---------------------------------------------------------------------------
// readPrevious
//---------------------------------------------------------------------------
bool GPSRFile::readPrevious (void)
{
  Q_ASSERT(m_nReadIndex >= 1);
  if (m_nReadIndex < 1)
    return false;

  return this->readChunk(m_nReadIndex - 1);
}

//---------------------------------------------------------------------------
// readChunk
//---------------------------------------------------------------------------
bool GPSRFile::readChunk (int nChunkIndex)
{
  Chunk  chunkHeader;
  Chunk* pChunk;
  int    nChunkOffset;

  if (m_bError)
    return false;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isWriting());
  if (!this->isOpen() || this->isWriting())
  {
    this->signalReadError(ERROR_NOTOPEN);
    return false;
  }

  if (m_bDiscoveryRead)
  {
    // behave like a 'next' action

    // eof already reached ?
    if (m_bEOF)
      return false;

    Q_ASSERT(nChunkIndex == m_nReadIndex + 1);
    ++m_nReadIndex;

    nChunkOffset = (int)ftell(m_pFile);
  }
  else
  {
    Q_ASSERT(m_bDiscoveryRead == false);
    Q_ASSERT(nChunkIndex >= 0);
    if (nChunkIndex < 0 || nChunkIndex >= m_vecReadChunks.count())
    {
      if (!m_bEOF && nChunkIndex >= m_vecReadChunks.count())
      {
        m_bEOF = true;
        emit sigReadEOF(this);
      }
      return false;
    }

    if (fseek(m_pFile, (int)m_vecReadChunks[nChunkIndex].uiOffset, SEEK_SET) < 0)
    {
      this->signalReadError(ERROR_READ);
      return false;
    }

    m_nReadIndex = nChunkIndex;
  }

  // read chunk
  if (!this->readSize((char*)&chunkHeader, sizeof(chunkHeader), &m_bEOF))
    return false;
  if (chunkHeader.aucMagic != '@')
  {
    this->signalReadError(ERROR_FORMAT);
    return false;
  }
  if (chunkHeader.uiSize > sizeof(*pChunk))
  {
    m_Swap.reserve(chunkHeader.uiSize +1);
    pChunk = (Chunk*)m_Swap.data();
    memcpy(pChunk, &chunkHeader, sizeof(chunkHeader));

    if (!this->readSize((char*)pChunk + sizeof(chunkHeader), pChunk->uiSize - sizeof(chunkHeader), &m_bEOF))
      return false;
  }

  // discovery mode ?
  if (m_bDiscoveryRead)
  {
    Q_ASSERT(m_nReadIndex == m_vecReadChunks.count());
    Q_ASSERT(nChunkOffset > 0);

    if (m_nReadIndex == m_vecReadChunks.count() && nChunkOffset > 0)
    {
      ChunkReadInfo chunkReadInfo;

      chunkReadInfo.uiOffset = (uint)nChunkOffset;
      chunkReadInfo.uiId     = chunkHeader.uiId;
      chunkReadInfo.uiSize   = chunkHeader.uiSize;
      chunkReadInfo.uiTime   = chunkHeader.uiTime;

      m_vecReadChunks.push_back(chunkReadInfo);
    }
  }
  else
  {
    // signal this chunk
    switch (pChunk->uiId)
    {
      case CHUNK_MESSAGE :
        emit sigReadChunkMessage(this, pChunk->uiTime, (char*)&(pChunk->aData), pChunk->uiSize - sizeof(*pChunk) - 1);
        break;
      case CHUNK_LOCATIONFIX :
        emit sigReadChunkLocationFix(this, pChunk->uiTime, (LocationFix&)*(pChunk->aData));
        break;
      case CHUNK_LOCATIONFIX_LOST :
        emit sigReadChunkLocationFixLost(this, pChunk->uiTime);
        break;
      case CHUNK_SNAP :
        emit sigReadChunkSnap(this, pChunk->uiTime);
        break;
      default :
        emit sigReadChunkUnknown(this, pChunk);
        break;
    }
  }

  return true;
}



//---------------------------------------------------------------------------
// getReadChunksCount
//---------------------------------------------------------------------------
int GPSRFile::getReadChunksCount (quint16 uiChunkId) const
{
  int nCount = 0;

  for (int i = 0; i < m_vecReadChunks.count(); ++i)
  {
    if (m_vecReadChunks[i].uiId == uiChunkId)
      ++nCount;
  }

  return nCount;
}



//---------------------------------------------------------------------------
// writeData
//---------------------------------------------------------------------------
void GPSRFile::writeData (const char* pData, uint uiSize)
{
  uint uiWritten = 0;
  uint uiRes;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(this->isWriting());
  if (!this->isOpen() || !this->isWriting())
    return;

  Q_ASSERT(pData);
  Q_ASSERT(uiSize > 0);
  if (!pData || !uiSize)
    return;

  while (uiWritten < uiSize)
  {
    clearerr(m_pFile);

    uiRes = fwrite(pData + uiWritten, 1, (uiSize - uiWritten), m_pFile);
    if (uiRes < (uiSize - uiWritten) && ferror(m_pFile))
      qFatal("Failed to write %u bytes into %s !", uiSize, m_strFilePath.constData());

    uiWritten += uiRes;
  }
}

//---------------------------------------------------------------------------
// readSize
//---------------------------------------------------------------------------
bool GPSRFile::readSize (char* pOutData, uint uiExpectedSize, bool* pbGotEOF)
{
  uint uiRead = 0;
  uint uiRes;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isWriting());
  if (!this->isOpen() || this->isWriting())
  {
    this->signalReadError(ERROR_NOTOPEN);
    return false;
  }

  Q_ASSERT(pOutData);
  Q_ASSERT(uiExpectedSize > 0);
  Q_ASSERT(pbGotEOF);
  if (!pOutData || !uiExpectedSize)
    qFatal("GPSRFile::readSize() : invalid parameters !");

  while (uiRead < uiExpectedSize)
  {
    clearerr(m_pFile);

    uiRes = fread(pOutData + uiRead, 1, (uiExpectedSize - uiRead), m_pFile);
    if (uiRes < (uiExpectedSize - uiRead))
    {
      if (feof(m_pFile))
      {
        if (pbGotEOF)
          *pbGotEOF = true;

        if (uiRes == 0 && uiRead == 0)
        {
          if (!m_bDiscoveryRead)
            emit sigReadEOF(this);
        }
        else
        {
          this->signalReadError(ERROR_TRUNCATED);
        }

        return false;
      }
      else
      {
        this->signalReadError(ERROR_READ);
        return false;
      }
    }
    else
    {
      uiRead += uiRes;
    }
  }

  return true;
}

//---------------------------------------------------------------------------
// signalReadError
//---------------------------------------------------------------------------
void GPSRFile::signalReadError (Error eError)
{
  m_bError = true;
  emit sigReadError(this, eError);
}
