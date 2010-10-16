//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
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
//
// Format V0 :
// * Initial version.
//
// Format V1 :
// * Appended iTimeZoneOffset field to the Header structure.
//
// Format V2 :
// * Added PAUSED and RESUMED chunks types.
//---------------------------------------------------------------------------
class GPSRFile : public QObject
{
  Q_OBJECT

public :
  enum
  {
    FORMAT_VERSION_V0 = 0x00,
    FORMAT_VERSION_V1 = 0x01,
    FORMAT_VERSION_V2 = 0x02,
    FORMAT_VERSION    = FORMAT_VERSION_V2, // current format version

    CHUNK_MESSAGE          = 10, // asciiz message (usually a log/event message)
    CHUNK_LOCATIONFIX      = 20, // LocationFix structure (only its storage zone)
    CHUNK_LOCATIONFIX_LOST = 21, // Lost GPS fix
    CHUNK_SNAP             = 30, // obsolete, write NAMEDSNAP chunk instead
    CHUNK_NAMEDSNAP        = 31, // a 'named snap' event (user pushed the 'snap' button while recording)
    CHUNK_PAUSED           = 40, // (v2+) track paused
    CHUNK_RESUMED          = 41, // (v2+) track resumed
  };

  enum Error
  {
    ERROR_NOTOPEN        = 1, // file is not open or is not in reading mode
    ERROR_READ           = 2, // system error occurred while reading file
    ERROR_FORMAT         = 3, // file or chunk header does not match
    ERROR_FORMAT_VERSION = 4, // format version does not match
    ERROR_TRUNCATED      = 5, // file is incomplete
  };

  struct Header
  {
    quint8  aucMagic[4];     // 'G','P','S','R'
    quint8  ucFormat;        // format version
    quint32 uiTime;          // start time (posix timestamp, utc)
    qint32  iTimeZoneOffset; // (v1+) time zone offset in seconds
  }
  __attribute__((packed));

  struct Chunk
  {
    quint8  ucMagic;  // '@'
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

  bool              isOpen    (void) const { return m_pFile != 0; }
  bool              isWriting (void) const { return m_bWriting; }
  bool              isError   (void) const { return m_bError; }
  bool              isEOF     (void) const { return m_bEOF; }
  const QByteArray& getPath   (void) const { return m_strFilePath; }

  void close (void);

  void writeMessage         (time_t uiTime, const char* pszMessage);
  void writeLocationFix     (time_t uiTime, const LocationFixContainer& fixCont);
  void writeLocationFixLost (time_t uiTime);
//void writeSnap            (time_t uiTime); // obsolete
  void writeNamedSnap       (time_t uiTime, const char* pszName);
  void writePaused          (time_t uiTime, const char* pszName);
  void writeResumed         (time_t uiTime);

  bool seekFirst    (void); // causes a sigReadSOF or a sigReadError
  bool readNext     (void); // causes a sigReadChunk*, a sigReadEOF, or a sigReadError
  bool readPrevious (void); // causes a sigReadChunk*, a sigReadEOF, or a sigReadError
  bool readChunk    (int nChunkIndex);

  // read mode properties
  bool                          isReadDiscovery    (void) const { return m_bDiscoveryRead; }
  bool                          isIncomplete       (void) const { return m_bIncomplete; }
  int                           getReadChunkIndex  (void) const { return m_nReadIndex; }
  int                           getReadChunksCount (void) const { return m_vecReadChunks.count(); }
  int                           getReadChunksCount (quint16 uiChunkId) const;
  const QVector<ChunkReadInfo>& getReadChunksInfo  (void) const { return m_vecReadChunks; }


signals :
  void sigReadError                (GPSRFile* pGPSRFile, GPSRFile::Error eError);
  void sigReadSOF                  (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion, qint32 iTimeZoneOffset);
  void sigReadChunkMessage         (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMessageLen);
  void sigReadChunkLocationFix     (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix);
  void sigReadChunkLocationFixLost (GPSRFile* pGPSRFile, time_t uiTime);
  void sigReadChunkSnap            (GPSRFile* pGPSRFile, time_t uiTime);
  void sigReadChunkNamedSnap       (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPointName, uint uiMsgLen);
  void sigReadChunkPaused          (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPointName, uint uiMsgLen);
  void sigReadChunkResumed         (GPSRFile* pGPSRFile, time_t uiTime);
  void sigReadChunkUnknown         (GPSRFile* pGPSRFile, GPSRFile::Chunk* pChunk);
  void sigReadEOF                  (GPSRFile* pGPSRFile);


private :
  void writeChunkSimple (quint16 uiChunkId, time_t uiTime);
  void writeChunkAsciiz (quint16 uiChunkId, time_t uiTime, const char* pszText);
  void writeData        (const char* pData, uint uiSize);
  bool readSize         (char* pOutData, uint uiExpectedSize, bool bIsFileHeader, bool* pbGotEOF);
  void signalReadError  (Error eError);


private :
  FILE*      m_pFile;
  QByteArray m_strFilePath;
  bool       m_bWriting;
  bool       m_bError;
  bool       m_bEOF;        // only used in 'read' mode
  bool       m_bIncomplete; // only used in 'read' mode
  QByteArray m_Swap;

  // 'read' mode state
  bool m_bDiscoveryRead;
  int  m_nReadIndex;
  QVector<ChunkReadInfo> m_vecReadChunks;
};


#endif // #ifndef __GPSRFILE_H__
