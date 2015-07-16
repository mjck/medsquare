/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqInteractorStyleImage3D.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqInteractorStyleImage3D.h"

#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkCoordinate.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsqInteractorStyleImage3D);
/** \endcond */

/***********************************************************************************//**
 * 
 */
vtkmsqInteractorStyleImage3D::vtkmsqInteractorStyleImage3D()
{
  this->ResetWindowLevel();
}

/***********************************************************************************//**
 * 
 */
vtkmsqInteractorStyleImage3D::~vtkmsqInteractorStyleImage3D()
{
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage3D::OnLeftButtonDown()
{
  // Add any special handling here
  this->Superclass::OnLeftButtonDown();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage3D::OnLeftButtonUp()
{
  // Add any special handling here
  this->Superclass::OnLeftButtonUp();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqInteractorStyleImage3D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

