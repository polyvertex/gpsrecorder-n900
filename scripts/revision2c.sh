#!/bin/sh

OUTFILE=$1

VERSION=$(svnversion -n "$(dirname $0)/..")
VERSION_C_STR="static const char SVN_REVISION_STR[] = \"${VERSION}\";"

if [ -z "$OUTFILE" ]; then
  echo "$VERSION_C_STR"
else
  echo "$VERSION_C_STR" > "$OUTFILE"
fi
