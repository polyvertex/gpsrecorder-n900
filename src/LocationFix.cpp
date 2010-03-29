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
void Fix::clear (void)
{
  fxuint32 uiSizeBak = uiSize;
  memset(&(this->uiSize), 0, sizeof(Fix));
  uiSize = uiSizeBak;
}

//---------------------------------------------------------------------------
// clearTrailingZone
//---------------------------------------------------------------------------
void Fix::clearTrailingZone (void)
{
  fxuint32 uiTrailingOffset = sizeof(Fix) + cSatCount * sizeof(FixSat);

  if (uiTrailingOffset < uiSize)
    memset((char*)(&(this->uiSize)) + uiTrailingOffset, 0, uiSize - uiTrailingOffset);
}

//---------------------------------------------------------------------------
// storageSize
//---------------------------------------------------------------------------
fxuint32 Fix::storageSize (void)
{
  fxuint32 uiStorageSize = sizeof(Fix) + (cSatCount * sizeof(FixSat));
  Q_ASSERT(uiStorageSize <= uiSize);
  return (uiStorageSize <= uiSize) ? uiStorageSize : 0;
}

//---------------------------------------------------------------------------
// getSat
//---------------------------------------------------------------------------
FixSat* Fix::getSat (fxuint8 cIndex)
{
  Q_ASSERT(cIndex < cSatCount);
  if (cIndex >= cSatCount)
    return 0;

  return (FixSat*)( ((char*)(&(this->uiSize)) + sizeof(Fix)) + (cIndex * sizeof(FixSat)) );
}
