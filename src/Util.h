//***************************************************************************
//
// GPS Recorder
// Show, log and convert GPS data from Maemo's GPS subsystem.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-14 18:43:32
//
// $Id$
//
//***************************************************************************

#ifndef __UTIL_H__
#define __UTIL_H__


//---------------------------------------------------------------------------
// Util
//---------------------------------------------------------------------------
class Util
{
public :

  // file system
  static bool               fileExists        (const char* pszFile);
  static bool               fileIsDir         (const char* pszFile);
  static bool               fileIsWritableDir (const char* pszFile);
  static QFile::Permissions filePermissions   (const char* pszFile);

  // time
  static QByteArray timeString            (bool bUTC=false, time_t uiTime=0, int iSecondsOffset=0);
  static QByteArray timeStringIso8601     (bool bUTC=false, time_t uiTime=0, int iSecondsOffset=0);
  static QByteArray timeStringForFileName (time_t uiTime=0);
  static int        timeDiff              (uint uiTimeOld, uint uiTimeNew, bool bAbsolute=false);
  static time_t     timeApplyOffset       (int iSecondsOffset, time_t uiTime=0, int* piAppliedOffset=0);
  static bool       timeSetup             (uint uiNewTime, int* pnTimeDiff=0);
  static int        timeZoneOffset        (void); // current time zone

  // maemo specific
  static QByteArray maemoFindHomeDir   (void);
  static QByteArray maemoFindMyDocsDir (void);
};


#endif // #ifndef __UTIL_H__
