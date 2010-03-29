//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-29 13:21:34
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRECORD_LOCATIONFIX_H__
#define __GPSRECORD_LOCATIONFIX_H__


// base types
typedef signed   char      fxint8;
typedef unsigned char      fxuint8;
typedef signed   short     fxint16;
typedef unsigned short     fxuint16;
typedef signed   int       fxint32;
typedef unsigned int       fxuint32;
//typedef signed   long long fxint64;
//typedef unsigned long long fxuint64;



// multipliers used to convert LocationGPSDeviceFix -> Fix
enum
{
  LOCFIX_MULTIPLIER_LATLONG = 1000000,
  LOCFIX_MULTIPLIER_TRACK   = 100,
  LOCFIX_MULTIPLIER_SPEED   = 1000, // km/h -> m/h
  LOCFIX_MULTIPLIER_CLIMB   = 100,  // m/s -> cm/s
};

// fix mode
enum
{
  FIXMODE_NOTSEEN = 0,
  FIXMODE_NOFIX   = 1,
  FIXMODE_2D      = 2,
  FIXMODE_3D      = 3,
};

// fix fields flags
enum
{
  FIXFIELD_NONE    = 0,
  FIXFIELD_ALT     = (1 << 0),
  FIXFIELD_SPEED   = (1 << 1),
  FIXFIELD_TRACK   = (1 << 2),
  FIXFIELD_CLIMB   = (1 << 3),
  FIXFIELD_LATLONG = (1 << 4),
  FIXFIELD_TIME    = (1 << 5),
  FIXFIELD_ALL =
    FIXFIELD_ALT |
    FIXFIELD_SPEED |
    FIXFIELD_TRACK |
    FIXFIELD_CLIMB |
    FIXFIELD_LATLONG |
    FIXFIELD_TIME,
};

// GSM cell info
struct LocationFixCellInfoGsm
{
  fxuint8  bSetup;
  fxuint16 uiMCC;    // Mobile Country Code
  fxuint16 uiMNC;    // Mobile Network Code
  fxuint16 uiLAC;    // Location Area Code
  fxuint16 uiCellId; // Cell ID
};

// WCDMA cell info
struct LocationFixCellInfoWcdma
{
  fxuint8  bSetup;
  fxuint16 uiMCC;  // Mobile Country Code
  fxuint16 uiMNC;  // Mobile Network Code
  fxuint16 uiUCID; // UC ID
};

// satellite info
struct LocationFixSat
{
  fxuint8 bInUse;
  fxint32 iPRN;            // id
  fxint32 iElevation;
  fxint32 iAzimuth;
  fxint32 iSignalStrength; // signal/noise ratio
};

// the location fix itself
struct LocationFix
{
  void             clear             (void);
  void             clearTrailingZone (void);
  fxuint32         storageSize       (void);
  LocationFixSat*  getSat            (fxuint8 uiSatIndex);


  // total size of this Fix, including trailing satellites structures
  fxuint32 uiSize; // CAUTION : must remain the first member !

  // gps fix info
  fxuint8  cFixMode;
  fxuint16 wFixFields;
  fxuint32 uiTime;
  fxuint32 uiTimeEP;   // estimated time uncertainty
  fxint32  iLat;       // latitude ; quint32(lat * LOCFIX_MULTIPLIER_LATLONG)
  fxint32  iLong;      // longitude ; quint32(long * LOCFIX_MULTIPLIER_LATLONG)
  fxuint32 uiHorizEP;  // horizontal position uncertainty (cm) ; quint32(eph)
  fxint32  iAlt;       // altitude (m) ; qint32(altm)
  fxuint32 uiAltEP;    // vertical position uncertainty (m) ; quint32(epv)
  fxuint16 uiTrack;    // heading in [0;359] degrees * LOCFIX_MULTIPLIER_TRACK ; quint16(track * LOCFIX_MULTIPLIER_TRACK)
  fxuint16 uiTrackEP;  // track uncertainty in degrees * LOCFIX_MULTIPLIER_TRACK ; quint16(epd * LOCFIX_MULTIPLIER_TRACK)
  fxuint32 uiSpeed;    // speed in m/h ; quint32(speed * LOCFIX_MULTIPLIER_SPEED)
  fxuint32 uiSpeedEP;  // speed uncertainty in m/h ; quint32(eps *  LOCFIX_MULTIPLIER_SPEED)
  fxint16  iClimb;     // rate of climb in [+-]cm/s ; qint16(climb * LOCFIX_MULTIPLIER_CLIMB)
  fxuint16 uiClimbEP;  // climb uncertainty in cm/s ; quint16(epc * LOCFIX_MULTIPLIER_CLIMB)

  // cell info
  LocationFixCellInfoGsm   sGSM;
  LocationFixCellInfoWcdma sWCDMA;

  // satellites count
  fxuint8 cSatCount;
  fxuint8 cSatView;
  fxuint8 cSatUse;

  // ... satellites (cSatCount * FixSat)
};


#endif // #ifndef __GPSRECORD_LOCATIONFIX_H__
