/*=========================================================================

 Program:   MedSquare
 Module:    MSQ3DRenderWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQ3DRenderWidget.h"

#include "QVTKWidget2.h"

#include "vtkCamera.h"
#include "vtkCaptionActor2D.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "vtkSmartPointer.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"

#include "vtkmsq3DOrientationMarkerWidget.h"
#include "vtkmsqAxialImagePlane.h"
#include "vtkmsqCoronalImagePlane.h"
#include "vtkmsqInteractorStyleImage3D.h"
#include "vtkmsqSagittalImagePlane.h"
#include "vtkmsqRectangleActor2D.h"

/***********************************************************************************//**
 *
 */
MSQ3DRenderWidget::MSQ3DRenderWidget(MSQOrthogonalViewer *viewer)
{
  this->renderer->GetActiveCamera()->ParallelProjectionOn();
  this->renderer->GetActiveCamera()->Elevation(-90);
  this->renderer->GetActiveCamera()->OrthogonalizeViewUp();
  this->renderer->GetActiveCamera()->Azimuth(135);
  this->renderer->GetActiveCamera()->OrthogonalizeViewUp();
  this->renderer->GetActiveCamera()->Pitch(-30);

  this->imagePlanes[0] = vtkmsqSagittalImagePlane::New();
  this->imagePlanes[1] = vtkmsqCoronalImagePlane::New();
  this->imagePlanes[2] = vtkmsqAxialImagePlane::New();

  this->renderer->AddActor(this->imagePlanes[0]);
  this->renderer->AddActor(this->imagePlanes[1]);
  this->renderer->AddActor(this->imagePlanes[2]);

  this->interStyle = vtkmsqInteractorStyleImage3D::New();
  this->interStyle->SetOrthogonalViewer(viewer);
}

/***********************************************************************************//**
 *
 */
MSQ3DRenderWidget::~MSQ3DRenderWidget()
{
  this->imagePlanes[0]->Delete();
  this->imagePlanes[1]->Delete();
  this->imagePlanes[2]->Delete();
  this->markerWidget->Delete();
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::setOpacity(double value)
{
  this->imagePlanes[0]->SetOpacity(value);
  this->imagePlanes[1]->SetOpacity(value);
  this->imagePlanes[2]->SetOpacity(value);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::createOrientationMarkerWidgets()
{
  vtkTextActor *orientationLabel = vtkTextActor::New();
  orientationLabel->SetInput("3D");
  orientationLabel->SetPosition(5, 5);
  orientationLabel->GetTextProperty()->SetFontFamilyToArial();
  orientationLabel->GetTextProperty()->SetBold(true);
  orientationLabel->GetTextProperty()->SetShadow(false);
  orientationLabel->GetTextProperty()->SetFontSize(12);
  this->renderer->AddActor(orientationLabel);

  this->markerWidget = vtkmsq3DOrientationMarkerWidget::New();
  this->markerWidget->SetInteractor(this->vtkWidget->GetRenderWindow()->GetInteractor());
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::enableOrientationMarkerWidget()
{
	this->markerWidget->SetEnabled(1);
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::disableOrientationMarkerWidget()
{
	this->markerWidget->SetEnabled(0);
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::setLevel(double value)
{
  this->imagePlanes[0]->SetLevel(value);
  this->imagePlanes[1]->SetLevel(value);
  this->imagePlanes[2]->SetLevel(value);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::setComponent(int component)
{
  this->imagePlanes[0]->SetActiveComponent(component);
  this->imagePlanes[1]->SetActiveComponent(component);
  this->imagePlanes[2]->SetActiveComponent(component);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::setWindow(double value)
{
  this->imagePlanes[0]->SetWindow(value);
  this->imagePlanes[1]->SetWindow(value);
  this->imagePlanes[2]->SetWindow(value);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::setInput(vtkmsqImageItem *imageItem)
{
  this->imagePlanes[0]->SetInput(imageItem);
  this->imagePlanes[1]->SetInput(imageItem);
  this->imagePlanes[2]->SetInput(imageItem);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ3DRenderWidget::setSlice(int axial, int sagittal, int coronal)
{
  this->imagePlanes[0]->SetSliceNumber(sagittal);
  this->imagePlanes[1]->SetSliceNumber(coronal);
  this->imagePlanes[2]->SetSliceNumber(axial);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
double MSQ3DRenderWidget::pick(double selectionX, double selectionY, double imageCoords[3])
{
  return -1;
}

double* MSQ3DRenderWidget::GetOrigin()
{
  return NULL;
}

double* MSQ3DRenderWidget::GetPoint1()
{
  return NULL;
}

double* MSQ3DRenderWidget::GetPoint2()
{
  return NULL;
}
