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


public slots :
  void onSOF         (const char* pszFilePath, time_t uiTime);
  void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void onEOF         (void);


private :
};


#endif // #ifndef __EXPORTERSINKKML_H__
