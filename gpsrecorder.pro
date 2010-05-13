
TARGET = gpsrecorder

TEMPLATE = app

CONFIG += warn_off precompile_header release
#CONFIG += silent
CONFIG += debug
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
  src/WndConfig.h \
  src/WndConvert.h \
  src/WndMain.h \
  src/WndSat.h
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
  src/WndConfig.cpp \
  src/WndConvert.cpp \
  src/WndMain.cpp \
  src/WndSat.cpp

RESOURCES +=


# all generated files goes same directory
DESTDIR     = build
OBJECTS_DIR = $$DESTDIR
MOC_DIR     = $$DESTDIR
UI_DIR      = $$DESTDIR


# build revision.h header
PRE_TARGETDEPS          += src/revision.h
build-revision.target    = src/revision.h
build-revision.commands  = scripts/revision2c.sh src/revision.h
build-revision.depends   = FORCE
QMAKE_EXTRA_TARGETS     += build-revision


# define install target
INSTALLS    += target
target.path  = /usr/bin/

INSTALLS      += desktop
desktop.path   = /usr/share/applications/hildon
desktop.files  = data/gpsrecorder.desktop

#INSTALLS      += service
#service.path   = /usr/share/dbus-1/services
#service.files  = data/gpsrecorder.service

INSTALLS     += icon64
icon64.path   = /usr/share/icons/hicolor/64x64/apps
icon64.files  = data/64x64/gpsrecorder.png


# targets for debian source and binary package creation
debian-src.commands  = dpkg-buildpackage -S -r -us -uc -d
debian-bin.commands  = dpkg-buildpackage -b -r -uc -d
debian-all.depends   = debian-src debian-bin
QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin


# clean all but Makefile
compiler_clean.commands  = -$(DEL_FILE) $(TARGET) src/revision.h
QMAKE_EXTRA_TARGETS     +=  compiler_clean
