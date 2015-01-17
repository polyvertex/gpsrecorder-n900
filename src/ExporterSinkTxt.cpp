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
// Created: 2010-10-25 08:30:51
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// ExporterSinkTxt
//---------------------------------------------------------------------------
ExporterSinkTxt::ExporterSinkTxt (Exporter* pParent)
: ExporterSink(pParent)
{
  this->connect(
    pParent,
    SIGNAL(sigSOF(const char*, time_t, qint32)),
    SLOT(onSOF(const char*, time_t, qint32)) );
  this->connect(
    pParent,
    SIGNAL(sigLocationFix(time_t, const LocationFixContainer&)),
    SLOT(onLocationFix(time_t, const LocationFixContainer&)) );
  this->connect(
    pParent,
    SIGNAL(sigEOF(void)),
    SLOT(onEOF(void)) );

  // read settings
  m_bIncludeLocationFix = App::instance()->settings()->getTxtIncludeLocationFix();
}

//---------------------------------------------------------------------------
// ~ExporterSinkTxt
//---------------------------------------------------------------------------
ExporterSinkTxt::~ExporterSinkTxt (void)
{
  this->close();
}



//---------------------------------------------------------------------------
// close
//---------------------------------------------------------------------------
void ExporterSinkTxt::close (void)
{
  ExporterSink::close();
}



//---------------------------------------------------------------------------
// onSOF
//---------------------------------------------------------------------------
void ExporterSinkTxt::onSOF (const char* pszFilePath, time_t uiTime, qint32 iTimeZoneOffset)
{
  Q_UNUSED(uiTime);
  Q_UNUSED(iTimeZoneOffset);

  QString   strDump;
  GPSRFile* pGPSRFile;

  this->close();

  // dump source file
  pGPSRFile = &static_cast<Exporter*>(this->parent())->gpsrFile();
  if (!GPSRFile::dump(pGPSRFile->getPath().constData(), strDump, m_bIncludeLocationFix))
  {
    qWarning("Failed to convert file to %s : %s", this->label(), pGPSRFile->getPath().constData());
    return;
  }

  // build destination path
  m_strFilePath  = pszFilePath;
  m_strFilePath += '.';
  m_strFilePath += this->extension();

  // write output file
  {
    QFile outFile(m_strFilePath);

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
      qWarning("Unable to create output file %s !", qPrintable(m_strFilePath));
      return;
    }

    if (outFile.write(strDump.toLocal8Bit()) != strDump.toLocal8Bit().length())
    {
      qWarning("Failed to write to %s !", qPrintable(m_strFilePath));
      return;
    }
  }
}

//---------------------------------------------------------------------------
// onLocationFix
//---------------------------------------------------------------------------
void ExporterSinkTxt::onLocationFix (time_t uiTime, const LocationFixContainer& fixCont)
{
  Q_UNUSED(uiTime);
  Q_UNUSED(fixCont);
}

//---------------------------------------------------------------------------
// onEOF
//---------------------------------------------------------------------------
void ExporterSinkTxt::onEOF (void)
{
  this->close();
}
