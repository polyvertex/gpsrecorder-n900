//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-04-12 13:21:53
//
// $Id$
//
//***************************************************************************


#ifndef __LOCATIONGPSRFILE_H__
#define __LOCATIONGPSRFILE_H__

#include "stable.h"


//---------------------------------------------------------------------------
// LocationGPSRFile
//---------------------------------------------------------------------------
class LocationGPSRFile : public Location
{
public :
  LocationGPSRFile (void);
  virtual ~LocationGPSRFile (void);

  bool setFixStep (uint uiNewFixStepSeconds);

  void resetLastFix (void);

  void start (void);
  void stop  (void);


  void setInputFileName (const char* pszFilePath);
  uint getFixCount      (void) const { return (uint)m_vecFixChunkIndex.count(); }
  bool readFix          (uint uiFixIndex);
  bool readFirstFix     (void)       { return this->readFix(0); }
  uint getFixIndex      (void) const { return m_uiFixIndex; }


private slots :
  void onReadError                (GPSRFile* pGPSRFile, GPSRFile::Error eError);
  void onReadChunkLocationFix     (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix);


private :
  QByteArray m_strFilePath;
  GPSRFile   m_GPSRFile;

  QVector<uint> m_vecFixChunkIndex;
  uint m_uiFixIndex; // current fix index
};


#endif // #ifndef __LOCATIONGPSRFILE_H__
