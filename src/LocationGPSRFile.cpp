//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex+gpsrecorder [AT] gmail [DOT] com>
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
// Created On : 2010-04-12 13:22:01
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// LocationGPSRFile
//---------------------------------------------------------------------------
LocationGPSRFile::LocationGPSRFile (void)
: Location(0)
{
  m_uiFixIndex = 0;


  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadError(GPSRFile*, GPSRFile::Error)),
    SLOT(onReadError(GPSRFile*, GPSRFile::Error)) );
  this->connect(
    &m_GPSRFile,
    SIGNAL(sigReadChunkLocationFix(GPSRFile*, time_t, const LocationFix&)),
    SLOT(onReadChunkLocationFix(GPSRFile*, time_t, const LocationFix&)) );
}

//---------------------------------------------------------------------------
// ~LocationGPSRFile
//---------------------------------------------------------------------------
LocationGPSRFile::~LocationGPSRFile (void)
{
  this->stop();
}




//---------------------------------------------------------------------------
// setInputFileName
//---------------------------------------------------------------------------
void LocationGPSRFile::setInputFileName (const char* pszFilePath)
{
  Q_ASSERT(pszFilePath);
  Q_ASSERT(pszFilePath[0]);
  if (!pszFilePath || !pszFilePath[0])
    return;

  m_strFilePath = pszFilePath;
}

//---------------------------------------------------------------------------
// readFix
//---------------------------------------------------------------------------
bool LocationGPSRFile::readFix (uint uiFixIndex)
{
  Q_ASSERT(m_GPSRFile.isOpen());
  if (!m_GPSRFile.isOpen())
    return false;

  if (m_vecFixChunkIndex.isEmpty())
    return false;

  Q_ASSERT(uiFixIndex < (uint)m_vecFixChunkIndex.count());
  if (uiFixIndex >= (uint)m_vecFixChunkIndex.count())
    return false;

  return m_GPSRFile.readChunk((int)m_vecFixChunkIndex[uiFixIndex]);
}




//---------------------------------------------------------------------------
// setAssisted
//---------------------------------------------------------------------------
bool LocationGPSRFile::setAssisted (bool bAssisted)
{
  m_bAssisted = bAssisted;
  return true;
}

//---------------------------------------------------------------------------
// setFixStep
//---------------------------------------------------------------------------
bool LocationGPSRFile::setFixStep (uint uiNewFixStepSeconds)
{
  // actually, we don't care about this...

  if (Location::isAllowedFixStep(uiNewFixStepSeconds))
  {
    m_uiFixStep = uiNewFixStepSeconds;
    return true;
  }

  return false;
}

//---------------------------------------------------------------------------
// resetLastFix
//---------------------------------------------------------------------------
void LocationGPSRFile::resetLastFix (void)
{
  Location::resetLastFix();
}

//---------------------------------------------------------------------------
// start
//---------------------------------------------------------------------------
void LocationGPSRFile::start (void)
{
  if (m_bStarted)
  {
    Q_ASSERT(m_GPSRFile.isOpen());
    Q_ASSERT(m_strFilePath == m_GPSRFile.getPath());

    if (m_strFilePath == m_GPSRFile.getPath())
    {
      // this is the same input file, just rewind
      this->readFirstFix();
      return;
    }
    else
    {
      // this is an another input file, stop now
      this->stop();
    }
  }

  if (m_GPSRFile.openRead(m_strFilePath.constData()))
  {
    m_bAcquiring = true;
    m_bStarted   = true;

    if (!m_vecFixChunkIndex.isEmpty())
      this->readFirstFix();
  }
}

//---------------------------------------------------------------------------
// stop
//---------------------------------------------------------------------------
void LocationGPSRFile::stop (void)
{
  m_GPSRFile.close();

  m_bStarted   = false;
  m_bAcquiring = false;
  m_FixCont.reset();
}




//---------------------------------------------------------------------------
// onReadError
//---------------------------------------------------------------------------
void LocationGPSRFile::onReadError (GPSRFile* pGPSRFile, GPSRFile::Error eError)
{
  Q_UNUSED(pGPSRFile);
  Q_UNUSED(eError);

  qWarning("GPSR read error %d !", eError);

  this->stop();
}

//---------------------------------------------------------------------------
// onReadChunkLocationFix
//---------------------------------------------------------------------------
void LocationGPSRFile::onReadChunkLocationFix (GPSRFile* pGPSRFile, time_t uiTime, const LocationFix& fix)
{
  Q_UNUSED(pGPSRFile);

  m_FixCont.setFix(fix);
  m_bAcquiring = false;
}
