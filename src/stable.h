//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:21
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRECORD_STABLE_H__
#define __GPSRECORD_STABLE_H__


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
extern "C" {
#include <glib.h>
}

// maemo - location
//
// http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Connectivity_Components/Using_Location_API
// http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDControl.html
// http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html
extern "C" {
#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#include <location/location-distance-utils.h>
#include <location/location-misc.h>
}

// maemo - qt
#include <QObject>
#include <QSettings>
#include <QtGui>


//---------------------------------------------------------------------------
// Local Headers
//---------------------------------------------------------------------------
#include "Util.h"
#include "Location.h"
#include "WndMain.h"

#include "App.h"


#endif // #ifndef __GPSRECORD_STABLE_H__
