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

  static Location*            createDevice    (void); // try to create a true Location driver
//static Location*            createReplay    (void); // driver to load and replay raw file data
  static const QVector<uint>& allowedFixSteps (void);


  virtual bool setFixStep (uint uiNewFixStepSeconds) = 0;

  virtual void resetLastFix (void);

  virtual void start (void) = 0;
  virtual void stop  (void) = 0;

  virtual bool isStarted (void) const { return m_bStarted; }


signals :
  void sigGotLocationFix (Location* pLocation, LocationFix* pLocationFix);


protected :
  // state
  uint  m_uiFixStep; // seconds
  bool  m_bStarted;

  // last fix
  LocationFix* m_pFix;
  time_t       m_uiFixTime;
};


#endif // #ifndef __LOCATION_H__
