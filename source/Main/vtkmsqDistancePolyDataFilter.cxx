/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqDistancePolyDataFilter.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqDistancePolyDataFilter.h"

#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkImplicitPolyDataDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangle.h"

vtkStandardNewMacro(vtkmsqDistancePolyDataFilter);

/***********************************************************************************//**
 * 
 */
vtkmsqDistancePolyDataFilter::vtkmsqDistancePolyDataFilter() : vtkPolyDataAlgorithm()
{
  this->SignedDistance = 1;
  this->NegateDistance = 0;

  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(1);

  vtkPolyData* output1 = vtkPolyData::New();
  this->GetExecutive()->SetOutputData(1, output1);
  output1->Delete();
}

/***********************************************************************************//**
 * 
 */
vtkmsqDistancePolyDataFilter::~vtkmsqDistancePolyDataFilter()
{
}

/***********************************************************************************//**
 * 
 */
int vtkmsqDistancePolyDataFilter::RequestData(vtkInformation* vtkNotUsed(request),
    vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* inInfo0 = inputVector[0]->GetInformationObject(0);
  vtkInformation* inInfo1 = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  if (!inInfo0 || !inInfo1 || !outInfo)
  {
    return 0;
  }
  vtkPolyData *input0 = vtkPolyData::SafeDownCast(inInfo0->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *input1 = vtkPolyData::SafeDownCast(inInfo1->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output0 = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!input0 || !input1 || !output0)
  {
    return 0;
  }

  output0->CopyStructure(input0);
  output0->GetPointData()->PassData(input0->GetPointData());
  output0->GetCellData()->PassData(input0->GetCellData());
  output0->BuildCells();
  this->GetPolyDataDistance(output0, input1);

  return 1;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqDistancePolyDataFilter::GetPolyDataDistance(vtkPolyData* mesh,
    vtkPolyData* src)
{
  vtkDebugMacro(<<"Start vtkmsqDistancePolyDataFilter::GetPolyDataDistance");

  if (mesh->GetNumberOfPolys() == 0 || mesh->GetNumberOfPoints() == 0)
  {
    vtkErrorMacro(<<"No points/cells to operate on");
    return;
  }

  if (src->GetNumberOfPolys() == 0 || src->GetNumberOfPoints() == 0)
  {
    vtkErrorMacro(<<"No points/cells to difference from");
    return;
  }

  vtkImplicitPolyDataDistance* imp = vtkImplicitPolyDataDistance::New();
  imp->SetInput(src);

  // Calculate distance from points.
  int numPts = mesh->GetNumberOfPoints();

  vtkDoubleArray* pointArray = vtkDoubleArray::New();
  pointArray->SetName("Distance");
  pointArray->SetNumberOfComponents(1);
  pointArray->SetNumberOfTuples(numPts);

  for (vtkIdType ptId = 0; ptId < numPts; ptId++)
  {
    UpdateProgress((double) ptId/(2*numPts));
    double pt[3];
    mesh->GetPoint(ptId, pt);
    double val = imp->EvaluateFunction(pt);
    double dist = SignedDistance ? (NegateDistance ? -val : val) : fabs(val);
    pointArray->SetValue(ptId, dist);
  }

  mesh->GetPointData()->AddArray(pointArray);
  pointArray->Delete();
  mesh->GetPointData()->SetActiveScalars("Distance");

  // Calculate distance from cell centers.
  int numCells = mesh->GetNumberOfCells();

  vtkDoubleArray* cellArray = vtkDoubleArray::New();
  cellArray->SetName("Distance");
  cellArray->SetNumberOfComponents(1);
  cellArray->SetNumberOfTuples(numCells);

  for (vtkIdType cellId = 0; cellId < numCells; cellId++)
  {
    UpdateProgress(0.5 + (double) cellId/(2*numCells));
    vtkCell *cell = mesh->GetCell(cellId);
    int subId;
    double pcoords[3], x[3], weights[256];

    cell->GetParametricCenter(pcoords);
    cell->EvaluateLocation(subId, pcoords, x, weights);

    double val = imp->EvaluateFunction(x);
    double dist = SignedDistance ? (NegateDistance ? -val : val) : fabs(val);
    cellArray->SetValue(cellId, dist);
  }

  mesh->GetCellData()->AddArray(cellArray);
  cellArray->Delete();
  mesh->GetCellData()->SetActiveScalars("Distance");

  imp->Delete();

  vtkDebugMacro(<<"End vtkmsqDistancePolyDataFilter::GetPolyDataDistance");
}

/***********************************************************************************//**
 * 
 */
int vtkmsqDistancePolyDataFilter::FillInputPortInformation(int port, vtkInformation *info)
{
  if (!this->Superclass::FillInputPortInformation(port, info))
  {
    return 0;
  }
  if (port == 0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  }
  else if (port == 1)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
  }
  return 1;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqDistancePolyDataFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "SignedDistance: " << this->SignedDistance << "\n";
  os << indent << "NegateDistance: " << this->NegateDistance << "\n";
}
