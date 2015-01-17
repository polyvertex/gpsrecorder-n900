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
// Created: 2010-04-03 11:58:43
//
//***************************************************************************

#ifndef __EXPORTERSINKCSV_H__
#define __EXPORTERSINKCSV_H__

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSinkCsv
//---------------------------------------------------------------------------
class ExporterSinkCsv : public ExporterSink
{
public :
  ExporterSinkCsv (Exporter* pParent);
  virtual ~ExporterSinkCsv (void);

  const char* label     (void) const { return "CSV"; }
  const char* extension (void) const { return "csv"; }

  void close (void);

  // default settings
  static char        defaultSeparator    (void) { return ','; }
  static const char* supportedSeparators (void) { return ",;\t"; }
  static int         separatorIndex      (char cSep);
  static bool        isAllowedSeparator  (char cSep) { return separatorIndex(cSep) >= 0; }


public slots :
  void onSOF         (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset);
  void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont);
  void onEOF         (void);


private :
  char m_cSeparator;
};


#endif // #ifndef __EXPORTERSINKCSV_H__
