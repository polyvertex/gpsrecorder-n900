//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-29 14:03:17
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRFILE_H__
#define __GPSRFILE_H__

#include "stable.h"


//---------------------------------------------------------------------------
// GPSRFile
//---------------------------------------------------------------------------
class GPSRFile : public QObject
{
  Q_OBJECT

public :
  enum
  {
    FORMAT_VERSION = 0x00,

    CHUNK_MESSAGE          = 10, // asciiz message (usually a log message)
    CHUNK_LOCATIONFIX      = 20, // LocationFix structure (only its storage zone)
    CHUNK_LOCATIONFIX_LOST = 21, // Lost GPS fix
    CHUNK_SNAP             = 30, // a 'snap' event (user pushed the 'snap' button while recording)
  };

  enum Error
  {
    ERROR_NOTOPEN        = 1, // file is not open or is not in reading mode
    ERROR_READ           = 2, // system error occurred while reading file
    ERROR_FORMAT         = 3, // file or chunk header does not match
    ERROR_FORMAT_VERSION = 4, // format version does not match
    ERROR_TRUNCATED      = 5, // file is not complete
  };

  struct Header
  {
    quint8  aucMagic[4]; // 'G','P','S','R'
    quint8  ucFormat;    // format version
    quint32 uiTime;      // start time (posix timestamp, utc)
  }
  __attribute__((packed));

  struct Chunk
  {
    quint8  aucMagic; // '@'
    quint16 uiId;     // chunk type id
    quint32 uiSize;   // total size of this chunk including this header
    quint32 uiTime;   // posix timestamp, utc
    quint8  aData[0];
  }
  __attribute__((packed));

  struct ChunkReadInfo
  {
    uint    uiOffset; // offset in file
    quint16 uiId;     // same as Chunk::uiId
    uint    uiSize;   // same as Chunk::uiSize
    uint    uiTime;   // same as Chunk::uiTime
  };


public :
  GPSRFile (void);
  ~GPSRFile (void);

  bool openWrite (const char* pszFile, bool bTruncate);
  bool openRead  (const char* pszFile); // implicit call to seekFirst()

  bool        isOpen    (void) const { return m_pFile != 0; }
  bool        isWriting (void) const { return m_bWriting; }
  bool        isError   (void) const { return m_bError; }
  bool        isEOF     (void) const { return m_bEOF; }
  const char* getPath   (void) const { return m_strFilePath.constData(); }

  void close (void);

  void writeMessage         (time_t uiTime, const char* pszMessage);
  void writeLocationFix     (time_t uiTime, const LocationFixContainer& fixCont);
  void writeLocationFixLost (time_t uiTime);
  void writeSnap            (time_t uiTime);

  bool seekFirst    (void); // causes a sigReadSOF or a sigReadError
  bool readNext     (void); // causes a sigReadChunk*, a sigReadEOF, or a sigReadError
  bool readPrevious (void); // causes a sigReadChunk*, a sigReadEOF, or a sigReadError
  bool readChunk    (int nChunkIndex);

  // read mode properties
  bool                          isReadDiscovery    (void) const { return m_bDiscoveryRead; }
  int                           getReadChunkIndex  (void) const { return m_nReadIndex; }
  int                           getReadChunksCount (void) const { return m_vecReadChunks.count(); }
  const QVector<ChunkReadInfo>& getReadChunksInfo  (void) const { return m_vecReadChunks; }


signals :
  void sigReadError                (GPSRFile* pGPSRFile, GPSRFile::Error eError);
  void sigReadSOF                  (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion);
  void sigReadChunkMessage         (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMsgLen);
  void sigReadChunkLocationFix     (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix);
  void sigReadChunkLocationFixLost (GPSRFile* pGPSRFile, time_t uiTime);
  void sigReadChunkSnap            (GPSRFile* pGPSRFile, time_t uiTime);
  void sigReadChunkUnknown         (GPSRFile* pGPSRFile, GPSRFile::Chunk* pChunk);
  void sigReadEOF                  (GPSRFile* pGPSRFile);


private :
  uint maxChunkDataSize (void) const { return sizeof(m_Swap) - sizeof(Chunk); }
  void writeData        (const char* pData, uint uiSize);
  bool readSize         (char* pOutData, uint uiExpectedSize, bool* pbGotEOF);
  void signalReadError  (Error eError);


private :
  FILE*      m_pFile;
  QByteArray m_strFilePath;
  bool       m_bWriting;
  bool       m_bError;
  bool       m_bEOF; // only used in 'read' mode
  QByteArray m_Swap;

  // 'read' mode state
  bool m_bDiscoveryRead;
  int  m_nReadIndex;
  QVector<ChunkReadInfo> m_vecReadChunks;
};


#endif // #ifndef __GPSRFILE_H__
