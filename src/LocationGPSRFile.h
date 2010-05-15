//***************************************************************************
//
// GPS Recorder
// Show, log and convert GPS data from Maemo's GPS subsystem.
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
