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
  void writeEOF (void);


public slots :
  void onSOF         (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset);
  void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void onGizmoPoint  (const Exporter::GizmoPoint* pGizmoPoint);
  void onEOF         (void);


private :
  // runtime
  QVector<Exporter::GizmoPoint> m_vecGizmoPoints;
};


#endif // #ifndef __EXPORTERSINKGPX_H__
