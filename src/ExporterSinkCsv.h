//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-03 11:58:43
//
// $Id$
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
