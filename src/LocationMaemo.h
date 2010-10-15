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
// Created On : 2010-03-14 18:43:26
//
// $Id$
//
//***************************************************************************

#ifndef __LOCATIONMAEMO_H__
#define __LOCATIONMAEMO_H__

#include "stable.h"


//---------------------------------------------------------------------------
// LocationMaemo
//
// Nokia's liblocation references :
// * http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Connectivity_Components/Using_Location_API
// * http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDControl.html
// * http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html
//---------------------------------------------------------------------------
class LocationMaemo : public Location
{
public :
  LocationMaemo (void);
  virtual ~LocationMaemo (void);

  bool setAssisted (bool bAssisted);

  bool setFixStep (uint uiNewFixStepSeconds);

  void resetLastFix (void);

  void start (void);
  void stop  (void);


private :
  // fix utils
  static quint8 fixConvertFixMode (LocationGPSDeviceMode eGpsDevMode);
  static void   fixSetup          (LocationFixContainer& fixCont, const LocationGPSDevice& gpsdev);

  // callbacks from gps device
  static void locationOnDevConnected    (LocationGPSDevice* pGpsDevice, gpointer pUserData);
  static void locationOnDevDisconnected (LocationGPSDevice* pGpsDevice, gpointer pUserData);
  static void locationOnDevChanged      (LocationGPSDevice* pGpsDevice, gpointer pUserData);

  // callbacks from gpsd
  static void locationOnGpsdRunning      (LocationGPSDControl* pGpsdControl, gpointer pUserData);
  static void locationOnGpsdStopped      (LocationGPSDControl* pGpsdControl, gpointer pUserData);
  static void locationOnGpsdErrorVerbose (LocationGPSDControl* pGpsdControl, LocationGPSDControlError eError, gpointer pUserData);


private :
  LocationGPSDControlInterval m_eGpsdControlInterval;
  LocationGPSDControlMethod   m_eGpsdControlMethod;

  LocationGPSDevice*   m_pGpsDevice;
  LocationGPSDControl* m_pGpsdControl;
  guint                m_auiSigHdlGpsDevice[3];
  guint                m_auiSigHdlGpsdControl[3];
};


#endif // #ifndef __LOCATIONMAEMO_H__
