/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqInteractorStyleImage.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqInteractorStyleImage.h"

#include "MSQOrthogonalViewer.h"

#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsqInteractorStyleImage);
/** \endcond */

/***********************************************************************************//**
 * 
 */
vtkmsqInteractorStyleImage::vtkmsqInteractorStyleImage()
{
}

/***********************************************************************************//**
 * 
 */
vtkmsqInteractorStyleImage::~vtkmsqInteractorStyleImage()
{
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::SetOrthogonalViewer(MSQOrthogonalViewer *viewer)
{
  this->OrthoViewer = viewer;
}

/***********************************************************************************//**
 * 
 */
MSQOrthogonalViewer* vtkmsqInteractorStyleImage::GetOrthogonalViewer()
{
  return this->OrthoViewer;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::OnLeftButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)
  {
    return;
  }

  this->GetOrthogonalViewer()->setCurrentRenderer(this->CurrentRenderer);

  if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
  {
    if (this->Interactor->GetRepeatCount() == 1)
    {
      this->toggleMaximizeViewport(this->CurrentRenderer);
    }
  }

  this->Superclass::OnLeftButtonDown();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::OnLeftButtonUp()
{
  this->Superclass::OnLeftButtonUp();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::OnRightButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)
  {
    return;
  }

  this->GetOrthogonalViewer()->setCurrentRenderer(this->CurrentRenderer);

  this->GrabFocus(this->EventCallbackCommand);
  if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
  {
    this->WindowLevelStartPosition[0] = x;
    this->WindowLevelStartPosition[1] = y;
    this->StartWindowLevel();
  }
  else
  {
    this->Superclass::OnRightButtonDown();
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::OnRightButtonUp()
{
  switch (this->State)
  {
    case VTKIS_WINDOW_LEVEL:
      this->EndWindowLevel();
      if (this->Interactor)
      {
        this->ReleaseFocus();
      }
      break;
    default:
      this->Superclass::OnRightButtonUp();
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::OnMouseMove()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  switch (this->State)
  {
    case VTKIS_WINDOW_LEVEL:
      this->FindPokedRenderer(x, y);
      this->updateWindowLevel(this->CurrentRenderer, x, y);
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;
    default:
      this->Superclass::OnMouseMove();
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::StartWindowLevel()
{
  if (this->State != VTKIS_NONE)
  {
    return;
  }

  this->InitialWindow = this->GetOrthogonalViewer()->getWindow();
  this->InitialLevel = this->GetOrthogonalViewer()->getLevel();
  this->StartState(VTKIS_WINDOW_LEVEL);
  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::EndWindowLevel()
{
  if (this->State != VTKIS_WINDOW_LEVEL)
  {
    return;
  }
  this->ResetWindowLevel();
  this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
  this->StopState();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::toggleMaximizeViewport(vtkRenderer *pokedRenderer)
{
  this->GetOrthogonalViewer()->toggleMaximizeWindow(pokedRenderer);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::updateWindowLevel(vtkRenderer *pokedRenderer, int x,
    int y)
{
  if (!this->GetOrthogonalViewer()->hasImageLoaded())
  {
    return;
  }

  int *size = pokedRenderer->GetRenderWindow()->GetSize();
  int window = static_cast<int> ( InitialWindow );
  int level  = static_cast<int> ( InitialLevel  );

  // compute normalized delta
  double dx = 4.0 * (x - this->WindowLevelStartPosition[0]) / size[0];
  double dy = 4.0 * (this->WindowLevelStartPosition[1] - y) / size[1];

  // scale by current values
  if (fabs(window) > 0.01)
  {
    dx = dx * window;
  }
  else
  {
    dx = dx * (window < 0 ? -0.01 : 0.01);
  }
  if (fabs(level) > 0.01)
  {
    dy = dy * level;
  }
  else
  {
    dy = dy * (level < 0 ? -0.01 : 0.01);
  }

  // abs so that direction does not flip
  if (window < 0.0)
  {
    dx = -1 * dx;
  }
  if (level < 0.0)
  {
    dy = -1 * dy;
  }

  // compute new window level
  double newWindow = dx + window;
  double newLevel = level - dy;

  // stay away from zero and really
  if (fabs(newWindow) < 0.01)
  {
    newWindow = (newWindow < 0 ? -1 : 1);
  }

  this->GetOrthogonalViewer()->setWindow(newWindow);
  this->GetOrthogonalViewer()->setLevel(newLevel);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::ResetWindowLevel()
{
  this->WindowLevelStartPosition[0] = 0;
  this->WindowLevelStartPosition[1] = 0;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

