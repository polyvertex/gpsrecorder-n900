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
  enum GizmoType
  {
    GIZMO_TRACK,
    GIZMO_SNAP,
    GIZMO_PAUSE,
    GIZMO_RESUME,
    GIZMO_MEANSTRANSPORT,
  };

  struct GizmoPoint
  {
    GizmoType  eType;
    time_t     uiTime;
    QByteArray strName;
    uint       uiTypeIndex;
    double     rLatDeg;
    double     rLongDeg;
    bool       bHasAlt;
    qint32     iAltM;

    union
    {
      quint8 ucMeansOfTransport;
    }
    data;

    inline GizmoPoint (void) { } // blank contructor to optimize lists resizing
    GizmoPoint (GizmoType eType_, time_t uiTime_, uint uiTypeIndex_)
    {
      eType       = eType_;
      uiTime      = uiTime_;
      strName.clear();
      uiTypeIndex = uiTypeIndex_;
      rLatDeg     = 0.0;
      rLongDeg    = 0.0;
      bHasAlt     = false;
      iAltM       = 0;
    }
  };


public :
  Exporter (void);
  virtual ~Exporter (void);

  bool exportFile  (const QString& strFile);
  uint exportFiles (const QStringList& filesList);
  uint exportDir   (const QString& strDirectory, const QString& strSuffix=".gpsr");

  GPSRFile& gpsrFile         (void) { return m_GPSRFile; }
  bool      isIncompleteFile (void) const { return m_GPSRFile.isIncomplete(); }


private :
  void clear (void);

  uint gizmoIndex     (GizmoType eType);
  void emitGizmoPoint (GizmoPoint& gizmoPt, const LocationFix* pFixA, const LocationFix* pFixB);


private slots :
  void onReadError                 (GPSRFile* pGPSRFile, GPSRFile::Error eError);
  void onReadSOF                   (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion, qint32 iTimeZoneOffset);
  void onReadChunkMessage          (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMessageLen);
  void onReadChunkLocationFix      (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix);
  void onReadChunkLocationFixLost  (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkSnap             (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkNamedSnap        (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPointName, uint uiPointNameLen);
  void onReadChunkPaused           (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPauseName, uint uiPauseNameLen);
  void onReadChunkResumed          (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkNewTrack         (GPSRFile* pGPSRFile, time_t uiTime, const char* pszTrackName, uint uiTrackNameLen);
  void onReadChunkMeansOfTransport (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucMeansOfTransport, const char* pszOptLabel, uint uiOptLabelLen);
  void onReadChunkUnknown          (GPSRFile* pGPSRFile, GPSRFile::Chunk* pChunk);
  void onReadEOF                   (GPSRFile* pGPSRFile);


signals :
  void sigSOF         (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset);
  void sigLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void sigGizmoPoint  (const Exporter::GizmoPoint* pGizmoPoint);
  void sigEOF         (void);


private :
  QString              m_strOutputBasePath;
  GPSRFile             m_GPSRFile;
  LocationFixContainer m_FixCont;
  QVector<GizmoPoint>  m_vecGizmoPoints;
  QMap<GizmoType,uint> m_mapGizmoIndexes;
};


#endif // #ifndef __EXPORTER_H__
