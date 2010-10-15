#!/bin/sh
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


TOPDIR="$(dirname $0)/.."
ACTION="$1"

VERSION_FILE="$TOPDIR/VERSION"
REVISION_FILE="$TOPDIR/REVISION"
HEADER_FILE="$TOPDIR/src/version.h"

VERSION=""
REVISION=""


fatal()
{
  echo "*** $*"
  exit 1
}

read_version()
{
  [ -f "$VERSION_FILE" ] || fatal "VERSION file does not exist or is not readable !"
  ( grep '^[0-9]\.[0-9]$' "$VERSION_FILE" > /dev/null ) || fatal "Format of VERSION file is incorrect !"
  VERSION=$(cat "$VERSION_FILE")
}

read_revision()
{
  if [ -d "$TOPDIR/.svn" ]; then
    REVISION=$(svnversion -n "$TOPDIR")
  elif [ -f "$REVISION_FILE" ]; then
    ( grep '^[0-9]\{1,5\}$' "$REVISION_FILE" > /dev/null ) || fatal "Format of REVISION file is incorrect !"
    REVISION=$(cat "$REVISION_FILE")
  else
    echo "*** Could not read REVISION number !"
    exit 1
  fi
}

write_header()
{
  echo "static const char APP_VERSION_STR[] = \"${VERSION}.${REVISION}\";" > "$HEADER_FILE"
}

clean_header()
{
  rm -f "$HEADER_FILE"
}

write_revision()
{
  [ -f "$REVISION_FILE" ] && fatal "REVISION file already exists !"
  ( echo -n "$REVISION" | grep '^[0-9]\{1,5\}$' > /dev/null ) || fatal "Format of REVISION number is incorrect ! Maybe you should commit then update ?"
  echo -n "$REVISION" > "$REVISION_FILE"
}

clean_revision()
{
  # erase REVISION file only if source code is not released
  [ -d "$TOPDIR/.svn" ] && rm -f "$REVISION_FILE"
}


case "$ACTION" in
  create-header)
    read_version
    read_revision
    write_header
    ;;
  clean-header)
    clean_header
    ;;
  create-revfile)
    read_revision
    write_revision
    ;;
  clean-revfile)
    clean_revision
    ;;
  clean)
    clean_header
    clean_revision
    ;;
  *)
    echo "Usage : $0 {create-header|clean-header|create-revfile|clean-revfile|clean}"
    exit 1
    ;;
esac

exit 0
