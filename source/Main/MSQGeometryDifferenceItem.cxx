/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryDifferenceItem.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQGeometryDifferenceItem.h"

#include "vtkPolyData.h"

MSQGeometryDifferenceItem::MSQGeometryDifferenceItem(const QString& name, vtkPolyData *polydata, vtkActor *actor, bool visible) :
    MSQGeometryItem(name, polydata, actor, visible)
{
}

MSQGeometryDifferenceItem::~MSQGeometryDifferenceItem()
{
}

void MSQGeometryDifferenceItem::createTreeItem(QTreeWidget *tree)
{
  QTreeWidgetItem *treeItem = new QTreeWidgetItem(tree);
  treeItem->setText(0, this->name);

  QFont font;
  font.setPointSize(11);

  QFont boldFont;
  boldFont.setPointSize(11);
  boldFont.setBold(true);

  treeItem->setFont(0, boldFont);
  treeItem->setFont(1, font);

  QTreeWidgetItem *itemPontos = new QTreeWidgetItem(treeItem);
  itemPontos->setText(0, "Number of points");
  itemPontos->setText(1, QString("%L1").arg(this->polydata->GetNumberOfPoints()));
  itemPontos->setFont(0, font);
  itemPontos->setFont(1, font);
  treeItem->addChild(itemPontos);

  QTreeWidgetItem *itemFaces = new QTreeWidgetItem(treeItem);
  itemFaces->setText(0, "Number of faces");
  itemFaces->setText(1, QString("%L1").arg(this->polydata->GetNumberOfPolys()));
  itemFaces->setFont(0, font);
  itemFaces->setFont(1, font);
  treeItem->addChild(itemFaces);
}
