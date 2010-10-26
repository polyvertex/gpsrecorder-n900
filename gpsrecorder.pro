#****************************************************************************
#
# GPS Recorder
# A GPS data logger for the Maemo platform.
#
# Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions :
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not
#    be misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
#
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

QT  = core gui dbus
QT += maemo5

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
  src/ExporterSinkTxt.h \
  src/GPSRFile.h \
  src/Location.h \
  src/LocationFix.h \
  src/LocationGPSRFile.h \
  src/LocationMaemo.h \
  src/QLabeledLineEdit.h \
  src/QMaemoComboBox.h \
  src/QMaemoGroupBox.h \
  src/Util.h \
  src/WndAbout.h \
  src/WndBase.h \
  src/WndCell.h \
  src/WndConfig.h \
  src/WndConvert.h \
  src/WndMain.h \
  src/WndSat.h \
  src/WndSpeed.h \
  src/WndStart.h

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
  src/ExporterSinkTxt.cpp \
  src/GPSRFile.cpp \
  src/Location.cpp \
  src/LocationFix.cpp \
  src/LocationGPSRFile.cpp \
  src/LocationMaemo.cpp \
  src/QLabeledLineEdit.cpp \
  src/QMaemoComboBox.cpp \
  src/QMaemoGroupBox.cpp \
  src/Util.cpp \
  src/WndAbout.cpp \
  src/WndBase.cpp \
  src/WndCell.cpp \
  src/WndConfig.cpp \
  src/WndConvert.cpp \
  src/WndMain.cpp \
  src/WndSat.cpp \
  src/WndSpeed.cpp \
  src/WndStart.cpp

RESOURCES += src/resources.qrc


# all generated files goes same directory
DESTDIR     = build
OBJECTS_DIR = $$DESTDIR
MOC_DIR     = $$DESTDIR
UI_DIR      = $$DESTDIR


# create version.h
build-version.commands  = scripts/version.sh create-header
build-version.depends   = FORCE
QMAKE_EXTRA_TARGETS    += build-version
PRE_TARGETDEPS          = build-version


# define install paths
isEmpty(PREFIX) {
  PREFIX = /usr
}
BINDIR  = $$PREFIX/bin
DATADIR = $$PREFIX/share

# define install target
target.path  = $$BINDIR
INSTALLS    += target

desktop.path   = $$DATADIR/applications/hildon
desktop.files  = data/gpsrecorder.desktop
INSTALLS      += desktop

#service.path   = $$DATADIR/dbus-1/services
#service.files  = data/gpsrecorder.service
#INSTALLS      += service

icon48.path   = $$DATADIR/icons/hicolor/48x48/apps
icon48.files  = data/48x48/gpsrecorder.png
INSTALLS     += icon48

icon64.path   = $$DATADIR/icons/hicolor/64x64/apps
icon64.files  = data/64x64/gpsrecorder.png
INSTALLS     += icon64


# create REVISION file for source code releasing
create-revfile.commands  = scripts/version.sh create-revfile
create-revfile.depends   = FORCE
QMAKE_EXTRA_TARGETS     += create-revfile

# targets for debian source and binary package creation
debian-src.commands  = dpkg-buildpackage -S -r -us -uc -d -i -I.svn
debian-src.depends   = create-revfile
debian-bin.commands  = dpkg-buildpackage -b -r -uc -d
debian-all.depends   = debian-src debian-bin
QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin


# clean all but Makefile
compiler_clean.commands  = -$(DEL_FILE) $(TARGET) ; $(DEL_FILE) -rf $$DESTDIR ; scripts/version.sh clean
QMAKE_EXTRA_TARGETS     += compiler_clean
