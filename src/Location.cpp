//***************************************************************************
//
// GPS Recorder
// Show, log and convert location data from Maemo's GPS subsystem.
//
// Copyright (C) 2010 Jean-Charles Lefebvre <gpsrecorder [AT] jcl [DOT] name>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
// createDevice
//---------------------------------------------------------------------------
Location* Location::createReplay (const char* pszInputFile)
{
  LocationGPSRFile* pLocation = new LocationGPSRFile();
  Q_CHECK_PTR(pLocation);

  pLocation->setInputFileName(pszInputFile);

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
