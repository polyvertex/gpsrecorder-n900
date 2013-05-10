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
  m_bAssisted = false;
  m_uiFixStep = 1;
  m_bStarted  = false;

  // last fix
  m_bAcquiring = true;
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
// createReplay
//---------------------------------------------------------------------------
/*Location* Location::createReplay (const char* pszInputFile)
{
  LocationGPSRFile* pLocation = new LocationGPSRFile();
  Q_CHECK_PTR(pLocation);

  pLocation->setInputFileName(pszInputFile);

  return pLocation;
}*/

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
    //vecFixSteps.append(10);
    //vecFixSteps.append(20);
    //vecFixSteps.append(30);
    //vecFixSteps.append(60);
    //vecFixSteps.append(120);
  }

  return vecFixSteps;
}

//---------------------------------------------------------------------------
// isAllowedFixStep
//---------------------------------------------------------------------------
bool Location::isAllowedFixStep (uint uiFixStep)
{
  return Location::allowedFixSteps().indexOf(uiFixStep) >= 0;
}

//---------------------------------------------------------------------------
// selectBestAllowedFixStep
//---------------------------------------------------------------------------
uint Location::selectBestAllowedFixStep (uint uiDesiredFixStep)
{
  if (Location::isAllowedFixStep(uiDesiredFixStep))
  {
    return uiDesiredFixStep;
  }
  else
  {
    const QVector<uint>& vecFixSteps = Location::allowedFixSteps();

    for (int i = vecFixSteps.count() - 1; i >= 0; --i)
    {
      if (uiDesiredFixStep > vecFixSteps[i] &&
          uiDesiredFixStep % vecFixSteps[i] == 0)
      {
        return vecFixSteps[i];
      }
    }
  }

  return 1;
}



//---------------------------------------------------------------------------
// resetLastFix
//---------------------------------------------------------------------------
void Location::resetLastFix (void)
{
  m_bAcquiring = true;
}
