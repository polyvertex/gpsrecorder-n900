TARGET = gpsrecorder

CONFIG += precompile_header release
#CONFIG += silent
CONFIG += debug
CONFIG(debug, debug|release) {
  TARGET = $$join(TARGET,,,d)
  message(This is a DEBUG build !)
}

# http://qt.nokia.com/doc/4.5/qmake-variable-reference.html#qt
QT  = core gui
#QT += maemo5

INCLUDEPATH += /usr/include/glib-2.0 /usr/lib/glib-2.0/include
LIBS        += -lglib-2.0 -llocation
DEFINES     +=

QMAKE_CFLAGS   += -fno-strict-aliasing -Werror
QMAKE_CXXFLAGS += -fno-strict-aliasing -Werror

PRECOMPILED_HEADER = src/stable.h

HEADERS += \
  src/App.h \
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
  src/WndMain.h
SOURCES += \
  src/main.cpp \
  src/App.cpp \
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
  src/WndMain.cpp

FORMS       +=
LEXSOURCES  += #LEXS#
YACCSOURCES += #YACCS#

# All generated files goes same directory
OBJECTS_DIR = build
MOC_DIR     = build
UI_DIR      = build

DESTDIR     = build
TEMPLATE    = app
DEPENDPATH +=
VPATH      += src uis

INSTALLS    += target
target.path  = /usr/bin/

INSTALLS      += desktop
desktop.path   = /usr/share/applications/hildon
desktop.files  = data/gpsrecorder.desktop

INSTALLS      += service
service.path   = /usr/share/dbus-1/services
service.files  = data/gpsrecorder.service

INSTALLS     += icon64
icon64.path   = /usr/share/icons/hicolor/64x64/apps
icon64.files  = data/64x64/gpsrecorder.png


PRE_TARGETDEPS          += src/revision.h
build-revision.target    = src/revision.h
build-revision.commands  = scripts/revision2c.sh src/revision.h
build-revision.depends   = FORCE
QMAKE_EXTRA_TARGETS     += build-revision

#
# Targets for debian source and binary package creation
#
debian-src.commands = dpkg-buildpackage -S -r -us -uc -d
debian-bin.commands = dpkg-buildpackage -b -r -uc -d
debian-all.depends  = debian-src debian-bin

#
# Clean all but Makefile
#
compiler_clean.commands = -$(DEL_FILE) $(TARGET) src/revision.h

QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin compiler_clean
