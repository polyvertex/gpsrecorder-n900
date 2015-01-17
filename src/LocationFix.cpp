//***************************************************************************
//
// GPS Recorder
// A GPS data logger for the Maemo platform.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <polyvertex@gmail.com>
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
// Created: 2010-03-29 13:21:41
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// getModeStr
//---------------------------------------------------------------------------
const char* LocationFix::getModeStr (void) const
{
  switch (cFixMode)
  {
    case FIXMODE_NOTSEEN :
      return "NotSeen";
    case FIXMODE_NOFIX :
      return "NoFix";
    case FIXMODE_2D :
      return "2D";
    case FIXMODE_3D :
      return "3D";

    default :
      return "?";
  }
}

//---------------------------------------------------------------------------
// getHorizEp
//---------------------------------------------------------------------------
double LocationFix::getHorizEp (uint uiUnitSystem) const
{
  if (!this->hasFields(FIXFIELD_LATLONG))
    return NAN;

  // uiHorizEP unit is cm
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return double(uiHorizEP) / 100.0; // cm -> m
    case UNITSYSTEM_IMPERIAL :
      return double(uiHorizEP) * 0.0328083989501; // cm -> ft

    default :
      Q_ASSERT(0);
      return NAN;
  }
}

const char* LocationFix::getHorizEpSuffix (uint uiUnitSystem) const
{
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return "m";
    case UNITSYSTEM_IMPERIAL :
      return "ft";

    default :
      Q_ASSERT(0);
      return "?";
  }
}

//---------------------------------------------------------------------------
// getAlt
//---------------------------------------------------------------------------
double LocationFix::getAlt (uint uiUnitSystem) const
{
  if (!this->hasFields(FIXFIELD_ALT))
    return NAN;

  // iAlt unit is m
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return double(iAlt); // m
    case UNITSYSTEM_IMPERIAL :
      return double(iAlt) * 3.28083989501; // m -> ft

    default :
      Q_ASSERT(0);
      return NAN;
  }
}

const char* LocationFix::getAltSuffix (uint uiUnitSystem) const
{
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return "m";
    case UNITSYSTEM_IMPERIAL :
      return "ft";

    default :
      Q_ASSERT(0);
      return "?";
  }
}

//---------------------------------------------------------------------------
// getAltEp
//---------------------------------------------------------------------------
double LocationFix::getAltEp (uint uiUnitSystem) const
{
  if (!this->hasFields(FIXFIELD_ALT))
    return NAN;

  // uiAltEP unit is m
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return double(uiAltEP); // m
    case UNITSYSTEM_IMPERIAL :
      return double(uiAltEP) * 3.28083989501; // m -> ft

    default :
      Q_ASSERT(0);
      return NAN;
  }
}

const char* LocationFix::getAltEpSuffix (uint uiUnitSystem) const
{
  return this->getAltSuffix(uiUnitSystem);
}

//---------------------------------------------------------------------------
// getSpeed
//---------------------------------------------------------------------------
double LocationFix::getSpeed (uint uiHorizSpeedUnit) const
{
  if (!this->hasFields(FIXFIELD_SPEED))
    return NAN;

  // uiSpeed unit is m/h
  switch (uiHorizSpeedUnit)
  {
    case HORIZSPEEDUNIT_KMH :
      return double(uiSpeed) / double(LOCFIX_MULTIPLIER_SPEED); // m/h -> km/h
    case HORIZSPEEDUNIT_MPH :
      return double(uiSpeed) * 0.000621371192237; // m/h -> mph
    case HORIZSPEEDUNIT_MS :
      return double(uiSpeed) * 0.000277777777778; // m/h -> m/s
    case HORIZSPEEDUNIT_KNOTS :
      return double(uiSpeed) * 0.000539956803456; // m/h -> knots

    default :
      Q_ASSERT(0);
      return NAN;
  }
}

const char* LocationFix::getSpeedSuffix (uint uiHorizSpeedUnit) const
{
  switch (uiHorizSpeedUnit)
  {
    case HORIZSPEEDUNIT_KMH :
      return "km/h";
    case HORIZSPEEDUNIT_MPH :
      return "mph";
    case HORIZSPEEDUNIT_MS :
      return "m/s";
    case HORIZSPEEDUNIT_KNOTS :
      return "knots";

    default :
      return "?";
  }
}

//---------------------------------------------------------------------------
// getSpeedEp
//---------------------------------------------------------------------------
double LocationFix::getSpeedEp (uint uiHorizSpeedUnit) const
{
  if (!this->hasFields(FIXFIELD_SPEED))
    return NAN;

  // uiSpeedEP unit is m/h
  switch (uiHorizSpeedUnit)
  {
    case HORIZSPEEDUNIT_KMH :
      return double(uiSpeedEP) / double(LOCFIX_MULTIPLIER_SPEED); // m/h -> km/h
    case HORIZSPEEDUNIT_MPH :
      return double(uiSpeedEP) * 0.000621371192237; // m/h -> mph
    case HORIZSPEEDUNIT_MS :
      return double(uiSpeedEP) * 0.000277777777778; // m/h -> m/s
    case HORIZSPEEDUNIT_KNOTS :
      return double(uiSpeedEP) * 0.000539956803456; // m/h -> knots

    default :
      Q_ASSERT(0);
      return NAN;
  }
}

const char* LocationFix::getSpeedEpSuffix (uint uiHorizSpeedUnit) const
{
  return this->getSpeedSuffix(uiHorizSpeedUnit);
}

//---------------------------------------------------------------------------
// getClimb
//---------------------------------------------------------------------------
double LocationFix::getClimb (uint uiUnitSystem) const
{
  if (!this->hasFields(FIXFIELD_CLIMB))
    return NAN;

  // iClimb unit is cm/s
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return double(iClimb) / double(LOCFIX_MULTIPLIER_CLIMB); // cm/s -> m/s
    case UNITSYSTEM_IMPERIAL :
      return double(iClimb) * 0.0328083989501; // cm/s -> ft/s

    default :
      Q_ASSERT(0);
      return NAN;
  }
}

const char* LocationFix::getClimbSuffix (uint uiUnitSystem) const
{
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return "m/s";
    case UNITSYSTEM_IMPERIAL :
      return "ft/s";

    default :
      Q_ASSERT(0);
      return "?";
  }
}

//---------------------------------------------------------------------------
// getClimbEp
//---------------------------------------------------------------------------
double LocationFix::getClimbEp (uint uiUnitSystem) const
{
  if (!this->hasFields(FIXFIELD_CLIMB))
    return NAN;

  // uiClimbEP unit is cm/s
  switch (uiUnitSystem)
  {
    case UNITSYSTEM_METRIC :
      return double(uiClimbEP) / double(LOCFIX_MULTIPLIER_CLIMB); // cm/s -> m/s
    case UNITSYSTEM_IMPERIAL :
      return double(uiClimbEP) * 0.0328083989501; // cm/s -> ft/s

    default :
      Q_ASSERT(0);
      return NAN;
  }
}

const char* LocationFix::getClimbEpSuffix (uint uiUnitSystem) const
{
  return this->getClimbSuffix(uiUnitSystem);
}

//---------------------------------------------------------------------------
// getSat
//---------------------------------------------------------------------------
const LocationFixSat* LocationFix::getSat (fxuint8 cSatIndex) const
{
  Q_ASSERT(cSatIndex < cSatCount);
  return (cSatIndex < cSatCount) ? &aFixSat[cSatIndex] : 0;
}




//---------------------------------------------------------------------------
// LocationFixContainer
//---------------------------------------------------------------------------
LocationFixContainer::LocationFixContainer (void)
{
  m_uiBufferSize = 0;
  m_pFix         = 0;
  m_uiFixSize    = 0;
}

//---------------------------------------------------------------------------
// ~LocationFixContainer
//---------------------------------------------------------------------------
LocationFixContainer::~LocationFixContainer (void)
{
  if (m_pFix)
    free(m_pFix);
}

//---------------------------------------------------------------------------
// reset
//---------------------------------------------------------------------------
void LocationFixContainer::reset (void)
{
  if (m_pFix)
  {
    free(m_pFix);
    m_pFix = 0;
  }

  m_uiBufferSize = 0;
  m_uiFixSize    = 0;
}

//---------------------------------------------------------------------------
// allocate
//---------------------------------------------------------------------------
void LocationFixContainer::allocate (fxuint32 uiNewSize)
{
  if (m_pFix && m_uiBufferSize >= uiNewSize)
    return;

  if (!m_pFix)
  {
    m_pFix = (LocationFix*)malloc(uiNewSize);
    Q_CHECK_PTR(m_pFix);

    m_uiBufferSize = uiNewSize;
    m_uiFixSize    = 0;
  }
  else if (m_uiBufferSize < uiNewSize)
  {
    m_pFix = (LocationFix*)realloc(m_pFix, uiNewSize);
    Q_CHECK_PTR(m_pFix);

    m_uiBufferSize = uiNewSize;
  }
}

//---------------------------------------------------------------------------
// prepare
//---------------------------------------------------------------------------
LocationFix* LocationFixContainer::prepare (fxuint8 cSatCount)
{
  this->allocate(LocationFixContainer::computeFixSize(cSatCount));
  return m_pFix;
}

//---------------------------------------------------------------------------
// finalize
//---------------------------------------------------------------------------
void LocationFixContainer::finalize (void)
{
  fxuint32 uiFixSize;

  Q_ASSERT(m_pFix);
  Q_ASSERT(m_uiBufferSize);
  if (!m_pFix || !m_uiBufferSize)
    return;

  // compute the real storage size of the fix according to the number of
  // trailing LocationFixSat elements
  uiFixSize = LocationFixContainer::computeFixSize(m_pFix->cSatCount);
  Q_ASSERT(uiFixSize <= m_uiBufferSize);
  if (uiFixSize > m_uiBufferSize)
  {
    m_uiFixSize = 0;
    return;
  }
  m_uiFixSize = uiFixSize;
}

//---------------------------------------------------------------------------
// setFix
//---------------------------------------------------------------------------
void LocationFixContainer::setFix (const LocationFix& fix)
{
  fxuint32 uiFixSize = LocationFixContainer::computeFixSize(fix.cSatCount);

  this->allocate(uiFixSize);

  memcpy(m_pFix, &fix, uiFixSize);
  m_uiFixSize = uiFixSize;
}

//---------------------------------------------------------------------------
// setFix
//---------------------------------------------------------------------------
void LocationFixContainer::setFix (const LocationFixContainer& fixCont)
{
  if (fixCont.hasFix())
    this->setFix(*fixCont.getFix());
  else
    m_uiFixSize = 0;
}

//---------------------------------------------------------------------------
// computeFixSize
//---------------------------------------------------------------------------
fxuint32 LocationFixContainer::computeFixSize (fxuint8 cSatCount)
{
  return sizeof(LocationFix) + cSatCount * sizeof(LocationFixSat);
}
