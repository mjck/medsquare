/*=========================================================================

 Program:   MedSquare
 Module:    main.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MedSquare.h"

#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QSurfaceFormat>
#include <QTimer>
#include "QVTKOpenGLWidget.h"

int main(int argc, char** argv)
{
  // it seems to be required to run OpenGL 3.2
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

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

  QSplashScreen *splash = new QSplashScreen(QPixmap(":/images/splash.png"), Qt::WindowStaysOnTopHint);
  splash->show();

  splash->showMessage("Initializing...", Qt::AlignLeft, Qt::white);
  app.processEvents();
  
  // Right now, insert a delay
  QTimer::singleShot(1500, splash, SLOT(close()));

  app.setApplicationName("MedSquare");

  MedSquare mainwindow;

  return app.exec();
}

