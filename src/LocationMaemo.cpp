//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex+gpsrecorder [AT] gmail [DOT] com>
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
  m_eGpsdControlInterval = LOCATION_INTERVAL_1S;
  m_eGpsdControlMethod   = LOCATION_METHOD_CWP | LOCATION_METHOD_GNSS;

  // init location lib
  {
    m_pGpsDevice = (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);
    Q_CHECK_PTR(m_pGpsDevice);

    m_pGpsdControl = location_gpsd_control_get_default();
    Q_CHECK_PTR(m_pGpsdControl);

    g_object_set(G_OBJECT(m_pGpsdControl), "preferred-interval", m_eGpsdControlInterval, NULL);
    g_object_set(G_OBJECT(m_pGpsdControl), "preferred-method",   m_eGpsdControlMethod, NULL);

    m_auiSigHdlGpsDevice[0] = g_signal_connect(G_OBJECT(m_pGpsDevice), "connected",    G_CALLBACK(LocationMaemo::locationOnDevConnected), this);
    m_auiSigHdlGpsDevice[1] = g_signal_connect(G_OBJECT(m_pGpsDevice), "disconnected", G_CALLBACK(LocationMaemo::locationOnDevDisconnected), this);
    m_auiSigHdlGpsDevice[2] = g_signal_connect(G_OBJECT(m_pGpsDevice), "changed",      G_CALLBACK(LocationMaemo::locationOnDevChanged), this);

    m_auiSigHdlGpsdControl[0] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_running",  G_CALLBACK(LocationMaemo::locationOnGpsdRunning), this);
    m_auiSigHdlGpsdControl[1] = g_signal_connect(G_OBJECT(m_pGpsdControl), "gpsd_stopped",  G_CALLBACK(LocationMaemo::locationOnGpsdStopped), this);
    m_auiSigHdlGpsdControl[2] = g_signal_connect(G_OBJECT(m_pGpsdControl), "error-verbose", G_CALLBACK(LocationMaemo::locationOnGpsdErrorVerbose), this);
  }

  // use default fix step value from Location constructor
  this->setFixStep(m_uiFixStep);
}

//---------------------------------------------------------------------------
// ~LocationMaemo
//---------------------------------------------------------------------------
LocationMaemo::~LocationMaemo (void)
{
  // uninit location lib
  {
    this->stop();

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
// setAssisted
//---------------------------------------------------------------------------
bool LocationMaemo::setAssisted (bool bAssisted)
{
  bool bWasStarted = this->isStarted();
  LocationGPSDControlMethod eNewMethod =
    bAssisted ?
    (LOCATION_METHOD_CWP | LOCATION_METHOD_GNSS | LOCATION_METHOD_AGNSS) :
    (LOCATION_METHOD_CWP | LOCATION_METHOD_GNSS);

  if (eNewMethod == m_eGpsdControlMethod)
  {
    m_bAssisted = bAssisted;
    return true;
  }

  if (this->isStarted())
    this->stop();

  g_object_set(G_OBJECT(m_pGpsdControl), "preferred-method", eNewMethod, NULL);
  m_eGpsdControlMethod = eNewMethod;
  m_bAssisted = bAssisted;

  if (bWasStarted)
    this->start();

  return true;
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

  m_bAcquiring = true;
  m_bStarted   = true;

  location_gpsd_control_start(m_pGpsdControl);
}

//---------------------------------------------------------------------------
// stop
//---------------------------------------------------------------------------
void LocationMaemo::stop (void)
{
  if (!m_bStarted)
    return;

  location_gpsd_control_stop(m_pGpsdControl);
  m_bStarted   = false;
  m_bAcquiring = false;
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
// fixSetup
//---------------------------------------------------------------------------
void LocationMaemo::fixSetup (LocationFixContainer& fixCont, const LocationGPSDevice& gpsdev)
{
  LocationFix* pFix = 0;

  // ensure we really got a fix
  Q_ASSERT(gpsdev.fix);
  if (!gpsdev.fix)
    qFatal("fixSetup() got null fix !");

  // be sure we can safely cast the satellites count
  if (gpsdev.satellites)
  {
    Q_ASSERT(gpsdev.satellites->len <= 0xFF);
    if (gpsdev.satellites->len > 0xFF)
      qFatal("Got LocationGPSDevice fix with %d satellites !", gpsdev.satellites->len);
  }

  // prepare destination buffer of this fix
  pFix = fixCont.prepare(gpsdev.satellites ? (quint8)gpsdev.satellites->len : 0);

  // convert fix
  {
    const LocationGPSDeviceFix& devfix = *gpsdev.fix;

    // gps fix info
    pFix->cFixMode   = LocationMaemo::fixConvertFixMode(devfix.mode);
    pFix->wFixFields = quint16(devfix.fields);
    pFix->uiTime     = quint32(_abs(devfix.time));
    pFix->uiTimeEP   = quint32(_abs(devfix.ept));
    pFix->iLat       = qint32(devfix.latitude * (double)LOCFIX_MULTIPLIER_LATLONG);
    pFix->iLong      = qint32(devfix.longitude * (double)LOCFIX_MULTIPLIER_LATLONG);
    pFix->uiHorizEP  = quint32(_abs(devfix.eph));
    pFix->iAlt       = qint32(devfix.altitude);
    pFix->uiAltEP    = quint32(_abs(devfix.epv));
    pFix->uiTrack    = quint16(_abs(devfix.track) * (double)LOCFIX_MULTIPLIER_TRACK);
    pFix->uiTrackEP  = quint16(_abs(devfix.epd) * (double)LOCFIX_MULTIPLIER_TRACK);
    pFix->uiSpeed    = quint32(_abs(devfix.speed) * (double)LOCFIX_MULTIPLIER_SPEED);
    pFix->uiSpeedEP  = quint32(_abs(devfix.eps) *  (double)LOCFIX_MULTIPLIER_SPEED);
    pFix->iClimb     = qint16(devfix.climb * (double)LOCFIX_MULTIPLIER_CLIMB);
    pFix->uiClimbEP  = quint16(_abs(devfix.epc) * (double)LOCFIX_MULTIPLIER_CLIMB);

    // cell info - gsm
    if (gpsdev.cell_info && (gpsdev.cell_info->flags & LOCATION_CELL_INFO_GSM_CELL_INFO_SET) != 0)
    {
      pFix->sGSM.bSetup   = 1;
      pFix->sGSM.uiMCC    = gpsdev.cell_info->gsm_cell_info.mcc;
      pFix->sGSM.uiMNC    = gpsdev.cell_info->gsm_cell_info.mnc;
      pFix->sGSM.uiLAC    = gpsdev.cell_info->gsm_cell_info.lac;
      pFix->sGSM.uiCellId = gpsdev.cell_info->gsm_cell_info.cell_id;
    }
    else
    {
      memset(&pFix->sGSM, 0, sizeof(pFix->sGSM));
    }

    // cell info - wcdma
    if (gpsdev.cell_info && (gpsdev.cell_info->flags & LOCATION_CELL_INFO_WCDMA_CELL_INFO_SET) != 0)
    {
      pFix->sWCDMA.bSetup = 1;
      pFix->sWCDMA.uiMCC  = gpsdev.cell_info->wcdma_cell_info.mcc;
      pFix->sWCDMA.uiMNC  = gpsdev.cell_info->wcdma_cell_info.mnc;
      pFix->sWCDMA.uiUCID = gpsdev.cell_info->wcdma_cell_info.ucid;
    }
    else
    {
      memset(&pFix->sWCDMA, 0, sizeof(pFix->sWCDMA));
    }

    // satellites count
    pFix->cSatCount = quint8(gpsdev.satellites_in_view);
    pFix->cSatUse   = quint8(gpsdev.satellites_in_use);
    Q_ASSERT((pFix->cSatCount <= 0) || (gpsdev.satellites && gpsdev.satellites->len <= 0xFF && pFix->cSatCount == quint8(gpsdev.satellites->len)));

    // satellites
    if (gpsdev.satellites)
    {
      for (quint8 cIdx = 0; cIdx < pFix->cSatCount; ++cIdx)
      {
        LocationFixSat* pFixSat = &pFix->aFixSat[cIdx];
        LocationGPSDeviceSatellite* pDevSat = (LocationGPSDeviceSatellite*)g_ptr_array_index(gpsdev.satellites, (uint)cIdx);

        pFixSat->bInUse          = pDevSat->in_use ? 1 : 0;
        pFixSat->iPRN            = pDevSat->prn;
        pFixSat->cElevation      = fxuint8(pDevSat->elevation);
        pFixSat->wAzimuth        = fxuint16(pDevSat->azimuth);
        pFixSat->cSignalStrength = fxuint8(pDevSat->signal_strength);
      }
    }

    // consolidate fix structure
    fixCont.finalize();
  }
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
  bool bAccurate;

  // we test the result of isStarted() because sometimes we are called right
  // after stop() has been called and processed
  if (!pThis->isStarted() || !pGpsDevice || !pGpsDevice->fix)
    return;

  if (pGpsDevice->fix->mode >= LOCATION_GPS_DEVICE_MODE_2D &&
     (pGpsDevice->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET) != 0 &&
      pGpsDevice->fix->eph < 9000.0) // 90m
  {
    bAccurate = true;

    LocationMaemo::fixSetup(pThis->m_FixCont, *pGpsDevice);

    if (pThis->m_bAcquiring)
    {
      pThis->m_bAcquiring = false;
      emit pThis->sigLocationFixGain(pThis, &pThis->m_FixCont);
    }
  }
  else
  {
    bAccurate = false;

    if (!pThis->m_bAcquiring)
    {
      pThis->m_bAcquiring = true;
      emit pThis->sigLocationFixLost(pThis, &pThis->m_FixCont);
    }

    LocationMaemo::fixSetup(pThis->m_FixCont, *pGpsDevice);
  }

  emit pThis->sigLocationFix(pThis, &pThis->m_FixCont, bAccurate);
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
      qWarning("User rejected location enabling dialog.");
      break;

    case LOCATION_ERROR_USER_REJECTED_SETTINGS :
      qWarning("User changed settings which disabled locationing.");
      break;

    case LOCATION_ERROR_BT_GPS_NOT_AVAILABLE :
      qWarning("Problems using BT GPS.");
      break;

    case LOCATION_ERROR_METHOD_NOT_ALLOWED_IN_OFFLINE_MODE :
      qWarning("Method unavailable in offline mode.");
      break;

    case LOCATION_ERROR_SYSTEM :
      qWarning("System error ! SIM card is missing ?");
      break;

    default :
      qWarning("Unknown GPSD error %d !", eError);
      break;
  }
}
