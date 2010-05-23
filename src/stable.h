//***************************************************************************
//
// GPS Recorder
// Show, log and convert location data from Maemo's GPS subsystem.
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
// Created On : 2010-03-14 18:43:21
//
// $Id$
//
//***************************************************************************

#ifndef __GPSRECORDER_STABLE_H__
#define __GPSRECORDER_STABLE_H__


//---------------------------------------------------------------------------
// System Headers
//---------------------------------------------------------------------------
// standard headers
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pwd.h>
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
#ifdef __cplusplus
extern "C" {
#endif
#include <glib.h>
#ifdef __cplusplus
}
#endif

// maemo - location
//
// http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Connectivity_Components/Using_Location_API
// http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDControl.html
// http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html
#ifdef __cplusplus
extern "C" {
#endif
#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#include <location/location-distance-utils.h>
#include <location/location-misc.h>
#ifdef __cplusplus
}
#endif

// maemo - clockd
//
// http://maemo.org/api_refs/5.0/5.0-final/libtime/libtime_8h.html
#ifdef __cplusplus
extern "C" {
#endif
#include <clockd/libtime.h>
#ifdef __cplusplus
}
#endif

// minizip
#ifdef __cplusplus
extern "C" {
#endif
#include "minizip/zip.h"
#ifdef __cplusplus
}
#endif

// qt
#ifdef __cplusplus
#include <QObject>
#include <QSettings>
#include <QtGui>
#endif

// qt - maemo
// requires 'libqt4-maemo5-maemo5' package into the device
#ifdef __cplusplus
//#include <QMaemo5InformationBox> // qt4.6+
#endif


//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
// use this to make your code uncompilable until you remove it
#define TODO  const char TODO[-1] = "This is a TODO !"

// static assert macro (i.e.: compile-time assert)
#define MY_STATIC_ASSERT(test)  typedef char MY_CAT(_static_assert_,__LINE__)[(test) * 2 - 1]

// util macros
#define MY_VERBATIM(x)   x
#define MY_STRINGIZE(x)  MY_STRINGIZE__SUB(x)
#define MY_CAT(a,b)      MY_CAT__SUB(a,b)

// util macros subs (do not use directly)
#define MY_STRINGIZE__SUB(x)  #x
#define MY_CAT__SUB(a,b)      a##b

// debug mark
#define MY_DBGMARK  { qDebug("DBGMARK %s(%d)", __FILE__, __LINE__); }


//---------------------------------------------------------------------------
// Raw Types
//---------------------------------------------------------------------------
#ifdef __cplusplus
class ExporterSink;
#endif


//---------------------------------------------------------------------------
// Local Headers
//---------------------------------------------------------------------------
#ifdef __cplusplus

#include "Util.h"
#include "LocationFix.h"
#include "Location.h"
#include "LocationMaemo.h"
#include "GPSRFile.h"
#include "LocationGPSRFile.h"
#include "Exporter.h"
#include "ExporterSink.h"
#include "ExporterSinkCsv.h"
#include "ExporterSinkGpx.h"
#include "ExporterSinkKml.h"

#include "WndAbout.h"
#include "WndCell.h"
#include "WndConfig.h"
#include "WndConvert.h"
#include "WndSat.h"
#include "WndSpeed.h"
#include "WndMain.h"
#include "AppSettings.h"
#include "App.h"

#endif


#endif // #ifndef __GPSRECORDER_STABLE_H__
