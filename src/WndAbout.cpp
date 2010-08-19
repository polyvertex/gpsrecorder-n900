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
: WndBase(pParent)
{
  Q_ASSERT(App::instance());

  this->setWindowTitle(tr("About") + QString(" ") + App::applicationLabel());
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
  QLabel* pLblIcon = new QLabel;
  QLabel* pLblAbout = new QLabel;

  this->readChangeLog();

  pLblIcon->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  pLblIcon->setPixmap(QPixmap(":/icons/appicon-128.png"));
  pLblIcon->setFixedWidth(140);

  // 'about' text
  {
    QString strAbout;
    QMapIterator<QString, QString> itLog(m_mapChangeLog);

    strAbout = QString(
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
      "<h3>ChangeLog :</h3>"
      )
      .arg(App::applicationLabel())
      .arg(App::applicationVersion())
      .arg(App::applicationUrl())
      .arg(App::applicationEMail())
      .arg(App::outputDir());

    itLog.toBack();
    while (itLog.hasPrevious())
    {
      itLog.previous();
      strAbout += itLog.key() + " :\n" + itLog.value() + "\n";
    }
    strAbout.replace('\n', "<br>");

    //pLblAbout->setStyleSheet("background-image: url(:/images/wallpaper-800x480.jpg);");
    pLblAbout->setTextFormat(Qt::RichText);
    pLblAbout->setWordWrap(true);
    pLblAbout->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    pLblAbout->setOpenExternalLinks(true);
    pLblAbout->setText(strAbout);
  }

  // main layout setup
  {
    QWidget*     pRootWidget = new QWidget;
    QHBoxLayout* pHBox = new QHBoxLayout;
    QScrollArea* pScrollArea = new QScrollArea;

    pScrollArea->setWidgetResizable(true);
    pScrollArea->setWidget(pLblAbout);
    pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pScrollArea->setProperty("FingerScrollable", true);

    pHBox->setSpacing(1);
    pHBox->addWidget(pLblIcon, 0, Qt::AlignLeft | Qt::AlignTop);
    pHBox->addWidget(pScrollArea);

    pRootWidget->setLayout(pHBox);

    this->setCentralWidget(pRootWidget);
  }
}

//---------------------------------------------------------------------------
// readChangeLog
//---------------------------------------------------------------------------
void WndAbout::readChangeLog (void)
{
  QFile file(":/changelog");

  m_mapChangeLog.clear();

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QTextStream stream(&file);
  QString     strLogHeader(QCoreApplication::applicationName() + " (");
  QString     strVersion;
  QString     strLine = stream.readLine();

  while (!strLine.isNull())
  {
    // log header
    if (strLine.startsWith(strLogHeader, Qt::CaseSensitive))
    {
      int nEnd = strLine.indexOf(')', strLogHeader.length());
      if (nEnd > 0)
        strVersion = strLine.mid(strLogHeader.length(), nEnd - strLogHeader.length());
    }

    // log footer
    else if (strLine.startsWith(" -- "))
    {
      strVersion.clear();
    }

    // log content
    else if (!strVersion.isEmpty())
    {
      strLine = strLine.trimmed();
      if (!strLine.isEmpty())
        m_mapChangeLog[strVersion] += strLine.trimmed() + "\n";
    }

    // next line
    strLine = stream.readLine();
  }

  file.close();
}
