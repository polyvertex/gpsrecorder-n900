//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:29
//
// $Id$
//
//***************************************************************************

#include "stable.h"


// if at least one of those values has changed,
// we'll have to increment storage format version !
MY_STATIC_ASSERT(FIXFIELD_NONE    == 0);        // LOCATION_GPS_DEVICE_NONE
MY_STATIC_ASSERT(FIXFIELD_ALT     == (1 << 0)); // LOCATION_GPS_DEVICE_ALTITUDE_SET
MY_STATIC_ASSERT(FIXFIELD_SPEED   == (1 << 1)); // LOCATION_GPS_DEVICE_SPEED_SET
MY_STATIC_ASSERT(FIXFIELD_TRACK   == (1 << 2)); // LOCATION_GPS_DEVICE_TRACK_SET
MY_STATIC_ASSERT(FIXFIELD_CLIMB   == (1 << 3)); // LOCATION_GPS_DEVICE_CLIMB_SET
MY_STATIC_ASSERT(FIXFIELD_LATLONG == (1 << 4)); // LOCATION_GPS_DEVICE_LATLONG_SET
MY_STATIC_ASSERT(FIXFIELD_TIME    == (1 << 5)); // LOCATION_GPS_DEVICE_TIME_SET
MY_STATIC_ASSERT(FIXFIELD_ALL     == FIXFIELD_ALT|FIXFIELD_SPEED|FIXFIELD_TRACK|FIXFIELD_CLIMB|FIXFIELD_LATLONG|FIXFIELD_TIME);
//
MY_STATIC_ASSERT(FIXMODE_NOTSEEN == 0); // LOCATION_GPS_DEVICE_MODE_NOT_SEEN
MY_STATIC_ASSERT(FIXMODE_NOFIX   == 1); // LOCATION_GPS_DEVICE_MODE_NO_FIX
MY_STATIC_ASSERT(FIXMODE_2D      == 2); // LOCATION_GPS_DEVICE_MODE_2D
MY_STATIC_ASSERT(FIXMODE_3D      == 3); // LOCATION_GPS_DEVICE_MODE_3D


//---------------------------------------------------------------------------
// _abs
//---------------------------------------------------------------------------
MY_STATIC_ASSERT(sizeof(qint64) == sizeof(double));
inline static double _abs (double d)
{
  qint64 i = (qint64&)d & 0x7FFFFFFFFFFFFFFFllu;
  return (double&)i;
}




//---------------------------------------------------------------------------
// LocationMaemo
//---------------------------------------------------------------------------
LocationMaemo::LocationMaemo (void)
: Location(0)
{
  m_eGpsdControlInterval = LOCATION_INTERVAL_5S;

  // init location lib
  {
    m_pGpsDevice   = (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);
    m_pGpsdControl = location_gpsd_control_get_default();

    g_object_set(G_OBJECT(m_pGpsdControl), "preferred-interval", m_eGpsdControlInterval, NULL);
    g_object_set(G_OBJECT(m_pGpsdControl), "preferred-method",   LOCATION_METHOD_AGNSS, NULL);

    m_auiSigHdlGpsDevice[0] = g_signal_connect(G_OBJECT(m_pGpsDevice), "connected",    G_CALLBACK(LocationMaemo::locationOnDevConnected), this);
    m_auiSigHdlGpsDevice[1] = g_signal_connect(G_OBJECT(m_pGpsDevice), "disconnected", G_CALLBACK(LocationMaemo::locationOnDevDisconnected), this);
    m_auiSigHdlGpsDevice[2] = g_signal_connect(G_OBJECT(m_pGpsDevice), "changed",      G_CALLBACK(LocationMaemo::locationOnDevChanged), this);

	  m_auiSigHdlGpsdControl[0] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_running",  G_CALLBACK(LocationMaemo::locationOnGpsdRunning), this);
	  m_auiSigHdlGpsdControl[1] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_stopped",  G_CALLBACK(LocationMaemo::locationOnGpsdStopped), this);
	  m_auiSigHdlGpsdControl[2] = g_signal_connect(G_OBJECT(m_pGpsdControl), "error-verbose", G_CALLBACK(LocationMaemo::locationOnGpsdErrorVerbose), this);
  }

  // use default fix step value from Location constructor
  this->setFixStep(m_uiFixTime);
}

//---------------------------------------------------------------------------
// ~LocationMaemo
//---------------------------------------------------------------------------
LocationMaemo::~LocationMaemo (void)
{
  // uninit location lib
  {
    location_gpsd_control_stop(m_pGpsdControl);

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
// setFixStep
//---------------------------------------------------------------------------
bool LocationMaemo::setFixStep (uint uiNewFixStepSeconds)
{
  bool bWasStarted = this->isStarted();
  LocationGPSDControlInterval eGpsdNewInterval;

  switch (uiNewFixStepSeconds)
  {
    case 1 :
      eGpsdNewInterval = LOCATION_INTERVAL_1S;
      break;
    case 2 :
      eGpsdNewInterval = LOCATION_INTERVAL_2S;
      break;
    case 5 :
      eGpsdNewInterval = LOCATION_INTERVAL_5S;
      break;
    case 10 :
      eGpsdNewInterval = LOCATION_INTERVAL_10S;
      break;
    case 20 :
      eGpsdNewInterval = LOCATION_INTERVAL_20S;
      break;
    case 30 :
      eGpsdNewInterval = LOCATION_INTERVAL_30S;
      break;
    case 60 :
      eGpsdNewInterval = LOCATION_INTERVAL_60S;
      break;
    case 120 :
      eGpsdNewInterval = LOCATION_INTERVAL_120S;
      break;
    default :
      qCritical("Unsupported fix step interval (%u seconds) !", uiNewFixStepSeconds);
      return false;
  }

  if (eGpsdNewInterval == m_eGpsdControlInterval)
  {
    m_uiFixStep = uiNewFixStepSeconds;
    return true;
  }

  if (this->isStarted())
    this->stop();

  g_object_set(G_OBJECT(m_pGpsdControl), "preferred-interval", eGpsdNewInterval, NULL);
  m_eGpsdControlInterval = eGpsdNewInterval;
  m_uiFixStep = uiNewFixStepSeconds;

  if (bWasStarted)
    this->start();

  return true;
}

//---------------------------------------------------------------------------
// resetLastFix
//---------------------------------------------------------------------------
void LocationMaemo::resetLastFix (void)
{
  //if (!this->isStarted())
    location_gps_device_reset_last_known(m_pGpsDevice);

  Location::resetLastFix();
}

//---------------------------------------------------------------------------
// start
//---------------------------------------------------------------------------
void LocationMaemo::start (void)
{
  if (m_bStarted)
    return;

  location_gpsd_control_start(m_pGpsdControl);
  m_bStarted = true;
}

//---------------------------------------------------------------------------
// stop
//---------------------------------------------------------------------------
void LocationMaemo::stop (void)
{
  if (!m_bStarted)
    return;

  location_gpsd_control_stop(m_pGpsdControl);
  m_bStarted = false;
}




//---------------------------------------------------------------------------
// fixConvertFixMode
//---------------------------------------------------------------------------
quint8 LocationMaemo::fixConvertFixMode (LocationGPSDeviceMode eGpsDevMode)
{
  switch (eGpsDevMode)
  {
    case LOCATION_GPS_DEVICE_MODE_NOT_SEEN :
      return FIXMODE_NOTSEEN;
    case LOCATION_GPS_DEVICE_MODE_NO_FIX :
      return FIXMODE_NOFIX;
    case LOCATION_GPS_DEVICE_MODE_2D :
      return FIXMODE_2D;
    case LOCATION_GPS_DEVICE_MODE_3D :
      return FIXMODE_3D;

    default :
      Q_ASSERT(0);
      return FIXMODE_NOFIX;
  }
}

//---------------------------------------------------------------------------
// fixNeededSize
//---------------------------------------------------------------------------
quint32 LocationMaemo::fixNeededSize (const LocationGPSDevice& gpsdev)
{
  quint32 uiSatsCount = (gpsdev.satellites->len > 0xFF) ? 0xFF : gpsdev.satellites->len;
  return sizeof(LocationFix) + uiSatsCount * sizeof(LocationFixSat);
}

//---------------------------------------------------------------------------
// fixSetup
//---------------------------------------------------------------------------
LocationFix* LocationMaemo::fixSetup (const LocationGPSDevice& gpsdev, LocationFix* pOutFix/*=0*/)
{
  quint32 dwDesiredSize;

  if (!gpsdev.fix)
    return pOutFix;
  dwDesiredSize = LocationMaemo::fixNeededSize(gpsdev);

  // allocate enough memory to store this fix
  if (!pOutFix)
  {
    pOutFix = (LocationFix*)malloc(dwDesiredSize);
    Q_CHECK_PTR(pOutFix);
    memset(pOutFix, 0, dwDesiredSize);

    pOutFix->uiSize = dwDesiredSize;
  }
  else if (pOutFix && pOutFix->uiSize < dwDesiredSize)
  {
    quint32 dwOldSize = pOutFix->uiSize;

    pOutFix = (LocationFix*)realloc(pOutFix, dwDesiredSize);
    Q_CHECK_PTR(pOutFix);
    pOutFix->uiSize = dwDesiredSize;

    // zeroize added trailing zone
    memset((char*)pOutFix + dwOldSize, 0, dwDesiredSize - dwOldSize);
  }

  // convert fix
  {
    const LocationGPSDeviceFix& devfix = *gpsdev.fix;

    // gps fix info
    pOutFix->cFixMode   = LocationMaemo::fixConvertFixMode(devfix.mode);
    pOutFix->wFixFields = quint16(devfix.fields);
    pOutFix->uiTime     = quint32(_abs(devfix.time));
    pOutFix->uiTimeEP   = quint32(_abs(devfix.ept));
    pOutFix->iLat       = qint32(devfix.latitude * (double)LOCFIX_MULTIPLIER_LATLONG);
    pOutFix->iLong      = qint32(devfix.longitude * (double)LOCFIX_MULTIPLIER_LATLONG);
    pOutFix->uiHorizEP  = quint32(_abs(devfix.eph));
    pOutFix->iAlt       = qint32(devfix.altitude);
    pOutFix->uiAltEP    = quint32(_abs(devfix.epv));
    pOutFix->uiTrack    = quint16(_abs(devfix.track) * (double)LOCFIX_MULTIPLIER_TRACK);
    pOutFix->uiTrackEP  = quint16(_abs(devfix.epd) * (double)LOCFIX_MULTIPLIER_TRACK);
    pOutFix->uiSpeed    = quint32(_abs(devfix.speed) * (double)LOCFIX_MULTIPLIER_SPEED);
    pOutFix->uiSpeedEP  = quint32(_abs(devfix.eps) *  (double)LOCFIX_MULTIPLIER_SPEED);
    pOutFix->iClimb     = qint16(devfix.climb * (double)LOCFIX_MULTIPLIER_CLIMB);
    pOutFix->uiClimbEP  = quint16(_abs(devfix.epc) * (double)LOCFIX_MULTIPLIER_CLIMB);

    // cell info - gsm
    if ((gpsdev.cell_info->flags & LOCATION_CELL_INFO_GSM_CELL_INFO_SET) != 0)
    {
      pOutFix->sGSM.bSetup   = 1;
      pOutFix->sGSM.uiMCC    = gpsdev.cell_info->gsm_cell_info.mcc;
      pOutFix->sGSM.uiMNC    = gpsdev.cell_info->gsm_cell_info.mnc;
      pOutFix->sGSM.uiLAC    = gpsdev.cell_info->gsm_cell_info.lac;
      pOutFix->sGSM.uiCellId = gpsdev.cell_info->gsm_cell_info.cell_id;
    }
    else
    {
      memset(&pOutFix->sGSM, 0, sizeof(pOutFix->sGSM));
    }

    // cell info - wcdma
    if ((gpsdev.cell_info->flags & LOCATION_CELL_INFO_WCDMA_CELL_INFO_SET) != 0)
    {
      pOutFix->sWCDMA.bSetup = 1;
      pOutFix->sWCDMA.uiMCC  = gpsdev.cell_info->wcdma_cell_info.mcc;
      pOutFix->sWCDMA.uiMNC  = gpsdev.cell_info->wcdma_cell_info.mnc;
      pOutFix->sWCDMA.uiUCID = gpsdev.cell_info->wcdma_cell_info.ucid;
    }
    else
    {
      memset(&pOutFix->sWCDMA, 0, sizeof(pOutFix->sWCDMA));
    }

    // satellites count
    pOutFix->cSatCount = (gpsdev.satellites->len > 0xFF) ? 0xFF : gpsdev.satellites->len;
    pOutFix->cSatView  = quint8(gpsdev.satellites_in_view);
    pOutFix->cSatUse   = quint8(gpsdev.satellites_in_use);

    // satellites
    for (quint8 cIdx = 0; cIdx < pOutFix->cSatCount; ++cIdx)
    {
      LocationFixSat* pOutFixSat = pOutFix->getSat(cIdx);

      if (!pOutFixSat)
      {
        // we should never here !
        pOutFix->cSatCount = cIdx;
        break;
      }

      LocationGPSDeviceSatellite* pDevSat = (LocationGPSDeviceSatellite*)g_ptr_array_index(gpsdev.satellites, (uint)cIdx);

      pOutFixSat->bInUse          = pDevSat->in_use ? 1 : 0;
      pOutFixSat->iPRN            = pDevSat->prn;
      pOutFixSat->iElevation      = pDevSat->elevation;
      pOutFixSat->iAzimuth        = pDevSat->azimuth;
      pOutFixSat->iSignalStrength = pDevSat->signal_strength;
    }

    // zeroize trailing memory zone
    pOutFix->clearTrailingZone();
  }

  return pOutFix;
}




//---------------------------------------------------------------------------
// locationOnDevConnected
//---------------------------------------------------------------------------
void LocationMaemo::locationOnDevConnected (LocationGPSDevice* pGpsDevice, gpointer pUserData)
{
  LocationMaemo* pThis = reinterpret_cast<LocationMaemo*>(pUserData);

  Q_UNUSED(pGpsDevice);
  Q_UNUSED(pThis);

  qDebug("GPS device connected.");
}

//---------------------------------------------------------------------------
// locationOnDevDisconnected
//---------------------------------------------------------------------------
void LocationMaemo::locationOnDevDisconnected (LocationGPSDevice* pGpsDevice, gpointer pUserData)
{
  LocationMaemo* pThis = reinterpret_cast<LocationMaemo*>(pUserData);

  Q_UNUSED(pGpsDevice);
  Q_UNUSED(pThis);

  qDebug("GPS device DISCONNECTED !");
}

//---------------------------------------------------------------------------
// locationOnDevChanged
//---------------------------------------------------------------------------
void LocationMaemo::locationOnDevChanged (LocationGPSDevice* pGpsDevice, gpointer pUserData)
{
  LocationMaemo* pThis = reinterpret_cast<LocationMaemo*>(pUserData);

  if (pGpsDevice && pGpsDevice->fix)
  {
    pThis->m_uiFixTime = time(0);
    pThis->m_pFix      = LocationMaemo::fixSetup(*pGpsDevice, pThis->m_pFix);

    if (pThis->m_pFix)
      emit pThis->sigGotLocationFix(pThis, pThis->m_pFix);
    else
      pThis->m_uiFixTime = 0;
  }
}

//---------------------------------------------------------------------------
// locationOnGpsdRunning
//---------------------------------------------------------------------------
void LocationMaemo::locationOnGpsdRunning (LocationGPSDControl* pGpsdControl, gpointer pUserData)
{
  LocationMaemo* pThis = reinterpret_cast<LocationMaemo*>(pUserData);

  Q_UNUSED(pGpsdControl);
  Q_UNUSED(pThis);

  qDebug("GPSD running.");
}

//---------------------------------------------------------------------------
// locationOnGpsdStopped
//---------------------------------------------------------------------------
void LocationMaemo::locationOnGpsdStopped (LocationGPSDControl* pGpsdControl, gpointer pUserData)
{
  LocationMaemo* pThis = reinterpret_cast<LocationMaemo*>(pUserData);

  Q_UNUSED(pGpsdControl);
  Q_UNUSED(pThis);

  qDebug("GPSD STOPPED !");
}

//---------------------------------------------------------------------------
// locationOnGpsdErrorVerbose
//---------------------------------------------------------------------------
void LocationMaemo::locationOnGpsdErrorVerbose (LocationGPSDControl* pGpsdControl, LocationGPSDControlError eError, gpointer pUserData)
{
  LocationMaemo* pThis = reinterpret_cast<LocationMaemo*>(pUserData);

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
