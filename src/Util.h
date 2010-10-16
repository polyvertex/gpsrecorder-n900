//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
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
