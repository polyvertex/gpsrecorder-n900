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
// Created On : 2010-03-29 14:19:38
//
// $Id$
//
//***************************************************************************

#ifndef __LOCATION_H__
#define __LOCATION_H__

#include "stable.h"


//---------------------------------------------------------------------------
// Location
//---------------------------------------------------------------------------
class Location : public QObject
{
  Q_OBJECT

public :
  Location (QObject* pParent);
  virtual ~Location (void);

  static Location*            createDevice             (void); // try to create a true Location driver
  static Location*            createReplay             (const char* pszInputFile); // driver to load and replay raw file data
  static const QVector<uint>& allowedFixSteps          (void);
  static bool                 isAllowedFixStep         (uint uiFixStep);
  static uint                 selectBestAllowedFixStep (uint uiDesiredFixStep);


  virtual bool getAssisted (bool bAssisted) const { return m_bAssisted; }
  virtual bool setAssisted (bool bAssisted) = 0;

  virtual uint getFixStep (void) const { return m_uiFixStep; }
  virtual bool setFixStep (uint uiNewFixStepSeconds) = 0;

  virtual void resetLastFix (void);

  virtual void start (void) = 0;
  virtual void stop  (void) = 0;

  virtual bool isStarted   (void) const { return m_bStarted; }
  virtual bool isAcquiring (void) const { return m_bAcquiring; }

  virtual const LocationFixContainer& getLastFixCont (void) const { return m_FixCont; }


signals :
  void sigLocationFixGain (Location* pLocation, const LocationFixContainer* pFixCont);
  void sigLocationFixLost (Location* pLocation, const LocationFixContainer* pLastFixCont);
  void sigLocationFix     (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


protected :
  // state
  bool  m_bAssisted;
  uint  m_uiFixStep; // seconds
  bool  m_bStarted;

  // last fix
  LocationFixContainer m_FixCont;
  bool                 m_bAcquiring;
};


#endif // #ifndef __LOCATION_H__
