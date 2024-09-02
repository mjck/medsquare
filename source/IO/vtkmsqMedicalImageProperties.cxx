/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqMedicalImageProperties.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqMedicalImageProperties.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"

/** \cond 0 */
//vtkCxxRevisionMacro(vtkmsqMedicalImageProperties, "0.1");
vtkStandardNewMacro(vtkmsqMedicalImageProperties);
/** \endcond */

/***********************************************************************************//**
 * 
 */
vtkmsqMedicalImageProperties::vtkmsqMedicalImageProperties() :
    vtkMedicalImageProperties()
{
  this->Superclass::SetInstanceUIDFromSliceID(0, 0, "");
  this->NumberOfGradientValues = 0;
  this->NumberOfBValues = 0;
  this->DiffusionGradients = vtkDoubleArray::New();
  this->DiffusionGradients->SetNumberOfComponents(4);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqMedicalImageProperties::DeepCopy(vtkMedicalImageProperties *src)
{
  if (src == NULL)
    {
      return;
    }
  
  this->Superclass::DeepCopy(src);
  vtkmsqMedicalImageProperties *msq = vtkmsqMedicalImageProperties::SafeDownCast(src);
    if (!msq)
      {
	// Just return without copying metadata
	return;
      }
    
  int num_grads = msq->GetNumberOfGradientValues();
  this->SetNumberOfGradientValues(num_grads);
  this->SetNumberOfBValues(msq->GetNumberOfBValues());
  double vals[4];
  for(int i=0;i<num_grads;i++) {
    msq->GetNthDiffusionGradient(i, vals);
    this->AddDiffusionGradient(vals);
  }
}


/***********************************************************************************//**
 * 
 */
vtkmsqMedicalImageProperties::~vtkmsqMedicalImageProperties()
{
  this->NumberOfGradientValues = 0;
  this->NumberOfBValues = 0;
  if (this->DiffusionGradients)
    this->DiffusionGradients->Delete();

  this->Clear();
}

/***********************************************************************************//**
 * 
 */
void vtkmsqMedicalImageProperties::AddDiffusionGradient(std::vector<float> gradient, float bvalue)
{
  double vals[4] = {gradient[0], gradient[1], gradient[2], bvalue};
  this->DiffusionGradients->InsertNextTuple(vals);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqMedicalImageProperties::AddDiffusionGradient(double *values)
{
  this->DiffusionGradients->InsertNextTuple(values);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqMedicalImageProperties::GetNthDiffusionGradient(int idx, double *values)
{
  this->DiffusionGradients->GetTuple(idx, values);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqMedicalImageProperties::SetOrientationType(int orientation)
{
  // It would be better if could verify below is true before setting instance
  this->Superclass::SetInstanceUIDFromSliceID(0, 0, "");
  this->Superclass::SetOrientationType(0, orientation);
}

/***********************************************************************************//**
 * 
 */
int vtkmsqMedicalImageProperties::GetOrientationType()
{
  return this->Superclass::GetOrientationType(0);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqMedicalImageProperties::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

/***********************************************************************************//**
 * 
 */
vtkSmartPointer<vtkMatrix4x4> vtkmsqMedicalImageProperties::GetDirectionCosineMatrix()
{
  double dircos[6];
  double dircosz[3];
  vtkSmartPointer<vtkMatrix4x4> dircosMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

  this->Superclass::GetDirectionCosine(dircos);

  vtkMath::Cross(&dircos[0], &dircos[3], dircosz);

  double dircosMatrixD[16] = { dircos[0],  dircos[1],   dircos[2],  0,
                               dircos[3],  dircos[4],   dircos[5],  0,
                               dircosz[0], dircosz[1],  dircosz[2], 0,
                               0,          0,           0,          1 };
  dircosMatrix->DeepCopy(dircosMatrixD);

  return dircosMatrix;
}

/***********************************************************************************//**
 * 
 */
vtkMatrix4x4* vtkmsqMedicalImageProperties::GetDirectionCosineMatrixPerpendicular()
{
  vtkSmartPointer<vtkMatrix4x4> dircosMatrix = this->GetDirectionCosineMatrix();
  vtkMatrix4x4* dircosMatrixPerp = vtkMatrix4x4::New();

  int i = 0;
  int j = 0;

  int maxj = 0;
  double maxvalj = fabs(dircosMatrix->GetElement(i, maxj));
  double val;

  for(j = 0; j < 3; j++){
    val = fabs(dircosMatrix->GetElement(i, j));
    if (val > maxvalj){
      maxvalj = val;
      maxj = j;
    }
  }
  for(j = 0; j < 3; j++){
    if (maxj == j){
      if (dircosMatrix->GetElement(i, maxj) < 0.0){
        dircosMatrixPerp->SetElement(i, j, -1.0);
      }
      else{
        dircosMatrixPerp->SetElement(i, j, 1.0);
      }
    }
    else{
      dircosMatrixPerp->SetElement(i, j, 0.0);
    }
  }
  

  for(i = 1; i < 3; i++){
    maxj = -1;
    maxvalj = -99999.9;
    for(j = 0; j < 3; j++){
      val = fabs(dircosMatrix->GetElement(i, j));
      if (val > maxvalj && dircosMatrixPerp->GetElement(i-1, j) == 0.0){
        maxvalj = val;
        maxj = j;
      }
    }
    for(j = 0; j < 3; j++){
      if (maxj == j){
        if (dircosMatrix->GetElement(i, maxj) < 0.0){
          dircosMatrixPerp->SetElement(i, j, -1.0);
        }
        else{
          dircosMatrixPerp->SetElement(i, j, 1.0);
        }
      }
      else{
        dircosMatrixPerp->SetElement(i, j, 0.0);
      }
    }
  }

  return dircosMatrixPerp;
}

/***********************************************************************************//**
 *
 *  Return array with reoriented extent of input image according to directionCosineMatrixPerpendicular.
 *
 *  The first parameter is the original extent, and the second is used to output the 
 *  reoriented extent.
 *  
 *  The third parameter indicates if the extent must be absolute or not. When true, the
 *  output extent will be always greater than zero.
 * 
 *  The extent refers to the voxel coordinates. The input data coordinates is different. 
 *  The voxel (0,0,0) corresponds to the origin of the input data. 
 *  
 *  Let X be a point in the input data space.
 *  Let X' be a point in the visualization space.
 *  Let D be directionCosineMatrixPerpendicular.
 *  Then X = M X'
 *  or equivalently X' = Transposed(M) X
 *  As M is a rotation matrix, its inverse is its transposed.
 *  
 *  Multiplying Transposed(M) by the extent minima and maxima will give the extents of 
 *  the voxel coordinates in the visualization space.
 *
 */
void vtkmsqMedicalImageProperties::GetReorientedExtent(int original[6], int reoriented[6], int absolute)
{
  int buffer[6];

  vtkMatrix4x4* dircosMatrix = this->GetDirectionCosineMatrixPerpendicular();

  for(int i = 0; i < 6; i++){
    buffer[i] = 0;
  }

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      buffer[2*j]   += static_cast<int> ( dircosMatrix->GetElement(i, j) * original[2*i] );
      buffer[2*j+1] += static_cast<int> ( dircosMatrix->GetElement(i, j) * original[2*i+1] );
    }
  }

  for(int i = 0; i < 6; i++){
    if (buffer[i] < 0 && absolute)
      reoriented[i] = -buffer[i];
    else
      reoriented[i] = buffer[i];
  }
}

/***********************************************************************************//**
 *
 *  Return array of 3 doubles reoriented according to directionCosineMatrixPerpendicular.
 *
 *  The first parameter is the original array, and the second is used to output the 
 *  reoriented array.
 *  
 *  The third parameter indicates if the values must be absolute or not. When true, the
 *  output values will be always greater than zero.
 * 
 *  Can be used to reorient the origin, spacing or coordinates in the input data space
 * 
 *  Let X be a point in the input data space.
 *  Let X' be a point in the visualization space.
 *  Let D be directionCosineMatrixPerpendicular.
 *  Then X = M X'
 *  or equivalently X' = Transposed(M) X
 *  As M is a rotation matrix, its inverse is its transposed.
 * 
 */
void vtkmsqMedicalImageProperties::GetReorientedDouble3(double original[3], double reoriented[3], int absolute)
{
  double buffer[3];

  vtkMatrix4x4* dircosMatrix = this->GetDirectionCosineMatrixPerpendicular();

  for(int i = 0; i < 3; i++){
    buffer[i] = 0;
  }

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      buffer[j]   += dircosMatrix->GetElement(i, j) * original[i];
    }
  }

  for(int i = 0; i < 3; i++){
    if (buffer[i] < 0.0 && absolute){
      reoriented[i] = -buffer[i];
    }
    else
    {
      reoriented[i] = buffer[i];
    }
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqMedicalImageProperties::GetOriginalDouble3(double reoriented[3], double original[3])
{
    vtkSmartPointer<vtkMatrix4x4> dircosMatrix = this->GetDirectionCosineMatrixPerpendicular();

    double reoriented4[4];
    double original4[4];
    for(int i = 0; i < 3; i++){
      reoriented4[i] = reoriented[i];
    }
    reoriented4[3] = 1.0;

    dircosMatrix->MultiplyPoint(reoriented4, original4);
    for(int i = 0; i < 3; i++){
      original[i] = original4[i];
    }
}
