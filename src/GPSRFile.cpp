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

  m_strFilePath.clear();

  m_bReading = false;
  m_bReadEOF = false;
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
    qWarning("Could not create file %s ! Error %d : %s", pszFile, errno, strerror(errno));
    return false;
  }

  m_strFilePath = pszFile;
  m_bReading    = false;
  m_bReadEOF    = false;

  // make it non-blocking
  /*{
    int nFlags = fcntl(fileno(m_pFile), F_GETFL, 0);
    nFlags |= O_NONBLOCK;
    fcntl(fileno(m_pFile), F_SETFL, nFlags);
  }*/

  // write header
  {
    Header* pHeader;

    m_Swap.reserve(sizeof(*pHeader));
    pHeader = (Header*)m_Swap.data();

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
bool GPSRFile::openRead (const char* pszFile)
{
  if (this->isOpen())
  {
    if (this->isReading())
      return true;
    this->close();
  }

  m_pFile = fopen(pszFile, "rb");
  if (!m_pFile)
  {
    qWarning("Could not open file %s ! Error %d : %s", pszFile, errno, strerror(errno));
    return false;
  }

  m_strFilePath = pszFile;
  m_bReading    = true;
  m_bReadEOF    = false;

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
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
    return;

  Q_ASSERT(pszMessage);
  Q_ASSERT(pszMessage[0]);
  if (!pszMessage || !pszMessage[0])
    return;

  uiMsgSize   = strlen(pszMessage) + 1; // +1 for the trailing '\0'
  uiChunkSize = sizeof(Chunk) + uiMsgSize;

  m_Swap.reserve(uiChunkSize);
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
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
    return;

  Q_ASSERT(fixCont.hasFix());
  if (!fixCont.hasFix())
    return;

  uiChunkSize = sizeof(Chunk) + fixCont.getFixSize();

  m_Swap.reserve(uiChunkSize);
  pChunk = (Chunk*)m_Swap.data();

  pChunk->aucMagic = '@';
  pChunk->uiId     = CHUNK_LOCATIONFIX;
  pChunk->uiSize   = uiChunkSize;
  pChunk->uiTime   = uiTime ? uiTime : time(0);

  memcpy(pChunk->aData, fixCont.getFix(), fixCont.getFixSize());

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
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
    return;

  uiChunkSize = sizeof(Chunk);

  m_Swap.reserve(uiChunkSize);
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

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
  {
    emit sigReadError(this, ERROR_NOTOPEN);
    return false;
  }

  // seek to start
  rewind(m_pFile);
  if (ferror(m_pFile) || feof(m_pFile))
  {
    if (feof(m_pFile))
      m_bReadEOF = true;
    emit sigReadError(this, ERROR_READ);
    return false;
  }
  m_bReadEOF = false;

  // prepare swap buffer
  m_Swap.reserve(sizeof(*pHeader));
  pHeader = (Header*)m_Swap.data();

  // read file header
  if (!readSize((char*)pHeader, sizeof(*pHeader), &m_bReadEOF))
    return false;

  // validate header
  {
    if (pHeader->aucMagic[0] != 'G' ||
        pHeader->aucMagic[1] != 'P' ||
        pHeader->aucMagic[2] != 'S' ||
        pHeader->aucMagic[3] != 'R' )
    {
      emit sigReadError(this, ERROR_FORMAT);
      return false;
    }

    if (pHeader->ucFormat != FORMAT_VERSION)
    {
      emit sigReadError(this, ERROR_FORMAT_VERSION);
      return false;
    }
  }

  emit sigReadSOF(this, pHeader->uiTime, pHeader->ucFormat);

  return true;
}

//---------------------------------------------------------------------------
// readNext
//---------------------------------------------------------------------------
bool GPSRFile::readNext (void)
{
  Chunk* pChunk;

  Q_ASSERT(this->isOpen());
  Q_ASSERT(!this->isReading());
  if (!this->isOpen() || this->isReading())
  {
    emit sigReadError(this, ERROR_NOTOPEN);
    return false;
  }

  // eof already reached ?
  if (m_bReadEOF)
  {
    emit sigReadEOF(this);
    return false;
  }

  // prepare swap buffer
  m_Swap.reserve(sizeof(*pChunk));
  pChunk = (Chunk*)m_Swap.data();

  // read chunk
  if (!this->readSize((char*)pChunk, sizeof(*pChunk), &m_bReadEOF))
    return false;
  if (pChunk->aucMagic != '@')
  {
    emit sigReadError(this, ERROR_FORMAT);
    return false;
  }
  if (pChunk->uiSize > sizeof(*pChunk))
  {
    m_Swap.reserve(pChunk->uiSize);
    if (!this->readSize((char*)pChunk, pChunk->uiSize - sizeof(*pChunk), &m_bReadEOF))
      return false;
  }

  // signal this chunk
  switch (pChunk->uiId)
  {
    case CHUNK_MESSAGE :
      emit sigReadChunkMessage(this, pChunk->uiTime, (char*)&(pChunk->aData), pChunk->uiSize - sizeof(*pChunk) - 1);
      break;
    case CHUNK_LOCATIONFIX :
      emit sigReadChunkLocationFix(this, pChunk->uiTime, (LocationFix&)*(pChunk->aData));
      break;
    case CHUNK_SNAP :
      emit sigReadChunkSnap(this, pChunk->uiTime);
      break;
    default :
      emit sigReadChunkUnknown(this, pChunk);
      break;
  }

  return true;
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
  if (!this->isOpen() || this->isReading())
    return;

  Q_ASSERT(pData);
  Q_ASSERT(uiSize > 0);
  if (!pData || !uiSize)
    return;

  while (uiWritten < uiSize)
  {
    clearerr(m_pFile);

    uiRes = fwrite(&pData[uiWritten], 1, (uiSize - uiWritten), m_pFile);
    if (uiRes < (uiSize - uiWritten) && ferror(m_pFile))
      qFatal("Failed to write %u bytes into file %s !", uiSize, m_strFilePath.constData());

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
  Q_ASSERT(this->isReading());
  if (!this->isOpen() || !this->isReading())
  {
    emit sigReadError(this, ERROR_NOTOPEN);
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

    uiRes = fread(&pOutData[uiRead], 1, (uiExpectedSize - uiRead), m_pFile);
    if (uiRes < (uiExpectedSize - uiRead))
    {
      if (feof(m_pFile))
      {
        if (pbGotEOF)
          *pbGotEOF = true;

        emit sigReadError(this, ERROR_TRUNCATED);
        return false;
      }
      else
      {
        emit sigReadError(this, ERROR_READ);
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
