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
