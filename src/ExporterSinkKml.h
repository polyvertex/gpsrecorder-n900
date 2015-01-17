//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex@gmail.com>
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
// Created: 2010-04-03 12:03:01
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
