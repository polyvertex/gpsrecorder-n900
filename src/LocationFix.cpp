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
  fxuint32 uiSizeBak = uiSize;
  memset(&(this->uiSize), 0, sizeof(LocationFix));
  uiSize = uiSizeBak;
}

//---------------------------------------------------------------------------
// clearTrailingZone
//---------------------------------------------------------------------------
void LocationFix::clearTrailingZone (void)
{
  fxuint32 uiTrailingOffset = sizeof(LocationFix) + cSatCount * sizeof(LocationFixSat);

  if (uiTrailingOffset < uiSize)
    memset((char*)(&(this->uiSize)) + uiTrailingOffset, 0, uiSize - uiTrailingOffset);
}

//---------------------------------------------------------------------------
// storageSize
//---------------------------------------------------------------------------
fxuint32 LocationFix::storageSize (void)
{
  fxuint32 uiStorageSize = sizeof(LocationFix) + (cSatCount * sizeof(LocationFixSat));
  Q_ASSERT(uiStorageSize <= uiSize);
  return (uiStorageSize <= uiSize) ? uiStorageSize : 0;
}

//---------------------------------------------------------------------------
// getSat
//---------------------------------------------------------------------------
LocationFixSat* LocationFix::getSat (fxuint8 cIndex)
{
  Q_ASSERT(cIndex < cSatCount);
  if (cIndex >= cSatCount)
    return 0;

  return (LocationFixSat*)( ((char*)(&(this->uiSize)) + sizeof(LocationFix)) + (cIndex * sizeof(LocationFixSat)) );
}
