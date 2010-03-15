//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:21
//
// $Id$
//
//***************************************************************************

#ifndef __MAIN_H__
#define __MAIN_H__


//---------------------------------------------------------------------------
// System Headers
//---------------------------------------------------------------------------
// standard headers
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// maemo - glib
//
// get glib include paths :
// * mad pkg-config --cflags glib-2.0
// * mad pkg-config --libs glib-2.0
//
// INCLUDEPATH += /usr/include/glib-2.0 /usr/lib/glib-2.0/include
// LIBS        += -lglib-2.0 -llocation
#include <glib.h>

// maemo - location
//
// http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDControl.html
// http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html
#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#include <location/location-distance-utils.h>
#include <location/location-misc.h>


//---------------------------------------------------------------------------
// Base Types
//---------------------------------------------------------------------------
#ifndef NULL
#define NULL  0
#endif

#if !defined(_STDINT_H) && !defined(__BIT_TYPES_DEFINED__)
  typedef unsigned char      uint8_t;   // must always be 8bits wide
  typedef unsigned short     uint16_t;  // must always be 16bits wide
  typedef unsigned int       uint32_t;  // must always be 32bits wide
  typedef unsigned long long uint64_t;  // must always be 64bits wide
#endif

typedef unsigned int  uint_t;
typedef unsigned char uchar_t;


//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
// branding
#define APP_NAME  "gpsrecord"

// default directory for output files
#define APP_DEFAULT_OUTPUT_DIRECTORY  "/home/user/MyDocs"


//---------------------------------------------------------------------------
// Local Headers
//---------------------------------------------------------------------------
#include "Util.h"
#include "Location.h"


#endif // #ifndef __MAIN_H__
