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
      "Licensed under the terms of the zLib License.<br>"
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

    m_pScrollArea = new QScrollArea;
    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->setWidget(pLblAbout);
    m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pScrollArea->setProperty("FingerScrollable", true);

    pHBox->setSpacing(1);
    pHBox->addWidget(pLblIcon, 0, Qt::AlignLeft | Qt::AlignTop);
    pHBox->addWidget(m_pScrollArea);

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



//---------------------------------------------------------------------------
// showEvent
//---------------------------------------------------------------------------
void WndAbout::showEvent (QShowEvent* pEvent)
{
  m_pScrollArea->ensureVisible(0, 0); // scroll to the top
  WndBase::showEvent(pEvent);
}
