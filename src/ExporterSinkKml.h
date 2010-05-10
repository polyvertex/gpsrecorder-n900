//***************************************************************************
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
  void onSOF          (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset);
  void onLocationFix  (time_t uiTime, const LocationFixContainer& fixCont);
  void onSnappedPoint (const Exporter::SnappedPoint* pSnappedPoint);
  void onEOF          (void);


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
  QVector<Exporter::SnappedPoint> m_vecSnappedPoints;
};


#endif // #ifndef __EXPORTERSINKKML_H__
