/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryWidget.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_GEOMETRY_WIDGET_H
#define MSQ_GEOMETRY_WIDGET_H

#include <QtGui>
#include <vtkOBJExporter.h>

class MedSquare;

class MSQGeometryItem;

class vtkObject;
class vtkPolyData;

class MSQGeometryWidget: public QWidget
{
Q_OBJECT

public:
  MSQGeometryWidget(MedSquare *medSquare);
  ~MSQGeometryWidget();

 void addGeometryItem(const QString& name, vtkPolyData *polydata, bool visible = true);
 void addGeometryItem(MSQGeometryItem *item);
 void removeGeometryItem(const QString& name);
 void saveGeometryItem(const QString& name);
 void setEnabled(bool value);

 QHash<QString, MSQGeometryItem*> getGeometryItems();
  
signals:
  void itemInserted(MSQGeometryItem *item);
  void itemRemoved(MSQGeometryItem *item);
  void itemChanged(MSQGeometryItem *item);

public slots:
  void loadFile();
  void widgetItemChanged(MSQGeometryItem*);

private slots:
  void removeSelectedGeometry();
  void saveSelectedGeometry();
  void changeSettingsSelectedGeometry(QTreeWidgetItem*, int);
  void setTreeItemActivated(QTreeWidgetItem *,int);

private:
  MedSquare *medSquare;

  QTreeWidget *geometryTree;
  QHash<QString, MSQGeometryItem*> geometryItems;
  QAction *addAction;
  QAction *removeAction;
  QAction *saveAction;
  QAction *configAction;
  
  QTreeWidgetItem *currentWidgetItem;

  void buildWidget();
  QString allowedNameGeometry(const QString &name);
  bool existGeometryName(const QString& name);

  MSQGeometryItem* getGeometryItem(const QString& name);
  QVector<QColor> predefinedColors;
};

#endif
