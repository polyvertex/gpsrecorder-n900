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
class GPSRFile
{
public :
  enum
  {
    FORMAT_VERSION = 0x01,

    CHUNK_MESSAGE     = 10, // asciiz message (usually a log message)
    CHUNK_LOCATIONFIX = 20, // LocationFix structure
    CHUNK_SNAP        = 30, // a 'snap' event (user pushed the 'snap' button while recording)
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
    quint16 uiId;   // chunk type id
    quint32 uiSize; // total size of this chunk including this header

    quint32 uiTime; // posix timestamp, utc
    quint8  aData[0];
  }
  __attribute__((packed));


public :
  GPSRFile (void);
  ~GPSRFile (void);

  bool openWrite (const char* pszFile, bool bTruncate);
//bool openRead  (const char* pszFile);
  bool isOpen    (void) const { return m_pFile != 0; }
  bool isReading (void) const { return m_bReading; }
  void close     (void);

  void writeMessage     (time_t uiTime, const char* pszMessage);
  void writeLocationFix (time_t uiTime, const LocationFix& fix);
  void writeSnap        (time_t uiTime);


private :
  uint maxChunkDataSize (void) const { return sizeof(m_Swap) - sizeof(Chunk); }
  void writeData        (const char* pData, uint uiSize);


private :
  FILE*      m_pFile;
  QByteArray m_strFilePath;
  bool       m_bReading;

  quint8 m_Swap[2048];
};


#endif // #ifndef __GPSRFILE_H__
