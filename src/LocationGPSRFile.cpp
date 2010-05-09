//***************************************************************************
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
    Q_ASSERT(strcmp(m_strFilePath.constData(), m_GPSRFile.getPath()) == 0);

    if (!strcmp(m_strFilePath.constData(), m_GPSRFile.getPath()))
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
