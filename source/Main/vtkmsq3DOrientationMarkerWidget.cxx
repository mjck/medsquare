/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsq3DOrientationMarkerWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsq3DOrientationMarkerWidget.h"

#include "MSQOrientationColors.h"

#include "vtkActor.h"
#include "vtkAxesActor.h"
#include "vtkCaptionActor2D.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkPropAssembly.h"
#include "vtkProperty.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkTextProperty.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsq3DOrientationMarkerWidget);
/** \endcond */

/***********************************************************************************//**
 * 
 */
vtkmsq3DOrientationMarkerWidget::vtkmsq3DOrientationMarkerWidget()
{
  vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
  axesActor->SetShaftTypeToCylinder();
  axesActor->SetTotalLength(1.0, 1.0, 1.0);
  axesActor->AxisLabelsOn();
  axesActor->SetCylinderRadius(0.07);
  axesActor->SetConeRadius(0.5);
  axesActor->GetZAxisTipProperty()->SetColor(MSQOrientationColors::axialColor);
  axesActor->GetZAxisShaftProperty()->SetColor(MSQOrientationColors::axialColor);

  axesActor->SetXAxisLabelText("Sag");
  axesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->ItalicOff();
  axesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOff();
  axesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(MSQOrientationColors::sagittalColor);

  axesActor->SetYAxisLabelText("Cor");
  axesActor->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ItalicOff();
  axesActor->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOff();
  axesActor->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(MSQOrientationColors::coronalColor);

  axesActor->SetZAxisLabelText("Axi");
  axesActor->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ItalicOff();
  axesActor->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOff();
  axesActor->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(MSQOrientationColors::axialColor);

  vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
  sphere->SetCenter(0, 0, 0);
  sphere->SetPhiResolution(24);
  sphere->SetThetaResolution(24);
  sphere->SetRadius(0.5);

  vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereMapper->SetInputConnection(sphere->GetOutputPort());

  vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();
  sphereActor->SetMapper(sphereMapper);

  vtkSmartPointer<vtkPropAssembly> props = vtkSmartPointer<vtkPropAssembly>::New();
  props->AddPart(axesActor);
  props->AddPart(sphereActor);

  this->markerWidget = vtkOrientationMarkerWidget::New();
  this->markerWidget->SetOrientationMarker(props);
  this->markerWidget->SetViewport(0.8, 0.0, 1.0, 0.20);
}

/***********************************************************************************//**
 * 
 */
vtkmsq3DOrientationMarkerWidget::~vtkmsq3DOrientationMarkerWidget()
{
  this->markerWidget->Delete();
}

/***********************************************************************************//**
 * 
 */
void vtkmsq3DOrientationMarkerWidget::SetInteractor(vtkRenderWindowInteractor *interactor)
{
  this->markerWidget->SetInteractor(interactor);
  this->markerWidget->EnabledOn();
  this->markerWidget->InteractiveOff();
}

/***********************************************************************************//**
 * 
 */
void vtkmsq3DOrientationMarkerWidget::SetEnabled(int enable)
{
	this->markerWidget->SetEnabled(enable);
}

/***********************************************************************************//**
 * 
 */
void vtkmsq3DOrientationMarkerWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->markerWidget->PrintSelf(os, indent);
}
