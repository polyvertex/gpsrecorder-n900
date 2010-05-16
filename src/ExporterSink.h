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
// Created On : 2010-04-03 11:51:19
//
// $Id$
//
//***************************************************************************

#ifndef __EXPORTERSINK_H__
#define __EXPORTERSINK_H__

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSink
//---------------------------------------------------------------------------
class ExporterSink : public QObject
{
  Q_OBJECT

public :
  ExporterSink (Exporter* pParent);
  virtual ~ExporterSink (void);

  virtual const char* label     (void) const = 0;
  virtual const char* extension (void) const = 0;

  Exporter*       parent (void)       { return static_cast<Exporter*>(QObject::parent()); }
  const Exporter* parent (void) const { return const_cast<Exporter*>(this->parent()); }

  virtual void close (void);


public slots :
  virtual void onSOF          (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset) = 0;
  virtual void onLocationFix  (time_t uiTime, const LocationFixContainer& fixCont) = 0;
  virtual void onSnappedPoint (const Exporter::SnappedPoint* pSnappedPoint) { }
  virtual void onEOF          (void) = 0;


protected :
  FILE*   m_pFile;
  QString m_strFilePath;
};


#endif // #ifndef __EXPORTERSINK_H__
