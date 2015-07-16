/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsq3DOrientationMarkerWidget.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef VTKMSQ_3D_ORIENTATION_MARKER_WIDGET_H
#define VTKMSQ_3D_ORIENTATION_MARKER_WIDGET_H

#include "vtkObject.h"

class vtkOrientationMarkerWidget;
class vtkRenderWindowInteractor;

class vtkmsq3DOrientationMarkerWidget : public vtkObject
{
public:
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkmsq3DOrientationMarkerWidget *New();

  void SetInteractor(vtkRenderWindowInteractor *interactor);
  void SetEnabled(int enable);

protected:
  vtkmsq3DOrientationMarkerWidget();
  virtual ~vtkmsq3DOrientationMarkerWidget();

private:
  vtkOrientationMarkerWidget *markerWidget;

  vtkmsq3DOrientationMarkerWidget(const vtkmsq3DOrientationMarkerWidget&); // Not implemented.
  void operator=(const vtkmsq3DOrientationMarkerWidget&); // Not implemented.
};

#endif
