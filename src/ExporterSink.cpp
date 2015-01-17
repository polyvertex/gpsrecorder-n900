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
// Created: 2010-04-03 11:51:19
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

  // read settings
  {
    AppSettings& settings = *App::instance()->settings();

    m_bExportPause = settings.getConvertExportPauses();
  }
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
