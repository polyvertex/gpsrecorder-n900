//***************************************************************************
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

  static Location*            createDevice     (void); // try to create a true Location driver
  static Location*            createReplay     (const char* pszInputFile); // driver to load and replay raw file data
  static const QVector<uint>& allowedFixSteps  (void);
  static bool                 isAllowedFixStep (uint uiFixStep);


  virtual bool setFixStep (uint uiNewFixStepSeconds) = 0;

  virtual void resetLastFix (void);

  virtual void start (void) = 0;
  virtual void stop  (void) = 0;

  virtual bool isStarted   (void) const { return m_bStarted; }
  virtual bool isAcquiring (void) const { return m_bAcquiring; }

  virtual const LocationFixContainer& getLastFixCont (void) const { return m_FixCont; }
  virtual time_t                      getLastFixTime (void) const { return m_uiFixTime; }


signals :
  void sigLocationFixGain (Location* pLocation, const LocationFixContainer* pFixCont);
  void sigLocationFixLost (Location* pLocation, const LocationFixContainer* pLastFixCont);
  void sigLocationFix     (Location* pLocation, const LocationFixContainer* pFixCont, bool bAccurate);


protected :
  // state
  uint  m_uiFixStep; // seconds
  bool  m_bStarted;

  // last fix
  LocationFixContainer m_FixCont;
  time_t               m_uiFixTime;
  bool                 m_bAcquiring;
};


#endif // #ifndef __LOCATION_H__
