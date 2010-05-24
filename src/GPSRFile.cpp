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

  m_bWriting    = false;
  m_bError      = false;
  m_bEOF        = false;
  m_bIncomplete = false;

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

  m_bWriting    = false;
  m_bError      = false;
  m_bEOF        = false;
  m_bIncomplete = false;

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

    pHeader->aucMagic[0]     = 'G';
    pHeader->aucMagic[1]     = 'P';
    pHeader->aucMagic[2]     = 'S';
    pHeader->aucMagic[3]     = 'R';
    pHeader->ucFormat        = FORMAT_VERSION;
    pHeader->uiTime          = time(0);
    pHeader->iTimeZoneOffset = Util::timeZoneOffset();

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
  qDebug("Opened  GPSR file %s.", pszFile);

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

  pChunk->ucMagic = '@';
  pChunk->uiId    = CHUNK_MESSAGE;
  pChunk->uiSize  = uiChunkSize;
  pChunk->uiTime  = uiTime ? uiTime : time(0);

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

  pChunk->ucMagic = '@';
  pChunk->uiId    = CHUNK_LOCATIONFIX;
  pChunk->uiSize  = uiChunkSize;
  pChunk->uiTime  = uiTime ? uiTime : time(0);

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

  pChunk->ucMagic = '@';
  pChunk->uiId    = CHUNK_LOCATIONFIX_LOST;
  pChunk->uiSize  = uiChunkSize;
  pChunk->uiTime  = uiTime ? uiTime : time(0);

  this->writeData((char*)pChunk, uiChunkSize);
}

//---------------------------------------------------------------------------
// writeSnap
//---------------------------------------------------------------------------
#if 0 // obsolete
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

  pChunk->ucMagic = '@';
  pChunk->uiId    = CHUNK_SNAP;
  pChunk->uiSize  = uiChunkSize;
  pChunk->uiTime  = uiTime ? uiTime : time(0);

  this->writeData((char*)pChunk, uiChunkSize);
}
#endif

//---------------------------------------------------------------------------
// writeNamedSnap
//---------------------------------------------------------------------------
void GPSRFile::writeNamedSnap (time_t uiTime, const char* pszName)
{
  Chunk* pChunk;
  uint   uiChunkSize;
  uint   uiMsgSize;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(this->isWriting());
  if (!this->isOpen() || !this->isWriting())
    return;

  uiMsgSize   = (pszName ? strlen(pszName) : 0) + 1; // +1 for the trailing '\0'
  uiChunkSize = sizeof(Chunk) + uiMsgSize;

  m_Swap.reserve(uiChunkSize +1);
  pChunk = (Chunk*)m_Swap.data();

  pChunk->ucMagic = '@';
  pChunk->uiId    = CHUNK_NAMEDSNAP;
  pChunk->uiSize  = uiChunkSize;
  pChunk->uiTime  = uiTime ? uiTime : time(0);

  memcpy(pChunk->aData, pszName, uiMsgSize);
  pChunk->aData[uiMsgSize - 1] = 0;

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
  if (!this->readSize((char*)pHeader, sizeof(*pHeader), true, &m_bEOF))
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
    emit sigReadSOF(this, pHeader->uiTime, pHeader->ucFormat, pHeader->iTimeZoneOffset);

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
        if (m_bIncomplete)
          this->signalReadError(ERROR_TRUNCATED);
        emit sigReadEOF(this);
      }
      return false;
    }

    if (fseek(m_pFile, (int)m_vecReadChunks[nChunkIndex].uiOffset, SEEK_SET) < 0)
    {
      this->signalReadError(ERROR_READ);
      return false;
    }
    nChunkOffset = (int)ftell(m_pFile);

    m_nReadIndex = nChunkIndex;
  }

  // read chunk header
  if (!this->readSize((char*)&chunkHeader, sizeof(chunkHeader), false, &m_bEOF))
    return false;
  if (chunkHeader.ucMagic != '@')
  {
    this->signalReadError(ERROR_FORMAT);
    return false;
  }
  m_Swap.reserve(chunkHeader.uiSize +1);
  pChunk = (Chunk*)m_Swap.data();
  memcpy(pChunk, &chunkHeader, sizeof(chunkHeader));

  // read chunk data if needed
  if (chunkHeader.uiSize > sizeof(chunkHeader))
  {
    if (!this->readSize((char*)pChunk + sizeof(chunkHeader), pChunk->uiSize - sizeof(chunkHeader), false, &m_bEOF))
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
      case CHUNK_NAMEDSNAP :
        emit sigReadChunkNamedSnap(this, pChunk->uiTime, (char*)&(pChunk->aData), pChunk->uiSize - sizeof(*pChunk) - 1);
        break;
      default :
        qWarning("Unknown chunk #%u @ offset %d in file %s !", (uint)pChunk->uiId, nChunkOffset, m_strFilePath.constData());
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
bool GPSRFile::readSize (char* pOutData, uint uiExpectedSize, bool bIsFileHeader, bool* pbGotEOF)
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
          m_bIncomplete = true;
          qWarning("Incomplete GPSR file %s !", m_strFilePath.constData());

          if (bIsFileHeader)
          {
            // we should never get here in non-discovery mode since the
            // purpose of the discovery mode is to have a first reading pass
            // to count available and complete chunks from the file.
            // thus, readChunk() will never try to read a chunk with an
            // index greater or equal than count of discovered chunks.
            Q_ASSERT(m_bDiscoveryRead == true);
            this->signalReadError(ERROR_TRUNCATED);
          }
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
