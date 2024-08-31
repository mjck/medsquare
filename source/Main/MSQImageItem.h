/*
 * MSQImageItem.h
 *
 *  Created on: Oct 15, 2012
 *      Author: williammizuta
 */

#ifndef MSQ_IMAGE_ITEM_H
#define MSQ_IMAGE_ITEM_H

#include <QtGui>
#include <QFont>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class vtkmsqImageItem;

class MSQImageItem : public QObject
{
Q_OBJECT

public:
  MSQImageItem(vtkmsqImageItem *imageItem);
  virtual ~MSQImageItem();

  virtual void createTreeItem(QTreeWidget *tree);

private:
  vtkmsqImageItem *imageItem;
  QFont font, boldFont;
};

#endif
