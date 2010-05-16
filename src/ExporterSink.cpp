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
// Created On : 2010-04-03 11:51:19
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSink
//---------------------------------------------------------------------------
ExporterSink::ExporterSink (Exporter* pParent)
: QObject(pParent)
{
  Q_ASSERT(pParent);

  m_pFile = 0;
}

//---------------------------------------------------------------------------
// ~ExporterSink
//---------------------------------------------------------------------------
ExporterSink::~ExporterSink (void)
{
}



//---------------------------------------------------------------------------
// close
//---------------------------------------------------------------------------
void ExporterSink::close (void)
{
  if (m_pFile)
  {
    fclose(m_pFile);
    m_pFile = 0;
  }

  m_strFilePath.clear();
}
