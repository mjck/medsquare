/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqInteractorStyleImage2D.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqInteractorStyleImage2D.h"

#include "MSQOrthogonalViewer.h"

#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkCoordinate.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsqInteractorStyleImage2D);
/** \endcond */

/***********************************************************************************//**
 * 
 */
vtkmsqInteractorStyleImage2D::vtkmsqInteractorStyleImage2D()
{
  this->ResetWindowLevel();
}

/***********************************************************************************//**
 * 
 */
vtkmsqInteractorStyleImage2D::~vtkmsqInteractorStyleImage2D()
{
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::OnLeftButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)
  {
    return;
  }

  this->GrabFocus(this->EventCallbackCommand);

  if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
  {
    this->StartClickedVoxel();
    this->GetOrthogonalViewer()->updateSlices(this->CurrentRenderer, x, y);
  }

  this->Superclass::OnLeftButtonDown();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::OnLeftButtonUp()
{
  switch (this->State)
  {
    case VTKIS_CLICKED_VOXEL:
      this->EndClickedVoxel();
      if (this->Interactor)
      {
        this->ReleaseFocus();
      }
      break;
    default:
      this->Superclass::OnLeftButtonUp();
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::OnMouseWheelForward()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)
  {
    return;
  }

  if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
  {
    this->GetOrthogonalViewer()->incrementSlice(+1, this->CurrentRenderer);
  }
  else
  {
    this->Superclass::OnMouseWheelForward();
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::OnMouseWheelBackward()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)
  {
    return;
  }

  if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
  {
    this->GetOrthogonalViewer()->incrementSlice(-1, this->CurrentRenderer);
  }
  else
  {
    this->Superclass::OnMouseWheelBackward();
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::OnMouseMove()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  switch (this->State)
  {
    case VTKIS_CLICKED_VOXEL:
      this->FindPokedRenderer(x, y);
      this->GetOrthogonalViewer()->updateSlices(this->CurrentRenderer, x, y);
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;
    default:
      this->Superclass::OnMouseMove();
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::StartClickedVoxel()
{
  if (this->State != VTKIS_NONE)
  {
    return;
  }
  this->StartState(VTKIS_CLICKED_VOXEL);
  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::EndClickedVoxel()
{
  if (this->State != VTKIS_CLICKED_VOXEL)
  {
    return;
  }
  this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
  this->StopState();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
