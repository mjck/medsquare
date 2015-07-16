/*=========================================================================

 Program:   MedSquare
 Module:    MSQ3DRenderWidget.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_3D_RENDER_WIDGET_H
#define MSQ_3D_RENDER_WIDGET_H

#include "vtkmsqImageItem.h"

#include "MSQRenderWidget.h"

class MSQOrthogonalViewer;

class vtkmsqImagePlane;
class vtkmsq3DOrientationMarkerWidget;

class MSQ3DRenderWidget : public MSQRenderWidget
{
Q_OBJECT

public:
  MSQ3DRenderWidget(MSQOrthogonalViewer *viewer);
  virtual ~MSQ3DRenderWidget();

  virtual double pick(double selectionX, double selectionY, double imageCoords[3]);
  virtual void createOrientationMarkerWidgets();
	virtual void enableOrientationMarkerWidget();
	virtual void disableOrientationMarkerWidget();

  virtual void setSlice(int axial, int sagittal, int coronal);
  virtual void setInput(vtkmsqImageItem *imageItem);
  double* GetOrigin();
  double* GetPoint1();
  double* GetPoint2();

public slots:
  virtual void setLevel(double value);
  virtual void setComponent(int component);
  virtual void setWindow(double value);
  virtual void setOpacity(double value);

private:
  vtkmsqImagePlane *imagePlanes[3];
  vtkmsq3DOrientationMarkerWidget *markerWidget;
};

#endif
