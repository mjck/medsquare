/*
 * MSQInspectorWidget.h
 *
 *  Created on: Oct 15, 2012
 *      Author: williammizuta
 */

#ifndef MSQ_DATA_MANAGER_H
#define MSQ_DATA_MANAGER_H

#include <QtGui>

class MedSquare;

class MSQGeometryWidget;
class MSQImageManagerWidget;
class MSQGeometryItem;

class MSQInspectorWidget: public QDockWidget
{
Q_OBJECT

public:
  MSQInspectorWidget(MedSquare *medSquare);
  virtual ~MSQInspectorWidget();

signals:
  void geometryInserted(MSQGeometryItem*);
  void geometryRemoved(MSQGeometryItem*);
  void geometryChanged(MSQGeometryItem*);

public slots:
  void setInput();
  void setEnabled(bool enabled);
  void loadGeometry();

private:
  QTabWidget *tabWidget;
  MSQImageManagerWidget *imageWidget;
  MSQGeometryWidget *geometryWidget;
};

#endif
