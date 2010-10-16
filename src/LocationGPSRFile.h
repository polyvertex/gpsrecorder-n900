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

  bool setAssisted (bool bAssisted); // unused

  bool setFixStep (uint uiNewFixStepSeconds); // unused

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
