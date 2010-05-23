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

  QWidget* pWidget = new QWidget();
  QHBoxLayout* pHBox = new QHBoxLayout();
  QLabel* pLblIcon = new QLabel();
  QLabel* pLblAbout = new QLabel();

  pLblIcon->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  pLblIcon->setPixmap(QPixmap(":/appicon-128.png"));
  pLblIcon->setFixedWidth(140);

  //pLblAbout->setStyleSheet("background-image: url(:/wallpaper-800x480.jpg);");
  pLblAbout->setTextFormat(Qt::RichText);
  pLblAbout->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
  pLblAbout->setOpenExternalLinks(true);
  pLblAbout->setText(QString(
    "<h1>%1</h1>"
    "version <b>%2</b><br>"
    "<br>"
    "<a href=\"%3\">%3</a><br>"
    "<br>"
    "Copyright (c) 2010 <a href=\"mailto:%4\">Jean-Charles Lefebvre</a><br>"
    "Licensed under the terms of the GNU Public License.<br>"
    "<br>"
    "Current output directory is :<br>"
    "%5<br>"
    )
    .arg(App::applicationLabel())
    .arg(App::applicationVersion())
    .arg(App::applicationUrl())
    .arg(App::applicationEMail())
    .arg(App::outputDir())
  );

  pHBox->addWidget(pLblIcon);
  pHBox->addWidget(pLblAbout);
  pWidget->setLayout(pHBox);
  this->setCentralWidget(pWidget);
}
