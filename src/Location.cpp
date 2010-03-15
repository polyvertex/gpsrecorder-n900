//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:29
//
// $Id$
//
//***************************************************************************

#include "main.h"


//---------------------------------------------------------------------------
// Location
//---------------------------------------------------------------------------
Location::Location (QObject* pParent/*=0*/)
: QObject(pParent)
{
  // init location lib
  {
    m_pGpsDevice   = (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);
    m_pGpsdControl = location_gpsd_control_get_default();

    g_object_set(G_OBJECT(m_pGpsdControl), "preferred-interval", LOCATION_INTERVAL_5S, NULL);
    g_object_set(G_OBJECT(m_pGpsdControl), "preferred-method",   LOCATION_METHOD_GNSS | LOCATION_METHOD_AGNSS, NULL);

    m_auiSigHdlGpsDevice[0] = g_signal_connect(G_OBJECT(m_pGpsDevice), "connected",    G_CALLBACK(Location::Location_OnDevConnected), NULL);
    m_auiSigHdlGpsDevice[1] = g_signal_connect(G_OBJECT(m_pGpsDevice), "disconnected", G_CALLBACK(Location::Location_OnDevDisconnected), NULL);
    m_auiSigHdlGpsDevice[2] = g_signal_connect(G_OBJECT(m_pGpsDevice), "changed",      G_CALLBACK(Location::Location_OnDevChanged), NULL);

	  m_auiSigHdlGpsdControl[0] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_running",  G_CALLBACK(Location::Location_OnGpsdRunning), NULL);
	  m_auiSigHdlGpsdControl[1] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_stopped",  G_CALLBACK(Location::Location_OnGpsdStopped), NULL);
	  m_auiSigHdlGpsdControl[2] = g_signal_connect(G_OBJECT(m_pGpsdControl), "error-verbose", G_CALLBACK(Location::Location_OnGpsdErrorVerbose), NULL);

    location_gps_device_reset_last_known(m_pGpsDevice);
    location_gpsd_control_start(m_pGpsdControl); // if (m_pGpsdControl->can_control)
  }
}

//---------------------------------------------------------------------------
// ~Location
//---------------------------------------------------------------------------
Location::~Location (void)
{
  // uninit location lib
  {
    location_gpsd_control_stop(m_pGpsdControl); // if (m_pGpsdControl->can_control)

    g_signal_handler_disconnect(m_pGpsDevice, m_auiSigHdlGpsDevice[0]);
    g_signal_handler_disconnect(m_pGpsDevice, m_auiSigHdlGpsDevice[1]);
    g_signal_handler_disconnect(m_pGpsDevice, m_auiSigHdlGpsDevice[2]);

    g_signal_handler_disconnect(m_pGpsdControl, m_auiSigHdlGpsdControl[0]);
    g_signal_handler_disconnect(m_pGpsdControl, m_auiSigHdlGpsdControl[1]);
    g_signal_handler_disconnect(m_pGpsdControl, m_auiSigHdlGpsdControl[2]);

    g_object_unref(m_pGpsdControl);
    g_object_unref(m_pGpsDevice);
  }
}




//---------------------------------------------------------------------------
// Location_OnDevConnected
//---------------------------------------------------------------------------
void Location::Location_OnDevConnected (LocationGPSDevice* /*pGpsDevice*/, gpointer /*pUserData*/)
{
  g_debug("GPS device connected.");
}

//---------------------------------------------------------------------------
// Location_OnDevDisconnected
//---------------------------------------------------------------------------
void Location::Location_OnDevDisconnected (LocationGPSDevice* /*pGpsDevice*/, gpointer /*pUserData*/)
{
  g_debug("GPS device DISCONNECTED !");
}

//---------------------------------------------------------------------------
// Location_OnDevChanged
//---------------------------------------------------------------------------
void Location::Location_OnDevChanged (LocationGPSDevice* pGpsDevice, gpointer /*pUserData*/)
{
  if (!pGpsDevice)
    return;

  if (pGpsDevice->fix)
  {
    // TODO
  }

  if (pGpsDevice->cell_info)
  {
    // TODO
  }
}




//---------------------------------------------------------------------------
// Location_OnGpsdRunning
//---------------------------------------------------------------------------
void Location::Location_OnGpsdRunning (LocationGPSDControl* /*pGpsdControl*/, gpointer /*pUserData*/)
{
  g_debug("GPSD running.");
}

//---------------------------------------------------------------------------
// Location_OnGpsdStopped
//---------------------------------------------------------------------------
void Location::Location_OnGpsdStopped (LocationGPSDControl* /*pGpsdControl*/, gpointer /*pUserData*/)
{
  g_debug("GPSD STOPPED !");
}

//---------------------------------------------------------------------------
// Location_OnGpsdErrorVerbose
//---------------------------------------------------------------------------
void Location::Location_OnGpsdErrorVerbose (LocationGPSDControl* /*pGpsdControl*/, LocationGPSDControlError eError, gpointer /*pUserData*/)
{
  switch (eError)
  {
    case LOCATION_ERROR_USER_REJECTED_DIALOG :
      g_debug("User rejected location enabling dialog.");
      break;

    case LOCATION_ERROR_USER_REJECTED_SETTINGS :
      g_debug("User changed settings which disabled locationing.");
      break;

    case LOCATION_ERROR_BT_GPS_NOT_AVAILABLE :
      g_debug("Problems using BT GPS.");
      break;

    case LOCATION_ERROR_METHOD_NOT_ALLOWED_IN_OFFLINE_MODE :
      g_debug("Method unavailable in offline mode.");
      break;

    case LOCATION_ERROR_SYSTEM :
      g_debug("System error ! SIM card is missing ?");
      break;

    default :
      g_debug("Unknown GPSD error %d !", eError);
      break;
  }
}
