/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomImageViewer.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOMIMAGE_VIEWER_H
#define MSQ_DICOMIMAGE_VIEWER_H

#include <QtGui>

class QVTKWidget2;

class vtkActor;
class vtkActor2D;
class vtkImageData;
class vtkRenderer;

//class vtkmsqLookupTable;
//class vtkmsqCornerAnnotation;
class vtkmsqInteractorStyleImage;
//class vtkmsqMedicalImageProperties;
//class vtkmsqRectangleActor2D;
//class vtkmsqImageItem;

class MSQDicomImageViewer : public QWidget
{
Q_OBJECT

public:
  MSQDicomImageViewer();
  virtual ~MSQDicomImageViewer();

  void disableInteraction();
  void enableInteraction();

  void addActor(vtkActor *actor);
  void removeActor(vtkActor *actor);

  void addActor(vtkActor2D *actor);
  void removeActor(vtkActor2D *actor);

  void reset();
  void dolly(double factor);

  void refresh();

  virtual void setInput(const QString& fileName);

  //virtual double pick(double selectionX, double selectionY, double imageCoords[3]) = 0;
  //virtual void createOrientationMarkerWidgets() = 0;
	//virtual void enableOrientationMarkerWidget() { };
	//virtual void disableOrientationMarkerWidget() { };

  //virtual void setSlice(int axial, int sagittal, int coronal) = 0;
  //virtual void setInput(vtkmsqImageItem *imageItem) = 0;
  //virtual double* GetOrigin() = 0;
  //virtual double* GetPoint1() = 0;
  //virtual double* GetPoint2() = 0;

public slots:
  //virtual void setComponent(int component) = 0;
  //virtual void setLevel(double value) = 0;
  //virtual void setWindow(double value) = 0;
  //virtual void setOpacity(double value) = 0;

protected:
  QVTKWidget2 *vtkWidget;

  //vtkmsqRectangleActor2D *frame;
  vtkmsqInteractorStyleImage *interStyle;
  vtkRenderer *renderer;
  
  void buildFrame();
};

#endif

