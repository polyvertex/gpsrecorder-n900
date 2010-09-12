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
// Created On : 2010-04-03 12:03:01
//
// $Id$
//
//***************************************************************************

#ifndef __EXPORTERSINKKML_H__
#define __EXPORTERSINKKML_H__

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSinkKml
//---------------------------------------------------------------------------
class ExporterSinkKml : public ExporterSink
{
public :
  ExporterSinkKml (Exporter* pParent);
  virtual ~ExporterSinkKml (void);

  const char* label     (void) const { return "KML"; }
  const char* extension (void) const { return "kml"; }

  void close (void);

  // default settings
  static bool           defaultZipped       (void) { return true; }
  static QColor         defaultLineColor    (void) { return QColor(0, 255, 0, 255); }
  static int            defaultLineWidth    (void) { return 2; }
  static QPair<int,int> lineWidthBounds     (void) { return QPair<int,int>(1, 5); }
  static bool           defaultAircraftMode (void) { return false; }
  static bool           defaultColorBySpeed (void) { return false; }

  // static utils
  static QString colorToString (const QColor& color);


private :
  void writeEOF (void);

  static bool kmlToKmz (const QString& strSrcFile);


public slots :
  void onSOF         (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset);
  void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void onGizmoPoint  (const Exporter::GizmoPoint* pGizmoPoint);
  void onEOF         (void);


private :
  // settings
  bool       m_bZipped;
  QByteArray m_strLineColor;
  int        m_nLineWidth;
  bool       m_bAircraftMode;

  // runtime
  int                  m_iTimeZoneOffset;
  time_t               m_uiTimeBegin;
  time_t               m_uiTimeEnd;
  LocationFixContainer m_FixContBegin;
  LocationFixContainer m_FixContEnd;
  QVector<Exporter::GizmoPoint> m_vecGizmoPoints;
};


#endif // #ifndef __EXPORTERSINKKML_H__
