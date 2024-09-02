/*=========================================================================

 Program:   MedSquare
 Module:    MSQRenderWidget.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_RENDER_WIDGET_H
#define MSQ_RENDER_WIDGET_H

#include <QtGui>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include "QVTKOpenGLWidget.h"

//class QVTKWidget2;

class vtkActor;
class vtkActor2D;
class vtkImageData;
class vtkRenderer;
class vtkVolume;

class vtkmsqLookupTable;
class vtkmsqCornerAnnotation;
class vtkmsqInteractorStyleImage;
class vtkmsqMedicalImageProperties;
class vtkmsqRectangleActor2D;
class vtkmsqImageItem;

class MSQRenderWidget : public QWidget
{
Q_OBJECT

public:
  MSQRenderWidget();
  virtual ~MSQRenderWidget();

  void setAxialSlice(int slice);
  void setSagittalSlice(int slice);
  void setCoronalSlice(int slice);

  void disableInteraction();
  void enableInteraction();

  void toggleMaximized(vtkRenderer *pokedRenderer);
  bool has(vtkRenderer *renderer);

  void addActor(vtkActor *actor);
  void removeActor(vtkActor *actor);

  void addActor(vtkActor2D *actor);
  void removeActor(vtkActor2D *actor);

  void addVolume(vtkVolume *volume);
  void removeVolume(vtkVolume *volume);

  void reset();
  void dolly(double factor);

  void setFrameEnabled(bool enabled);
  bool isFrameEnabled();
  void refresh();

  virtual double pick(double selectionX, double selectionY, double imageCoords[3]) = 0;
  virtual void createOrientationMarkerWidgets() = 0;
	virtual void enableOrientationMarkerWidget() { };
	virtual void disableOrientationMarkerWidget() { };

  virtual void setSlice(int axial, int sagittal, int coronal) = 0;
  virtual void setInput(vtkmsqImageItem *imageItem) = 0;
  virtual double* GetOrigin() = 0;
  virtual double* GetPoint1() = 0;
  virtual double* GetPoint2() = 0;

public slots:
  virtual void setComponent(int component) = 0;
  virtual void setLevel(double value) = 0;
  virtual void setWindow(double value) = 0;
  virtual void setOpacity(double value) = 0;
  void takeSnapshot();

protected:
  int currentAxialSlice;
  int currentSagittalSlice;
  int currentCoronalSlice;

  //QVTKWidget2 *vtkWidget;
  QVTKOpenGLWidget *vtkWidget;

  vtkmsqRectangleActor2D *frame;
  vtkmsqInteractorStyleImage *interStyle;
  vtkRenderer *renderer;
  
  void buildFrame();
};

#endif

