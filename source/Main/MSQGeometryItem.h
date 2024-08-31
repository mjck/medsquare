/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryItem.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_GEOMETRYITEM_H
#define MSQ_GEOMETRYITEM_H

#include <QtGui>
#include <QComboBox>
#include <QTreeWidget>
#include <QCheckBox>
#include <QTreeWidgetItem>

class vtkActor;
class vtkPolyData;

class MSQGeometryItem : public QObject
{
Q_OBJECT

public:
  MSQGeometryItem();
  MSQGeometryItem(const QString& name, vtkPolyData *polydata, vtkActor *actor, bool visible = true);
  virtual ~MSQGeometryItem();

  virtual void createTreeItem(QTreeWidget *tree);

  vtkActor *getActor();
  vtkPolyData *getPolyData();
  QString getName();

  QColor getColor();
  void setColor(double R, double G, double B, double alpha = 1);
  void setColor(QColor *color);

  void show();
  void hide();

protected:
  QString name;
  vtkPolyData *polydata;

private:
  vtkActor *actor;
  double opacity;
  bool visible;

  double colorsR[5];
  double colorsG[5];
  double colorsB[5]; 

signals:
  void itemChanged(MSQGeometryItem *item);

private slots:
  void changeSelectedVisualizationType(int);
  void changeVisibility(bool);

};

#endif
