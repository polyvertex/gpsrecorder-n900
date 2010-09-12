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
    GIZMO_SNAP,
    GIZMO_PAUSE,
    GIZMO_RESUME,
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
  void onReadError                (GPSRFile* pGPSRFile, GPSRFile::Error eError);
  void onReadSOF                  (GPSRFile* pGPSRFile, time_t uiTime, quint8 ucFormatVersion, qint32 iTimeZoneOffset);
  void onReadChunkMessage         (GPSRFile* pGPSRFile, time_t uiTime, const char* pszMessage, uint uiMessageLen);
  void onReadChunkLocationFix     (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix);
  void onReadChunkLocationFixLost (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkSnap            (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkNamedSnap       (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPointName, uint uiPointNameLen);
  void onReadChunkPaused          (GPSRFile* pGPSRFile, time_t uiTime, const char* pszPauseName, uint uiPauseNameLen);
  void onReadChunkResumed         (GPSRFile* pGPSRFile, time_t uiTime);
  void onReadChunkUnknown         (GPSRFile* pGPSRFile, GPSRFile::Chunk* pChunk);
  void onReadEOF                  (GPSRFile* pGPSRFile);


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
