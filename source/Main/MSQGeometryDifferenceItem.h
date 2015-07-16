/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryDifferenceItem.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_GEOMETRY_DIFFERENCE_ITEM_H
#define MSQ_GEOMETRY_DIFFERENCE_ITEM_H

#include "MSQGeometryItem.h"

class MSQGeometryDifferenceItem : public MSQGeometryItem
{
Q_OBJECT

public:
  MSQGeometryDifferenceItem(const QString& name, vtkPolyData *polydata, vtkActor *actor, bool visible = true);
  virtual ~MSQGeometryDifferenceItem();

  virtual void createTreeItem(QTreeWidget *tree);
};

#endif
