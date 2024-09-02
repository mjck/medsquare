/*=========================================================================

 Program:   MedSquare
 Module:    MSQCoronalRenderWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQCoronalRenderWidget.h"

#include "QVTKWidget.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"

#include "vtkmsqCoronalImagePlane.h"

#include "MSQOrientationColors.h"

/***********************************************************************************//**
 * 
 */
MSQCoronalRenderWidget::MSQCoronalRenderWidget(MSQOrthogonalViewer *viewer) : MSQ2DRenderWidget(viewer)
{
  this->renderer->GetActiveCamera()->Elevation(-90);
  this->renderer->GetActiveCamera()->OrthogonalizeViewUp();
  this->renderer->GetActiveCamera()->ParallelProjectionOn();

  this->imagePlane = vtkmsqCoronalImagePlane::New();
  this->renderer->AddActor(this->imagePlane);
}

/***********************************************************************************//**
 * 
 */
MSQCoronalRenderWidget::~MSQCoronalRenderWidget()
{
  this->imagePlane->Delete();
}

/***********************************************************************************//**
 * 
 */
void MSQCoronalRenderWidget::createOrientationMarkerWidgets()
{
  vtkTextActor *orientationLabel = vtkTextActor::New();
  orientationLabel->SetInput("Cor");
  orientationLabel->SetPosition(5, 5);
  orientationLabel->GetTextProperty()->SetFontFamilyToArial();
  orientationLabel->GetTextProperty()->SetBold(true);
  orientationLabel->GetTextProperty()->SetShadow(false);
  orientationLabel->GetTextProperty()->SetColor(MSQOrientationColors::coronalColor);
  orientationLabel->GetTextProperty()->SetFontSize(12);
  this->renderer->AddActor(orientationLabel);
}

/***********************************************************************************//**
 * 
 */
void MSQCoronalRenderWidget::setSlice(int axial, int sagittal, int coronal)
{
  this->imagePlane->SetSliceNumber(coronal);
  this->refresh();
}
