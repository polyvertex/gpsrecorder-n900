//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-03 11:46:07
//
// $Id$
//
//***************************************************************************

#ifndef __EXPORTER_H__
#define __EXPORTER_H__

#include "stable.h"


//---------------------------------------------------------------------------
// Exporter
//---------------------------------------------------------------------------
class Exporter : public QObject
{
  Q_OBJECT

public :
  Exporter (void);
  virtual ~Exporter (void);

  bool exportFile  (const QString& strFile);
  uint exportFiles (const QStringList& filesList);
  uint exportDir   (const QString& strDirectory, const QString& strSuffix=".gpsr");

  GPSRFile& gpsrFile (void) { return m_GPSRFile; }


private :
  void clear (void);


private slots :
  void onReadError                (GPSRFile* pGPSRFile, GPSRFile::Error eError);
  void onReadSOF                  (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion);
  void onReadChunkMessage         (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMessageLen);
  void onReadChunkLocationFix     (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix);
  void onReadChunkLocationFixLost (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkSnap            (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkNamedSnap       (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPointName, uint uiPointNameLen);
  void onReadChunkUnknown         (GPSRFile* pGPSRFile, GPSRFile::Chunk* pChunk);
  void onReadEOF                  (GPSRFile* pGPSRFile);


signals :
  // TODO : implement Message, FixLost and Snap signals
  void sigSOF         (const char* pszFilePath, time_t uiTime);
  void sigLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void sigEOF         (void);


private :
  //QVector<ExporterSink*> m_vecSinks;

  QString              m_strOutputBasePath;
  GPSRFile             m_GPSRFile;
  LocationFixContainer m_FixCont;
};


#endif // #ifndef __EXPORTER_H__
