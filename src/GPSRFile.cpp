//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-29 14:03:23
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// GPSRFile
//---------------------------------------------------------------------------
GPSRFile::GPSRFile (void)
{
  m_pFile = 0;
  m_bReading = false;
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
    fclose(m_pFile);
    m_pFile = 0;
  }
  m_bReading = false;
  m_strFilePath.clear();
}

//---------------------------------------------------------------------------
// openWrite
//---------------------------------------------------------------------------
bool GPSRFile::openWrite (const char* pszFile, bool bTruncate)
{
  if (this->isOpen())
  {
    if (!this->isReading())
      return true;
    this->close();
  }

  if (!bTruncate && Util::fileExists(pszFile))
    return false;

  m_pFile = fopen(pszFile, "wb");
  if (!m_pFile)
  {
    qWarning("Could not create file %s !", pszFile);
    return false;
  }

  m_strFilePath = pszFile;
  m_bReading    = false;

  // make it non-blocking
  /*{
    int nFlags = fcntl(fileno(m_pFile), F_GETFL, 0);
    nFlags |= O_NONBLOCK;
    fcntl(fileno(m_pFile), F_SETFL, nFlags);
  }*/

  // write header
  {
    Header* pHeader = (Header*)&m_Swap;

    pHeader->aucMagic[0] = 'G';
    pHeader->aucMagic[1] = 'P';
    pHeader->aucMagic[2] = 'S';
    pHeader->aucMagic[3] = 'R';
    pHeader->ucFormat    = FORMAT_VERSION;
    pHeader->uiTime      = time(0);

    this->writeData((char*)pHeader, sizeof(*pHeader));
  }

  return true;
}

//---------------------------------------------------------------------------
// openRead
//---------------------------------------------------------------------------
/*bool GPSRFile::openRead (const char* pszFile)
{
  if (this->isOpen())
  {
    if (this->isReading())
      return true;
    this->close();
  }

  // TODO
}*/



//---------------------------------------------------------------------------
// writeMessage
//---------------------------------------------------------------------------
void GPSRFile::writeMessage (time_t uiTime, const char* pszMessage)
{
  Chunk* pChunk = (Chunk*)&m_Swap;
  uint uiMsgLen;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
    return;

  Q_ASSERT(pszMessage);
  Q_ASSERT(pszMessage[0]);
  if (!pszMessage || !pszMessage[0])
    return;

  uiMsgSize = strlen(pszMessage) + 1; // +1 for the trailing '\0'
  if (uiMsgSize > this->maxChunkDataSize())
    uiMsgSize = this->maxChunkDataSize();

  pChunk->uiId   = CHUNK_MESSAGE;
  pChunk->uiSize = sizeof(Chunk) + uiMsgSize;
  pChunk->uiTime = uiTime ? uiTime : time(0);

  memcpy(pChunk->aData, pszMessage, uiMsgSize);
  pChunk->aData[uiMsgSize - 1] = 0;

  this->writeData((char*)&m_Swap, pChunk->uiSize);
}

//---------------------------------------------------------------------------
// writeLocationFix
//---------------------------------------------------------------------------
void GPSRFile::writeLocationFix (time_t uiTime, const LocationFix& fix)
{
  Chunk* pChunk = (Chunk*)&m_Swap;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
    return;

  Q_ASSERT(fix.uiStorageSize > 0);
  Q_ASSERT(fix.uiStorageSize < this->maxChunkDataSize());
  if (!fix.uiStorageSize || fix.uiStorageSize < this->maxChunkDataSize())
    return;

  pChunk->uiId   = CHUNK_LOCATIONFIX;
  pChunk->uiSize = sizeof(Chunk) + fix.uiStorageSize;
  pChunk->uiTime = uiTime ? uiTime : time(0);

  memcpy(pChunk->aData, fix.getStorageZone(), fix.uiStorageSize);

  this->writeData((char*)&m_Swap, pChunk->uiSize);
}

//---------------------------------------------------------------------------
// writeSnap
//---------------------------------------------------------------------------
void GPSRFile::writeSnap (time_t uiTime)
{
  Chunk* pChunk = (Chunk*)&m_Swap;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
    return;

  pChunk->uiId   = CHUNK_SNAP;
  pChunk->uiSize = sizeof(Chunk);
  pChunk->uiTime = uiTime ? uiTime : time(0);

  this->writeData((char*)&m_Swap, pChunk->uiSize);
}



//---------------------------------------------------------------------------
// writeData
//---------------------------------------------------------------------------
void GPSRFile::writeData (const char* pData, uint uiSize)
{
  uint uiWritten = 0;
  uint uiRes;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isReading());
  Q_ASSERT(pData);
  Q_ASSERT(uiSize > 0);
  if (!this->isOpen() || this->isReading() || !pData || !uiSize)
    return;

  while (uiWritten < uiSize)
  {
    clearerr(pFile);

    uiRes = fwrite(&pData[uiWritten], 1, (uiSize - uiWritten), pFile);
    if (uiRes < (uiSize - uiWritten) && ferror(pFile))
      qFatal("Failed to write %u bytes into file %s !", uiSize, m_strFile.constData());

    uiWritten += uiRes;
  }
}
