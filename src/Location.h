//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:26
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRECORD_LOCATION_H__
#define __GPSRECORD_LOCATION_H__

#include "stable.h"


//---------------------------------------------------------------------------
// Location
//
// Nokia's liblocation references :
// * http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Connectivity_Components/Using_Location_API
// * http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDControl.html
// * http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html
//---------------------------------------------------------------------------
class Location : public QObject
{
  Q_OBJECT

public :
  /*struct Fix
  {
  };*/


public :
  Location (QObject* pParent=0);
  virtual ~Location (void);


private :
  // callbacks from gps device
  static void locationOnDevConnected    (LocationGPSDevice* pGpsDevice, gpointer pUserData);
  static void locationOnDevDisconnected (LocationGPSDevice* pGpsDevice, gpointer pUserData);
  static void locationOnDevChanged      (LocationGPSDevice* pGpsDevice, gpointer pUserData);

  // callbacks from gpsd
  static void locationOnGpsdRunning      (LocationGPSDControl* pGpsdControl, gpointer pUserData);
  static void locationOnGpsdStopped      (LocationGPSDControl* pGpsdControl, gpointer pUserData);
  static void locationOnGpsdErrorVerbose (LocationGPSDControl* pGpsdControl, LocationGPSDControlError eError, gpointer pUserData);


private :
  // context
  LocationGPSDevice*   m_pGpsDevice;
  LocationGPSDControl* m_pGpsdControl;
  guint                m_auiSigHdlGpsDevice[3];
  guint                m_auiSigHdlGpsdControl[3];

  // fix history
};


#endif // #ifndef __GPSRECORD_LOCATION_H__
