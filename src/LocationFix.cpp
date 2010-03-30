//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-29 13:21:41
//
// $Id$
//
//***************************************************************************

#include "LocationFix.h"



//---------------------------------------------------------------------------
// clear
//---------------------------------------------------------------------------
void LocationFix::clear (void)
{
  fxuint32 uiSizeBak = uiMemorySize;
  memset(&(this->uiMemorySize), 0, sizeof(LocationFix));
  uiMemorySize = uiSizeBak;
}

//---------------------------------------------------------------------------
// updateStorageSize
//---------------------------------------------------------------------------
void LocationFix::updateStorageSize (void)
{
  fxuint32 uiNewStorageSize =
    sizeof(LocationFix) +
    (cSatCount * sizeof(LocationFixSat)) -
    sizeof(uiMemorySize);

  Q_ASSERT(uiNewStorageSize <= uiMemorySize);
  if (uiNewStorageSize <= uiMemorySize)
    uiStorageSize = uiNewStorageSize;
  else
    uiStorageSize = 0;
}

//---------------------------------------------------------------------------
// clearTrailingZone
//---------------------------------------------------------------------------
void LocationFix::clearTrailingZone (void)
{
  fxuint32 uiTrailingOffset = sizeof(LocationFix) + cSatCount * sizeof(LocationFixSat);

  if (uiTrailingOffset < uiMemorySize)
    memset((char*)(&(this->uiMemorySize)) + uiTrailingOffset, 0, uiMemorySize - uiTrailingOffset);
}

//---------------------------------------------------------------------------
// getStorageZone
//---------------------------------------------------------------------------
const char* LocationFix::getStorageZone (void)
{
  return (char*)this->uiStorageSize;
}

//---------------------------------------------------------------------------
// getSat
//---------------------------------------------------------------------------
LocationFixSat* LocationFix::getSat (fxuint8 cIndex)
{
  Q_ASSERT(cIndex < cSatCount);
  if (cIndex >= cSatCount)
    return 0;

  return (LocationFixSat*)( ((char*)(&(this->uiMemorySize)) + sizeof(LocationFix)) + (cIndex * sizeof(LocationFixSat)) );
}
