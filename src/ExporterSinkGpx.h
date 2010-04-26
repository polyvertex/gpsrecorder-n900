//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-03 12:06:31
//
// $Id$
//
//***************************************************************************

#ifndef __EXPORTERSINKGPX_H__
#define __EXPORTERSINKGPX_H__

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSinkGpx
//---------------------------------------------------------------------------
class ExporterSinkGpx : public ExporterSink
{
public :
  ExporterSinkGpx (Exporter* pParent);
  virtual ~ExporterSinkGpx (void);

  const char* label     (void) const { return "GPX"; }
  const char* extension (void) const { return "gpx"; }

  void close (void);


private :
  void extractSettings (const QSettings& settings);


public slots :
  void onSOF         (const char* pszFilePath, time_t uiTime);
  void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void onEOF         (void);


private :
};


#endif // #ifndef __EXPORTERSINKGPX_H__
