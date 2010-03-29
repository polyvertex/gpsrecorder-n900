//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:26
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRECORD_LOCATIONMAEMO_H__
#define __GPSRECORD_LOCATIONMAEMO_H__

#include "stable.h"


//---------------------------------------------------------------------------
// LocationMaemo
//
// Nokia's liblocation references :
// * http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Connectivity_Components/Using_Location_API
// * http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDControl.html
// * http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html
//---------------------------------------------------------------------------
class LocationMaemo : public QObject
{
  Q_OBJECT

public :
  enum State
  {
    STATE_STOPPED,
    STATE_PAUSED,
    STATE_STARTED,
  };


public :
  LocationMaemo (QObject* pParent=0);
  virtual ~LocationMaemo (void);

  bool setFixStep (uint uiStepSeconds);

  void start (void);
  void pause (void);
  void stop  (void);

  bool  isStarted (void) const;
  bool  isPaused  (void) const;
  State getState  (void) const;


private :
  // fix utils
  static quint8  fixConvertFixMode (LocationGPSDeviceMode eGpsDevMode);
  static quint32 fixNeededSize     (const LocationGPSDevice& gpsdev);
  static Fix*    fixSetup          (const LocationGPSDevice& gpsdev, Fix* pOutFix=0);

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

  // state
  uint  m_uiFixStep; // seconds
  State m_eState;

  // last fix
  Fix*   m_pFix;
  time_t m_uiFixTime;
};


#endif // #ifndef __GPSRECORD_LOCATIONMAEMO_H__
