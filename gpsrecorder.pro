TARGET = gpsrecorder

CONFIG += precompile_header release
#CONFIG += silent
CONFIG(debug, debug|release) {
  TARGET = $$join(TARGET,,,d)
  message(This is a DEBUG build !)
}

# http://qt.nokia.com/doc/4.5/qmake-variable-reference.html#qt
QT = core gui

INCLUDEPATH += /usr/include/glib-2.0 /usr/lib/glib-2.0/include
LIBS        += -lglib-2.0 -llocation
DEFINES     +=

QMAKE_CFLAGS   += -fno-strict-aliasing -Werror
QMAKE_CXXFLAGS += -fno-strict-aliasing -Werror

PRECOMPILED_HEADER = src/stable.h

HEADERS += \
  src/App.h \
  src/GPSRFile.h \
  src/Location.h \
  src/LocationFix.h \
  src/LocationMaemo.h \
  src/Util.h \
  src/WndMain.h
SOURCES += \
  src/main.cpp \
  src/App.cpp \
  src/GPSRFile.cpp \
  src/Location.cpp \
  src/LocationFix.cpp \
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

#
# Targets for debian source and binary package creation
#
debian-src.commands = dpkg-buildpackage -S -r -us -uc -d
debian-bin.commands = dpkg-buildpackage -b -r -uc -d
debian-all.depends  = debian-src debian-bin

#
# Clean all but Makefile
#
compiler_clean.commands = -$(DEL_FILE) $(TARGET)

QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin compiler_clean
