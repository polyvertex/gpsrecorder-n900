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

    CHUNK_MESSAGE     = 10, // asciiz message (usually a log message)
    CHUNK_LOCATIONFIX = 20, // LocationFix structure (only its storage zone)
    CHUNK_SNAP        = 30, // a 'snap' event (user pushed the 'snap' button while recording)
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


public :
  GPSRFile (void);
  ~GPSRFile (void);

  bool openWrite (const char* pszFile, bool bTruncate);
  bool openRead  (const char* pszFile); // implicit call to seekFirst()
  bool isOpen    (void) const { return m_pFile != 0; }
  bool isReading (void) const { return m_bReading; }
  bool isEOF     (void) const { return m_bReadEOF; }
  void close     (void);

  void writeMessage     (time_t uiTime, const char* pszMessage);
  void writeLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void writeSnap        (time_t uiTime);

  bool seekFirst (void); // causes a sigReadSOF or a sigReadError
  bool readNext  (void); // causes a sigReadChunk*, a sigReadEOF, or a sigReadError


signals :
  void sigReadError            (GPSRFile* pGPSRFile, Error eError);
  void sigReadSOF              (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion);
  void sigReadChunkMessage     (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMsgLen);
  void sigReadChunkLocationFix (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix);
  void sigReadChunkSnap        (GPSRFile* pGPSRFile, time_t uiTime);
  void sigReadChunkUnknown     (GPSRFile* pGPSRFile, Chunk* pChunk);
  void sigReadEOF              (GPSRFile* pGPSRFile);


private :
  uint maxChunkDataSize (void) const { return sizeof(m_Swap) - sizeof(Chunk); }
  void writeData        (const char* pData, uint uiSize);
  bool readSize         (char* pOutData, uint uiExpectedSize, bool* pbGotEOF);


private :
  FILE*      m_pFile;
  QByteArray m_strFilePath;
  bool       m_bReading;
  bool       m_bReadEOF; // reached EOF
  QByteArray m_Swap;
};


#endif // #ifndef __GPSRFILE_H__
