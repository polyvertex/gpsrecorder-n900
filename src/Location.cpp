//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-29 14:19:40
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// Location
//---------------------------------------------------------------------------
Location::Location (QObject* pParent)
: QObject(pParent)
{
  // state
  m_uiFixStep = 5;
  m_bStarted  = false;

  // last fix
  m_uiFixTime = 0;
}

//---------------------------------------------------------------------------
// ~Location
//---------------------------------------------------------------------------
Location::~Location (void)
{
}



//---------------------------------------------------------------------------
// createDevice
//---------------------------------------------------------------------------
Location* Location::createDevice (void)
{
  LocationMaemo* pLocation = new LocationMaemo();
  Q_CHECK_PTR(pLocation);
  return pLocation;
}

//---------------------------------------------------------------------------
// allowedFixSteps
//---------------------------------------------------------------------------
const QVector<uint>& Location::allowedFixSteps (void)
{
  static QVector<uint> vecFixSteps;

  if (vecFixSteps.isEmpty())
  {
    vecFixSteps.reserve(8);

    vecFixSteps.append(1);
    vecFixSteps.append(2);
    vecFixSteps.append(5);
    vecFixSteps.append(10);
    vecFixSteps.append(20);
    vecFixSteps.append(30);
    vecFixSteps.append(60);
    vecFixSteps.append(120);
  }

  return vecFixSteps;
}



//---------------------------------------------------------------------------
// resetLastFix
//---------------------------------------------------------------------------
void Location::resetLastFix (void)
{
  m_uiFixTime = 0;
}
