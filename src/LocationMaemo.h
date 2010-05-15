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
