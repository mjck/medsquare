/*=========================================================================

 Program:   MedSquare
 Module:    MSQProjectionMenu.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_PROJECTION_MENU_H
#define MSQ_PROJECTION_MENU_H

#include <QtGui>

class MSQColormapFactory;

class vtkmsqImageItem;
class vtkmsqLookupTable;
class vtkmsqMedicalImageProperties;

class vtkImageData;
class vtkVolume;
class vtkCallbackCommand;

class MSQProjectionMenu : public QMenu
{
Q_OBJECT
public:
  MSQProjectionMenu(QWidget *parent);
  ~MSQProjectionMenu();

  void setInput(vtkmsqImageItem *imageItem);
  void setActive(bool active);

signals:
  void projectionChanged(vtkVolume *volume);

public slots:
  void changedColormap();

private slots:
  void removeProjections();
  void maximumProjectionAction();
  void minimumProjectionAction();
  void compositeProjectionAction();

private:
  vtkmsqImageItem *imageItem;
  vtkImageData *image;
  vtkCallbackCommand *updateColormap;
  vtkmsqMedicalImageProperties *properties;
  vtkVolume* getProjection();
  vtkmsqLookupTable* colormap;
  MSQColormapFactory *colormapFactory;
  int lutType;
  int projectionType;
  QActionGroup *projections;
};

#endif
