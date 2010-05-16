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
OUTFILE_TXT="$TOPDIR/version.txt"
OUTFILE_H="$TOPDIR/src/version.h"
VERSION=""
REVISION=""

# get version number
if [ ! -f "$TOPDIR/VERSION" ]; then
  echo "*** VERSION file does not exist or is not readable !"
  exit 1
fi
VERSION=$(cat "$TOPDIR/VERSION")

# if .svn is not here, we assume this is a released version and the revision
# number is already included into the VERSION file
if [ -d "$TOPDIR/.svn" ]; then
  REVISION=$(svnversion -n "$TOPDIR")
  VERSION="${VERSION}.${REVISION}"
fi

# create output files
echo -n "$VERSION" > "$OUTFILE_TXT"
echo "static const char APP_VERSION_STR[] = \"${VERSION}\";" > "$OUTFILE_H"

exit 0
