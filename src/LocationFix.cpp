//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-29 13:21:41
//
// $Id$
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
    m_uiFixSize = 0;
  else
    this->setFix(*fixCont.getFix());
}

//---------------------------------------------------------------------------
// computeFixSize
//---------------------------------------------------------------------------
fxuint32 LocationFixContainer::computeFixSize (fxuint8 cSatCount)
{
  return sizeof(LocationFix) + cSatCount * sizeof(LocationFixSat);
}
