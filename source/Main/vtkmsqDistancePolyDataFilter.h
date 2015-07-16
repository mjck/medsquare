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

#ifndef VTK_DISTANCE_POLY_DATA_FILTER_H
#define VTK_DISTANCE_POLY_DATA_FILTER_H

#include "vtkPolyDataAlgorithm.h"

class vtkmsqDistancePolyDataFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkmsqDistancePolyDataFilter *New();vtkTypeMacro(vtkmsqDistancePolyDataFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Enable/disable computation of the signed distance between
  // the first poly data and the second poly data. Defaults to on.
  vtkSetMacro(SignedDistance, int);
  vtkGetMacro(SignedDistance, int);
  vtkBooleanMacro(SignedDistance, int);

  // Description:
  // Enable/disable negation of the distance values. Defaults to
  // off. Has no effect if SignedDistance is off.
  vtkSetMacro(NegateDistance, int);
  vtkGetMacro(NegateDistance, int);
  vtkBooleanMacro(NegateDistance, int);

protected:
  vtkmsqDistancePolyDataFilter();
  ~vtkmsqDistancePolyDataFilter();

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  int FillInputPortInformation(int, vtkInformation*);

  void GetPolyDataDistance(vtkPolyData*, vtkPolyData*);

private:
  vtkmsqDistancePolyDataFilter(const vtkmsqDistancePolyDataFilter&); // Not implemented
  void operator=(const vtkmsqDistancePolyDataFilter&); // Not implemented

  int SignedDistance;
  int NegateDistance;
};

#endif
