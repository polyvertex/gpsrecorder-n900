#!/bin/sh
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
