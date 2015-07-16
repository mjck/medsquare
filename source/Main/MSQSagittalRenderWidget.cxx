/*=========================================================================

 Program:   MedSquare
 Module:    MSQSagittalRenderWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQSagittalRenderWidget.h"

#include "QVTKWidget.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"

#include "vtkmsqSagittalImagePlane.h"

#include "MSQOrientationColors.h"

/***********************************************************************************//**
 * 
 */
MSQSagittalRenderWidget::MSQSagittalRenderWidget(MSQOrthogonalViewer *viewer) : MSQ2DRenderWidget(viewer)
{
  this->renderer->GetActiveCamera()->ParallelProjectionOn();
  this->renderer->GetActiveCamera()->Azimuth(-90);
  this->renderer->GetActiveCamera()->Elevation(180);
  this->renderer->GetActiveCamera()->Roll(-90);
  this->imagePlane = vtkmsqSagittalImagePlane::New();
  this->renderer->AddActor(this->imagePlane);
}

/***********************************************************************************//**
 * 
 */
MSQSagittalRenderWidget::~MSQSagittalRenderWidget()
{
  this->imagePlane->Delete();
}

/***********************************************************************************//**
 * 
 */
void MSQSagittalRenderWidget::createOrientationMarkerWidgets()
{
  vtkTextActor *orientationLabel = vtkTextActor::New();
  orientationLabel->SetInput("Sag");
  orientationLabel->SetPosition(5, 3);
  orientationLabel->GetTextProperty()->SetFontFamilyToArial();
  orientationLabel->GetTextProperty()->SetBold(true);
  orientationLabel->GetTextProperty()->SetShadow(false);
  orientationLabel->GetTextProperty()->SetColor(MSQOrientationColors::sagittalColor);
  orientationLabel->GetTextProperty()->SetFontSize(12);
  this->renderer->AddActor(orientationLabel);
}

/***********************************************************************************//**
 * 
 */
void MSQSagittalRenderWidget::setSlice(int axial, int sagittal, int coronal)
{
  this->imagePlane->SetSliceNumber(sagittal);
  this->refresh();
}
