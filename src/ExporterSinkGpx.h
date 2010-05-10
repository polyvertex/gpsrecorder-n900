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


public slots :
  void onSOF          (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset);
  void onLocationFix  (time_t uiTime, const LocationFixContainer& fixCont);
  void onSnappedPoint (const Exporter::SnappedPoint* pSnappedPoint);
  void onEOF          (void);


private :
  // runtime
  uint m_uiSnapCount;
};


#endif // #ifndef __EXPORTERSINKGPX_H__
