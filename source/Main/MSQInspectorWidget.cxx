/*
 * MSQInspectorWidget.cxx
 *
 *  Created on: Oct 15, 2012
 *      Author: williammizuta
 */

#include "MSQInspectorWidget.h"

#include "MedSquare.h"

#include "MSQGeometryWidget.h"
#include "MSQImageManagerWidget.h"

MSQInspectorWidget::MSQInspectorWidget(MedSquare *medSquare) : QDockWidget(tr("Inspector"), medSquare)
{
  this->tabWidget = new QTabWidget();
  this->tabWidget->setStyleSheet("QTabBar::tab { font: 11pt; } QTabWidget::pane { margin: 0px, 0px, 0px, 0px; border: 2px; } " );

  this->imageWidget = new MSQImageManagerWidget(medSquare);
  this->geometryWidget = new MSQGeometryWidget(medSquare);

  this->tabWidget->addTab(this->imageWidget, tr("Image"));
  this->tabWidget->addTab(this->geometryWidget, tr("Geometry"));

  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(this->tabWidget);
  layout->setContentsMargins(QMargins(0,10,0,0));

  // create placeholder widget
  QWidget *dockContents = new QWidget();

  // set layout
  dockContents->setLayout(layout);

  // set dock contents
  this->setWidget(dockContents);

  this->setInput();

  connect(this->geometryWidget, SIGNAL(itemInserted(MSQGeometryItem*)), this,  SIGNAL(geometryInserted(MSQGeometryItem*)));
  connect(this->geometryWidget, SIGNAL(itemRemoved(MSQGeometryItem*)), this,  SIGNAL(geometryRemoved(MSQGeometryItem*)));
  connect(this->geometryWidget, SIGNAL(itemChanged(MSQGeometryItem*)), this,  SIGNAL(geometryChanged(MSQGeometryItem*)));
}

MSQInspectorWidget::~MSQInspectorWidget()
{
}

void MSQInspectorWidget::setInput()
{
  this->imageWidget->setInput();
}

void MSQInspectorWidget::setEnabled(bool enabled)
{
  this->geometryWidget->setEnabled(enabled);
}

void MSQInspectorWidget::loadGeometry()
{
  this->geometryWidget->loadFile();
}
