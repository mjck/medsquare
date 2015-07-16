/*=========================================================================

 Program:   MedSquare
 Module:    MSQAxialRenderWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQAxialRenderWidget.h"

#include "QVTKWidget.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"

#include "vtkmsqAxialImagePlane.h"

#include "MSQOrientationColors.h"

/***********************************************************************************//**
 *
 */
MSQAxialRenderWidget::MSQAxialRenderWidget(MSQOrthogonalViewer *viewer) : MSQ2DRenderWidget(viewer)
{
  this->renderer->GetActiveCamera()->ParallelProjectionOn();
  this->imagePlane = vtkmsqAxialImagePlane::New();
  this->renderer->AddActor(this->imagePlane);
}

/***********************************************************************************//**
 *
 */
MSQAxialRenderWidget::~MSQAxialRenderWidget()
{
  this->imagePlane->Delete();
}

/***********************************************************************************//**
 *
 */
void MSQAxialRenderWidget::createOrientationMarkerWidgets()
{
  vtkTextActor *orientationLabel = vtkTextActor::New();
  orientationLabel->SetInput("Axi");
  orientationLabel->SetPosition(5, 5);
  orientationLabel->GetTextProperty()->SetFontFamilyToArial();
  orientationLabel->GetTextProperty()->SetBold(true);
  orientationLabel->GetTextProperty()->SetShadow(false);
  orientationLabel->GetTextProperty()->SetColor(MSQOrientationColors::axialColor);
  orientationLabel->GetTextProperty()->SetFontSize(12);
  this->renderer->AddActor(orientationLabel);
}

/***********************************************************************************//**
 *
 */
void MSQAxialRenderWidget::setSlice(int axial, int sagittal, int coronal)
{
  this->imagePlane->SetSliceNumber(axial);
  this->refresh();
}
