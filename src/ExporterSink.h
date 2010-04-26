//***************************************************************************
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


private :
  virtual void extractSettings (const QSettings& settings) = 0;


public slots :
  virtual void onSOF         (const char* pszFilePath, time_t uiTime) = 0;
  virtual void onLocationFix (time_t uiTime, const LocationFixContainer& fixCont) = 0;
  virtual void onEOF         (void) = 0;


protected :
  FILE*   m_pFile;
  QString m_strFilePath;
};


#endif // #ifndef __EXPORTERSINK_H__
