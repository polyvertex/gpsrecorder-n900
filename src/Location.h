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

#include "main.h"


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
  static void Location_OnDevConnected    (LocationGPSDevice* pGpsDevice, gpointer pUserData);
  static void Location_OnDevDisconnected (LocationGPSDevice* pGpsDevice, gpointer pUserData);
  static void Location_OnDevChanged      (LocationGPSDevice* pGpsDevice, gpointer pUserData);

  // callbacks from gpsd
  static void Location_OnGpsdRunning      (LocationGPSDControl* pGpsdControl, gpointer pUserData);
  static void Location_OnGpsdStopped      (LocationGPSDControl* pGpsdControl, gpointer pUserData);
  static void Location_OnGpsdErrorVerbose (LocationGPSDControl* pGpsdControl, LocationGPSDControlError eError, gpointer pUserData);


private :
  // context
  LocationGPSDevice*   m_pGpsDevice;
  LocationGPSDControl* m_pGpsdControl;
  guint                m_auiSigHdlGpsDevice[3];
  guint                m_auiSigHdlGpsdControl[3];

  // fix history
};


#endif // #ifndef __GPSRECORD_LOCATION_H__
