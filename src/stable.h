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

// maemo - mce (Mode Control Entity)
//
// http://maemo.org/api_refs/5.0/beta/mce-dev/index.html
#ifdef __cplusplus
extern "C" {
#endif
#include <mce/dbus-names.h>
#include <mce/mode-names.h>
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
#include <QtDBus>
#endif

// qt - maemo specific (qt4.6+)
#ifdef __cplusplus
#include <QtMaemo5>
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
// Constants
//---------------------------------------------------------------------------
// unit system
#define UNITSYSTEM_METRIC    0
#define UNITSYSTEM_IMPERIAL  1

// horizontal speed unit
#define HORIZSPEEDUNIT_KMH    0
#define HORIZSPEEDUNIT_MPH    1
#define HORIZSPEEDUNIT_MS     2
#define HORIZSPEEDUNIT_KNOTS  3


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

#include "QBattery.h"
#include "QLabeledLineEdit.h"
#include "QMaemoComboBox.h"
#include "QMaemoGroupBox.h"
#include "QMeansOfTransport.h"

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
#include "ExporterSinkTxt.h"

#include "WndBase.h"
#include "WndAbout.h"
#include "WndCell.h"
#include "WndConfig.h"
#include "WndConvert.h"
#include "WndSat.h"
#include "WndSpeed.h"
#include "WndStart.h"
#include "WndMain.h"
#include "WndMeansOfTransport.h"

#include "AppSettings.h"
#include "App.h"

#endif


#endif // #ifndef __GPSRECORDER_STABLE_H__
