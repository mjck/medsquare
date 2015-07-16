/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryDifference.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_GEOMETRY_DIFFERENCE_H
#define MSQ_GEOMETRY_DIFFERENCE_H

#include <QtGui>

class MedSquare;

class MSQGeometryItem;

class vtkActor;
class vtkmsqDistancePolyDataFilter;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkScalarBarActor;

class MSQGeometryDifference : public QObject
{
Q_OBJECT
public:
  MSQGeometryDifference(MedSquare *medSquare, QHash<QString, MSQGeometryItem*> geometries);
  virtual ~MSQGeometryDifference();

  MSQGeometryItem *getGeometryItem();
  vtkScalarBarActor *getScalarBar();

private:
  vtkActor *differenceActor;
  vtkmsqDistancePolyDataFilter *distanceFilter;
  vtkPolyDataMapper *mapper;
  vtkScalarBarActor *scalarBar;
};

#endif
