/*=========================================================================

 Project:   MedSquare
 Program:   MSQDicomExplorer
 Module:    main.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomExplorer.h"

#include <qapplication.h>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char** argv)
{
  #ifdef Q_OS_MACX
    if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 )
    {
        // fix Mac OS X 10.9 (mavericks) font issue
        // https://bugreports.qt-project.org/browse/QTBUG-32789
        QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
        // fix Mac OS X 10.10 font
        // https://bugreports.qt-project.org/browse/QTBUG-40833
        QFont::insertSubstitution(".Helvetica Neue DeskInterface", "Helvetica Neue");
    }
  #endif

  QApplication app(argc, argv);

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  QSplashScreen *splash = new QSplashScreen(QPixmap(":/images/splash.png"), Qt::WindowStaysOnTopHint);
  splash->show();

  splash->showMessage("Initializing...", Qt::AlignLeft, Qt::white);
  app.processEvents();
  
  // Right now, insert a delay
  QTimer::singleShot(1000, splash, SLOT(close()));

  app.setApplicationName("DICOM Explorer");

  MSQDicomExplorer mainwindow;
  mainwindow.setIconSize(QSize(32,32));
  mainwindow.resize(1200, 800);

  // Delay main window from showing up
  QTimer::singleShot(1000, &mainwindow, SLOT(show()));

  return app.exec();
}

