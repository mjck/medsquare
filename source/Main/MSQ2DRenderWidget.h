/*=========================================================================

 Program:   MedSquare
 Module:    MSQ2DRenderWidget.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_2D_RENDER_WIDGET_H
#define MSQ_2D_RENDER_WIDGET_H

#include "vtkmsqImageItem.h"

#include "MSQRenderWidget.h"

class MSQOrthogonalViewer;
class vtkmsqImagePlane;

class MSQ2DRenderWidget : public MSQRenderWidget
{
Q_OBJECT

public:
  MSQ2DRenderWidget(MSQOrthogonalViewer *viewer);
  virtual ~MSQ2DRenderWidget();

  virtual double pick(double selectionX, double selectionY, double imageCoords[3]);
  virtual void createOrientationMarkerWidgets() = 0;

  virtual void setSlice(int axial, int sagittal, int coronal) = 0;
  virtual void setInput(vtkmsqImageItem *imageItem);
  virtual double* GetOrigin();
  virtual double* GetPoint1();
  virtual double* GetPoint2();
  //void refresh();

public slots:
  virtual void setComponent(int component);
  virtual void setLevel(double value);
  virtual void setWindow(double value);
  virtual void setOpacity(double value);

protected:
  vtkmsqImagePlane *imagePlane;
};

#endif
