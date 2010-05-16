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
