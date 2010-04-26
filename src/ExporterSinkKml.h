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
  static QColor defaultLineColor    (void) { return QColor(0, 255, 0, 255); }
  static int    defaultLineWidth    (void) { return 2; }
  static bool   defaultAircraftMode (void) { return false; }
  static bool   defaultColorBySpeed (void) { return false; }

  // static utils
  static QString colorToString (const QColor& color);


private :
  void extractSettings (const QSettings& settings);


public slots :
  void onSOF         (const char* pszFilePath, time_t uiTime);
  void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void onEOF         (void);


private :
  // settings
  QByteArray m_strLineColor;
  int        m_nLineWidth;
  bool       m_bAircraftMode;
  bool       m_bColorBySpeed;
};


#endif // #ifndef __EXPORTERSINKKML_H__
