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
// Created On : 2010-05-16 11:45:23
//
// $Id$
//
//***************************************************************************

#include "stable.h"


//---------------------------------------------------------------------------
// WndAbout
//---------------------------------------------------------------------------
WndAbout::WndAbout (QMainWindow* pParent/*=0*/)
: QMainWindow(pParent)
{
  Q_ASSERT(App::instance());

  this->setWindowTitle(tr("About") + QString(" ") + App::applicationLabel());
#if QT_VERSION > 0x040503
  this->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
  this->createWidgets();
}

//---------------------------------------------------------------------------
// ~WndAbout
//---------------------------------------------------------------------------
WndAbout::~WndAbout (void)
{
}



//---------------------------------------------------------------------------
// createWidgets
//---------------------------------------------------------------------------
void WndAbout::createWidgets (void)
{
  // TODO :
  // * "Record" button if (App::instance()->getState() != App::STATE_STARTED)
  // * wallpaper
  // * water effect on wallpaper :)

  QLabel* pLblAbout = new QLabel();

  //pLblAbout->setStyleSheet("background-image: url(:/wallpaper-800x480.jpg);");
  pLblAbout->setTextFormat(Qt::RichText);
  pLblAbout->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
  pLblAbout->setOpenExternalLinks(true);
  pLblAbout->setText(QString(
    "<h1>%1</h1>"
    "version <b>%2</b><br>"
    "<br>"
    "Copyright (c) 2010 Jean-Charles Lefebvre<br>"
    "Licensed under the terms of the GNU Public License.<br>"
    "<br>"
    "Visit <a href=\"%3\">%3</a> for more information.<br>"
    "<br>"
    "Current output directory is %4<br>"
    )
    .arg(App::applicationLabel())
    .arg(App::applicationVersion())
    .arg(App::applicationUrl())
    .arg(App::outputDir())
  );

  this->setCentralWidget(pLblAbout);
}
