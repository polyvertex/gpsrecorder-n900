//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:29
//
// $Id$
//
//***************************************************************************

#include "stable.h"


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

    m_auiSigHdlGpsDevice[0] = g_signal_connect(G_OBJECT(m_pGpsDevice), "connected",    G_CALLBACK(Location::locationOnDevConnected), this);
    m_auiSigHdlGpsDevice[1] = g_signal_connect(G_OBJECT(m_pGpsDevice), "disconnected", G_CALLBACK(Location::locationOnDevDisconnected), this);
    m_auiSigHdlGpsDevice[2] = g_signal_connect(G_OBJECT(m_pGpsDevice), "changed",      G_CALLBACK(Location::locationOnDevChanged), this);

	  m_auiSigHdlGpsdControl[0] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_running",  G_CALLBACK(Location::locationOnGpsdRunning), this);
	  m_auiSigHdlGpsdControl[1] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_stopped",  G_CALLBACK(Location::locationOnGpsdStopped), this);
	  m_auiSigHdlGpsdControl[2] = g_signal_connect(G_OBJECT(m_pGpsdControl), "error-verbose", G_CALLBACK(Location::locationOnGpsdErrorVerbose), this);

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
// locationOnDevConnected
//---------------------------------------------------------------------------
void Location::locationOnDevConnected (LocationGPSDevice* pGpsDevice, gpointer pUserData)
{
  Location* pThis = reinterpret_cast<Location*>(pUserData);

  Q_UNUSED(pGpsDevice);
  Q_UNUSED(pThis);

  qDebug("GPS device connected.");
}

//---------------------------------------------------------------------------
// locationOnDevDisconnected
//---------------------------------------------------------------------------
void Location::locationOnDevDisconnected (LocationGPSDevice* pGpsDevice, gpointer pUserData)
{
  Location* pThis = reinterpret_cast<Location*>(pUserData);

  Q_UNUSED(pGpsDevice);
  Q_UNUSED(pThis);

  qDebug("GPS device DISCONNECTED !");
}

//---------------------------------------------------------------------------
// locationOnDevChanged
//---------------------------------------------------------------------------
void Location::locationOnDevChanged (LocationGPSDevice* pGpsDevice, gpointer pUserData)
{
  Location* pThis = reinterpret_cast<Location*>(pUserData);

  Q_UNUSED(pThis);

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
// locationOnGpsdRunning
//---------------------------------------------------------------------------
void Location::locationOnGpsdRunning (LocationGPSDControl* pGpsdControl, gpointer pUserData)
{
  Location* pThis = reinterpret_cast<Location*>(pUserData);

  Q_UNUSED(pGpsdControl);
  Q_UNUSED(pThis);

  qDebug("GPSD running.");
}

//---------------------------------------------------------------------------
// locationOnGpsdStopped
//---------------------------------------------------------------------------
void Location::locationOnGpsdStopped (LocationGPSDControl* pGpsdControl, gpointer pUserData)
{
  Location* pThis = reinterpret_cast<Location*>(pUserData);

  Q_UNUSED(pGpsdControl);
  Q_UNUSED(pThis);

  qDebug("GPSD STOPPED !");
}

//---------------------------------------------------------------------------
// locationOnGpsdErrorVerbose
//---------------------------------------------------------------------------
void Location::locationOnGpsdErrorVerbose (LocationGPSDControl* pGpsdControl, LocationGPSDControlError eError, gpointer pUserData)
{
  Location* pThis = reinterpret_cast<Location*>(pUserData);

  Q_UNUSED(pGpsdControl);
  Q_UNUSED(pThis);

  switch (eError)
  {
    case LOCATION_ERROR_USER_REJECTED_DIALOG :
      qDebug("User rejected location enabling dialog.");
      break;

    case LOCATION_ERROR_USER_REJECTED_SETTINGS :
      qDebug("User changed settings which disabled locationing.");
      break;

    case LOCATION_ERROR_BT_GPS_NOT_AVAILABLE :
      qDebug("Problems using BT GPS.");
      break;

    case LOCATION_ERROR_METHOD_NOT_ALLOWED_IN_OFFLINE_MODE :
      qDebug("Method unavailable in offline mode.");
      break;

    case LOCATION_ERROR_SYSTEM :
      qDebug("System error ! SIM card is missing ?");
      break;

    default :
      qDebug("Unknown GPSD error %d !", eError);
      break;
  }
}
