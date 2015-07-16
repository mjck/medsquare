/*=========================================================================

 Program:   MedSquare
 Module:    MSQSagittalRenderWidget.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_SAGITTAL_RENDER_WIDGET_H
#define MSQ_SAGITTAL_RENDER_WIDGET_H

#include "MSQ2DRenderWidget.h"

class MSQOrthogonalViewer;

class MSQSagittalRenderWidget : public MSQ2DRenderWidget
{
Q_OBJECT

public:
  MSQSagittalRenderWidget(MSQOrthogonalViewer *viewer);
  virtual ~MSQSagittalRenderWidget();

  virtual void createOrientationMarkerWidgets();
  virtual void setSlice(int axial, int sagittal, int coronal);
};

#endif
