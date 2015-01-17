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
// Created: 2010-04-03 11:51:19
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
  virtual void onSOF         (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset) = 0;
  virtual void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont) = 0;
  virtual void onGizmoPoint  (const Exporter::GizmoPoint* pGizmoPoint) { }
  virtual void onEOF         (void) = 0;


protected :
  // settings
  bool m_bExportPause;

  // runtime
  FILE*   m_pFile;
  QString m_strFilePath;
};


#endif // #ifndef __EXPORTERSINK_H__
