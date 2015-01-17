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
// Created: 2010-10-25 08:30:51
//
//***************************************************************************

#ifndef __EXPORTERSINKTXT_H__
#define __EXPORTERSINKTXT_H__

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSinkTxt
//---------------------------------------------------------------------------
class ExporterSinkTxt : public ExporterSink
{
public :
  ExporterSinkTxt (Exporter* pParent);
  virtual ~ExporterSinkTxt (void);

  const char* label     (void) const { return "TXT"; }
  const char* extension (void) const { return "txt"; }

  void close (void);

  // default settings
  static bool defaultIncludelocationFix (void) { return false; }


public slots :
  void onSOF         (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset);
  void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void onEOF         (void);


private :
  bool m_bIncludeLocationFix;
};


#endif // #ifndef __EXPORTERSINKTXT_H__
