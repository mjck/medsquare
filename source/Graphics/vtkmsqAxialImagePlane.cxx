/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqAxialImagePlane.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqAxialImagePlane.h"

#include "vtkmsqFrameSource.h"
#include "vtkmsqMedicalImageProperties.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkImageActor.h"
#include "vtkPlaneSource.h"
#include "vtkActor.h"
#include "vtkCell.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsqAxialImagePlane);
/** \endcond */

/***********************************************************************************//**
 * \brief Constructs the plane setting its color to blue.
 */
vtkmsqAxialImagePlane::vtkmsqAxialImagePlane()
{
  this->FrameActor->GetProperty()->SetColor(0.0, 0.57, 0.78);
  this->SetOrientationToAxial();
}

/***********************************************************************************//**
 *
 */
vtkmsqAxialImagePlane::~vtkmsqAxialImagePlane()
{
}

/***********************************************************************************//**
 * \brief Deprecated function. Use vtkmsqImagePlane::UpdateCoords instead.
 */
void vtkmsqAxialImagePlane::UpdateCoords(vtkSmartPointer<vtkMatrix4x4> resliceMatrix)
{
  // Get image information
  int extent[6];
  double spacing[3], center[3], origin[3];

  center[0] = resliceMatrix->GetElement(0, 3);
  center[1] = resliceMatrix->GetElement(1, 3);
  center[2] = resliceMatrix->GetElement(2, 3);

  this->InputImage->GetExtent(extent);
  this->InputImage->GetSpacing(spacing);
  this->InputImage->GetOrigin(origin);

  // update frame coords
   this->FrameSource->SetOrigin(origin[0], origin[1], center[2]);
   this->FrameSource->SetPoint1(origin[0] + spacing[0] * (extent[0] + extent[1]),
       origin[1], center[2]);
   this->FrameSource->SetPoint2(origin[0],
       origin[1] + spacing[1] * (extent[2] + extent[3]), center[2]);

   // update plane coords
   this->PlaneSource->SetOrigin(origin[0], origin[1], center[2]);
   this->PlaneSource->SetPoint1(origin[0] + spacing[0] * (extent[0] + extent[1]),
       origin[1], center[2]);
   this->PlaneSource->SetPoint2(origin[0],
       origin[1] + spacing[1] * (extent[2] + extent[3]), center[2]);
}

/***********************************************************************************//**
 *
 */
vtkSmartPointer<vtkMatrix4x4> vtkmsqAxialImagePlane::GetReslicingMatrix(int slice)
{
  // Get image information
  int extent[6];
  double spacing[3], center[3], origin[3];
  static double zElements[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

  this->InputImage->GetExtent(extent);
  this->InputImage->GetSpacing(spacing);
  this->InputImage->GetOrigin(origin);

  vtkSmartPointer<vtkMatrix4x4> resliceAxes = vtkSmartPointer<vtkMatrix4x4>::New();

  center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
  center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
  if (extent[5] == 0)
  {
    center[2] = origin[2];
  }
  else
  {
    center[2] = origin[2]
        + spacing[2] * ((float) slice / (extent[4] + extent[5]))
            * (extent[4] + extent[5]);
  }

  resliceAxes->DeepCopy(zElements);
  resliceAxes->SetElement(0, 3, center[0]);
  resliceAxes->SetElement(1, 3, center[1]);
  resliceAxes->SetElement(2, 3, center[2]);

  return resliceAxes;
}

/***********************************************************************************//**
 *
 */
void vtkmsqAxialImagePlane::AdjustPickPosition(double pickedPosition[3])
{
  double *bounds;
  bounds = this->ImageActor->GetBounds();
  pickedPosition[2] = bounds[4];
}

/***********************************************************************************//**
 *
 */
void vtkmsqAxialImagePlane::ComputePickedImageCoordinates(double pickedPosition[3], double imageCoords[3])
{
  double origin[3];
  double spacing[3];
  int extent[6];

  this->InputImage->GetOrigin(origin);
  this->InputImage->GetSpacing(spacing);
  this->InputImage->GetExtent(extent);
  if (MSQ_REORIENT){
    this->InputProperties->GetReorientedDouble3(spacing, spacing);
    this->InputProperties->GetReorientedDouble3(origin, origin, 0);
    this->InputProperties->GetReorientedExtent(extent, extent);
  }


  imageCoords[0] = (int) ((pickedPosition[0] - origin[0]) / spacing[0] + 0.5);
  imageCoords[1] = (int) ((pickedPosition[1] - origin[1]) / spacing[1] + 0.5);
  imageCoords[2] = this->SliceNumber;


  if (MSQ_REORIENT){
    vtkSmartPointer<vtkMatrix4x4> dircosMatrix = this->InputProperties->GetDirectionCosineMatrixPerpendicular();
    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 3; j++){
        if (dircosMatrix->GetElement(i, j) < 0 && j != 2) // j != SliceNumber
        {
          imageCoords[j] = imageCoords[j] + extent[2*j+1];
        }
      }
    }
  }

}

/***********************************************************************************//**
 *
 */
void vtkmsqAxialImagePlane::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

