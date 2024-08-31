/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryDifferenceAction.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQGeometryDifference.h"

#include "MedSquare.h"

#include "MSQGeometryDialog.h"
#include "MSQGeometryDifferenceItem.h"

#include "vtkmsqDistancePolyDataFilter.h"

#include "vtkActor.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkScalarBarActor.h"
#include "vtkSmartPointer.h"
#include "vtkTextProperty.h"

MSQGeometryDifference::MSQGeometryDifference(MedSquare *medSquare, QHash<QString, MSQGeometryItem*> geometries) : QObject(medSquare)
{
  MSQGeometryItem *geometry1 = MSQGeometryDialog::getGeometryItem(medSquare, tr("Reference"), tr("Geometry"), geometries.values());
  MSQGeometryItem *geometry2 = MSQGeometryDialog::getGeometryItem(medSquare, tr("Other"), tr("Geometry"), geometries.values());

  medSquare->updateStatusBar(tr("Calculating difference..."), true);

  this->distanceFilter = vtkmsqDistancePolyDataFilter::New();

  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  connection->Connect(distanceFilter, vtkCommand::ProgressEvent, medSquare, SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

  distanceFilter->SetInputData(0, geometry1->getPolyData());
  distanceFilter->SetInputData(1, geometry2->getPolyData());
  distanceFilter->Update();

  this->mapper = vtkPolyDataMapper::New();
  this->mapper->SetInputConnection(this->distanceFilter->GetOutputPort());

  double *range = this->distanceFilter->GetOutput()->GetPointData()->GetScalars()->GetRange();
  this->mapper->SetScalarRange(range[0], range[1]);

  this->differenceActor = vtkActor::New();
  this->differenceActor->SetMapper(mapper);

  this->scalarBar = vtkScalarBarActor::New();
  this->scalarBar->SetLookupTable(mapper->GetLookupTable());
  this->scalarBar->SetOrientationToVertical();
  this->scalarBar->SetNumberOfLabels(5);
  this->scalarBar->SetPosition(0.82, 0.25);
  this->scalarBar->GetLabelTextProperty()->SetFontSize(8);

  vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
  lookupTable->SetTableRange(0, 1);
  lookupTable->SetHueRange(0.6, 0.0);
  lookupTable->SetSaturationRange(1.0, 1.0);
  lookupTable->SetValueRange(1.0, 1.0);
  lookupTable->SetAlphaRange(1.0, 1.0);
  lookupTable->SetNumberOfTableValues(1024000);
  lookupTable->Build();

  this->mapper->SetLookupTable(lookupTable);
  this->scalarBar->SetLookupTable(lookupTable);

  medSquare->updateStatusBar(tr("Ready"), false);
}

MSQGeometryDifference::~MSQGeometryDifference()
{
  this->differenceActor->Delete();
  this->distanceFilter->Delete();
  this->mapper->Delete();
  this->scalarBar->Delete();
}

MSQGeometryItem *MSQGeometryDifference::getGeometryItem()
{
  return new MSQGeometryDifferenceItem("Difference", this->distanceFilter->GetOutput(), this->differenceActor, true);
}

vtkScalarBarActor *MSQGeometryDifference::getScalarBar()
{
  return this->scalarBar;
}
