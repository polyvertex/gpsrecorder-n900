#****************************************************************************
#
# GPS Recorder
# Show, log and convert location data from Maemo's GPS subsystem.
#
# Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Id$
#
#****************************************************************************

TARGET = gpsrecorder

TEMPLATE = app

CONFIG += warn_off precompile_header release
#CONFIG += silent
#CONFIG += debug
CONFIG(debug, debug|release) {
  #TARGET = $$join(TARGET,,,-debug)
  message(This is a DEBUG build !)
}

QT  = core gui
#QT += maemo5

INCLUDEPATH += /usr/include/glib-2.0 /usr/lib/glib-2.0/include
LIBS        += -lrt -lz -lglib-2.0 -llocation -ltime
DEFINES     +=

QMAKE_CFLAGS   +=
QMAKE_CXXFLAGS += -fno-strict-aliasing -Wall -Werror

QMAKE_POST_LINK = strip --strip-all $(TARGET)

PRECOMPILED_HEADER = src/stable.h

HEADERS += \
  src/minizip/crypt.h \
  src/minizip/ioapi.h \
  src/minizip/zip.h \
  src/App.h \
  src/AppSettings.h \
  src/Exporter.h \
  src/ExporterSink.h \
  src/ExporterSinkCsv.h \
  src/ExporterSinkGpx.h \
  src/ExporterSinkKml.h \
  src/GPSRFile.h \
  src/Location.h \
  src/LocationFix.h \
  src/LocationGPSRFile.h \
  src/LocationMaemo.h \
  src/Util.h \
  src/WndAbout.h \
  src/WndBase.h \
  src/WndCell.h \
  src/WndConfig.h \
  src/WndConvert.h \
  src/WndMain.h \
  src/WndSat.h \
  src/WndSpeed.h

SOURCES += \
  src/minizip/ioapi.c \
  src/minizip/zip.c \
  src/main.cpp \
  src/App.cpp \
  src/AppSettings.cpp \
  src/Exporter.cpp \
  src/ExporterSink.cpp \
  src/ExporterSinkCsv.cpp \
  src/ExporterSinkGpx.cpp \
  src/ExporterSinkKml.cpp \
  src/GPSRFile.cpp \
  src/Location.cpp \
  src/LocationFix.cpp \
  src/LocationGPSRFile.cpp \
  src/LocationMaemo.cpp \
  src/Util.cpp \
  src/WndAbout.cpp \
  src/WndBase.cpp \
  src/WndCell.cpp \
  src/WndConfig.cpp \
  src/WndConvert.cpp \
  src/WndMain.cpp \
  src/WndSat.cpp \
  src/WndSpeed.cpp

RESOURCES += src/resources.qrc


# all generated files goes same directory
DESTDIR     = build
OBJECTS_DIR = $$DESTDIR
MOC_DIR     = $$DESTDIR
UI_DIR      = $$DESTDIR


# build revision.h header
PRE_TARGETDEPS         += version.txt
build-version.target    = version.txt
build-version.commands  = scripts/buildversion.sh
build-version.depends   = FORCE
QMAKE_EXTRA_TARGETS    += build-version


# define install target
target.path  = /usr/bin/
INSTALLS    += target

desktop.path   = /usr/share/applications/hildon
desktop.files  = data/gpsrecorder.desktop
INSTALLS      += desktop

#service.path   = /usr/share/dbus-1/services
#service.files  = data/gpsrecorder.service
#INSTALLS      += service

icon48.path   = /usr/share/icons/hicolor/48x48/apps
icon48.files  = data/48x48/gpsrecorder.png
INSTALLS     += icon48

icon64.path   = /usr/share/icons/hicolor/64x64/apps
icon64.files  = data/64x64/gpsrecorder.png
INSTALLS     += icon64


# targets for debian source and binary package creation
debian-src.commands  = dpkg-buildpackage -S -r -us -uc -d -i -I.svn
debian-bin.commands  = dpkg-buildpackage -b -r -uc -d
debian-all.depends   = debian-src debian-bin
QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin


# clean all but Makefile
compiler_clean.commands  = -$(DEL_FILE) $(TARGET) version.txt src/version.h ; $(DEL_FILE) -rf $$DESTDIR
QMAKE_EXTRA_TARGETS     +=  compiler_clean
