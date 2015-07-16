/*=========================================================================

 Program:   MedSquare
 Module:    MSQ2DRenderWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQ2DRenderWidget.h"

#include "QVTKWidget.h"

#include "vtkCamera.h"
#include "vtkLogLookupTable.h"
#include "vtkRenderer.h"

#include "vtkmsqImagePlane.h"
#include "vtkmsqInteractorStyleImage2D.h"

/***********************************************************************************//**
 *
 */
MSQ2DRenderWidget::MSQ2DRenderWidget(MSQOrthogonalViewer *viewer)
{
  this->interStyle = vtkmsqInteractorStyleImage2D::New();
  this->interStyle->SetOrthogonalViewer(viewer);
}

/***********************************************************************************//**
 *
 */
MSQ2DRenderWidget::~MSQ2DRenderWidget()
{
}

/***********************************************************************************//**
 *
 */
void MSQ2DRenderWidget::setOpacity(double value)
{
  this->imagePlane->SetOpacity(value);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ2DRenderWidget::setLevel(double value)
{
  this->imagePlane->SetLevel(value);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ2DRenderWidget::setComponent(int component)
{
  this->imagePlane->SetActiveComponent(component);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ2DRenderWidget::setWindow(double value)
{
  this->imagePlane->SetWindow(value);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQ2DRenderWidget::setInput(vtkmsqImageItem *imageItem)
{
  this->imagePlane->SetInput(imageItem);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
double MSQ2DRenderWidget::pick(double selectionX, double selectionY, double imageCoords[3])
{
  return this->imagePlane->Pick(selectionX, selectionY, this->renderer, imageCoords);
}

double* MSQ2DRenderWidget::GetOrigin()
{
  return this->imagePlane->GetOrigin();
}

double* MSQ2DRenderWidget::GetPoint1()
{
  return this->imagePlane->GetPoint1();
}

double* MSQ2DRenderWidget::GetPoint2()
{
  return this->imagePlane->GetPoint2();
}
