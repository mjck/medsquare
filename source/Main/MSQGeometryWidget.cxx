/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQGeometryWidget.h"

#include <sstream>

#include "MedSquare.h"
#include "MSQGeometryItem.h"

#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkProperty.h"
#include "vtkmsqOBJWriter.h"

/***********************************************************************************//**
 * 
 */
MSQGeometryWidget::MSQGeometryWidget(MedSquare *medSquare) :
    QWidget(medSquare), medSquare(medSquare)
{
  // build interface elements
  this->buildWidget();

  this->predefinedColors.push_back(QColor(Qt::red));
  this->predefinedColors.push_back(QColor(Qt::green));
  this->predefinedColors.push_back(QColor(Qt::blue));
  this->predefinedColors.push_back(QColor(Qt::cyan));
  this->predefinedColors.push_back(QColor(Qt::magenta));
  this->predefinedColors.push_back(QColor(Qt::yellow));
  this->predefinedColors.push_back(QColor(Qt::gray));
}

/***********************************************************************************//**
 * 
 */
MSQGeometryWidget::~MSQGeometryWidget()
{

}

/***********************************************************************************//**
 * Build widget components
 */
void MSQGeometryWidget::buildWidget()
{
  // set default font size
  QFont font;
  font.setPointSize(11);

  QFont boldFont;
  boldFont.setPointSize(11);
  boldFont.setBold(true);  

  // create geometries tree
  this->geometryTree = new QTreeWidget;
  this->geometryTree->setAlternatingRowColors(true);

  QTreeWidgetItem* headerItem = new QTreeWidgetItem();
  headerItem->setText(0, QString("Property"));
  headerItem->setText(1, QString("Value"));
  this->geometryTree->setHeaderItem(headerItem);

  this->geometryTree->header()->setHorizontalScrollMode(
      QAbstractItemView::ScrollPerPixel);
  this->geometryTree->header()->setResizeMode(QHeaderView::ResizeToContents);
 
  //add buttons
  addAction = new QAction(QIcon(":/images/plus.png"), tr("&Add..."), this);
  addAction->setStatusTip(tr("Add geometry"));
  addAction->setEnabled(false);
  connect(addAction, SIGNAL(triggered()), this, SLOT(loadFile()));

  removeAction = new QAction(QIcon(":/images/minus.png"), tr("&Remove"), this);
  removeAction->setStatusTip(tr("Remove geometry"));
  removeAction->setEnabled(false);
  connect(removeAction, SIGNAL(triggered()), this, SLOT(removeSelectedGeometry()));

  saveAction = new QAction(QIcon(":/images/savegray.png"), tr("&Save"), this);
  saveAction->setStatusTip(tr("Save geometry"));
  saveAction->setEnabled(false);
  connect(saveAction, SIGNAL(triggered()), this, SLOT(saveSelectedGeometry()));


  configAction = new QAction(QIcon(":/images/spanner.png"), tr("&Tools"), this);
  configAction->setStatusTip(tr("Geometry tools"));
  configAction->setEnabled(false);


  connect(geometryTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this,
      SLOT(changeSettingsSelectedGeometry(QTreeWidgetItem *, int)));

  connect(geometryTree, SIGNAL(itemClicked(QTreeWidgetItem *,int)), this, SLOT(setTreeItemActivated(QTreeWidgetItem *,int)));


  QToolBar *toolBar = new QToolBar;
  toolBar->setIconSize(QSize(12,12));
  toolBar->addAction(addAction);
  toolBar->addSeparator();
  toolBar->addAction(removeAction);
  toolBar->addSeparator();
  toolBar->addAction(saveAction);
  toolBar->addSeparator();
  toolBar->addAction(configAction);

  // set layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(5);

  layout->addWidget(geometryTree);
  layout->addWidget(toolBar);

  this->setLayout(layout);
}

/***********************************************************************************//**
 * This function is called when an image is loaded
 */
void MSQGeometryWidget::addGeometryItem(const QString& name, vtkPolyData *polydata,
    bool visible)
{
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(polydata);

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  mapper->Delete();

  MSQGeometryItem *item = new MSQGeometryItem(name, polydata, actor, visible);
  this->addGeometryItem(item);
  connect(item, SIGNAL(itemChanged(MSQGeometryItem*)), this,  SLOT(widgetItemChanged(MSQGeometryItem*)));
}

/***********************************************************************************//**
 *
 */
void MSQGeometryWidget::addGeometryItem(MSQGeometryItem *item)
{

  //add a random color
  double ranMax = RAND_MAX;
  double opacity = 1;

  //if exists more geometries than predefined colors use a random color
  QColor *coloritem = new QColor();
  if (geometryItems.size() >= predefinedColors.size())
  {
    double R, G, B;
    R = rand() / ranMax;
    G = rand() / ranMax;
    B = rand() / ranMax;
    coloritem->setRgbF(R, G, B, opacity);
    item->setColor(R, G, B, opacity);
  }
  else
  {
    coloritem = &predefinedColors[geometryItems.size()];
    item->setColor(coloritem);
  }
  geometryItems.insert(item->getName(), item);
  item->createTreeItem(this->geometryTree);

  emit itemInserted(item);
}

/***********************************************************************************//**
 *
 */
void MSQGeometryWidget::setTreeItemActivated(QTreeWidgetItem *item ,int column)
{
  std::cout<<"itemActivated"<<endl;
  std::cout<<item->text(0).toLocal8Bit().constData();
  currentWidgetItem=item;
}

/***********************************************************************************//**
 *
 */
void MSQGeometryWidget::setEnabled(bool value)
{
  this->removeAction->setEnabled(value);
  this->addAction->setEnabled(value);
  this->saveAction->setEnabled(true);
}

/***********************************************************************************//**
 * 
 */
void MSQGeometryWidget::removeGeometryItem(const QString& name)
{
  MSQGeometryItem *item = getGeometryItem(name);
  emit itemRemoved(item);
  geometryItems.remove(name);
}

/***********************************************************************************//**
 *
 */
void MSQGeometryWidget::saveGeometryItem(const QString& name)
{
  MSQGeometryItem *item = getGeometryItem(name);  

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Geometry"),
      name, tr("Wavefront (*.obj)"));

  // In case a file was chosen try saving it
  if (!fileName.isEmpty())
  {
    vtkmsqOBJWriter* objWriter = vtkmsqOBJWriter::New();
    objWriter->SetInput(item->getPolyData());
    objWriter->SetFileName(fileName.toUtf8().constData());
    objWriter->Update();
  }
}

/***********************************************************************************//**
 *
 */
QHash<QString, MSQGeometryItem*> MSQGeometryWidget::getGeometryItems()
{
  return this->geometryItems;
}

MSQGeometryItem *MSQGeometryWidget::getGeometryItem(const QString& name)
{
  return geometryItems.find(name).value();
}

/***********************************************************************************//**
 * 
 */
bool MSQGeometryWidget::existGeometryName(const QString& name)
{
  return geometryItems.contains(name);
}

/***********************************************************************************//**
 * 
 */
QString MSQGeometryWidget::allowedNameGeometry(const QString &name)
{
  int n = 0;
  QString tmpName = name;
  while (existGeometryName(tmpName))
  {
    n++;
    std::stringstream ss;
    ss << n;

    tmpName = name + " (" + QString(ss.str().c_str()) + ")" ;
  }
  return tmpName;
}

/***********************************************************************************//**
 *
 */
void MSQGeometryWidget::widgetItemChanged(MSQGeometryItem * item)
{
  emit itemChanged(item);
}

/***********************************************************************************//**
 *
 */
void MSQGeometryWidget::loadFile()
{
  QString fileName = QFileDialog::getOpenFileName(this->medSquare, tr("Open geometry"),
      NULL, tr("VTK files (*.vtk)"));
  if (!fileName.isEmpty())
  {
    // show status message
    medSquare->updateStatusBar(tr("Reading geometry..."), true);

    QFileInfo finfo(fileName);

    QString geometryName = allowedNameGeometry(finfo.fileName());
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();

    // instantiate connection between vtk and qt events
    vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
        vtkEventQtSlotConnect>::New();

    // connect progress events to qt progress bar updates
    
    connection->Connect(reader, vtkCommand::ProgressEvent, this->medSquare, SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));
    reader->SetFileName(fileName.toLocal8Bit().constData());
    reader->Update();

    this->addGeometryItem(geometryName.toLocal8Bit().constData(), reader->GetOutput(),
        true);

    // ready for more
    medSquare->updateStatusBar(tr("Ready"), false);
  }
}

/***********************************************************************************//**
 * 
 */
void MSQGeometryWidget::removeSelectedGeometry()
{
  if (geometryItems.size() > 0 && geometryTree->currentItem() != NULL)
  {
    if (geometryTree->currentItem()->parent()) 
    {
      this->removeGeometryItem(geometryTree->currentItem()->parent()->text(0));
      delete geometryTree->currentItem()->parent();
    }
    else 
    {
      this->removeGeometryItem(geometryTree->currentItem()->text(0));
      delete geometryTree->currentItem();
    }
  }
}

/*************************************************************************************//**
*
*/

void MSQGeometryWidget::saveSelectedGeometry()
{
  if (geometryItems.size() > 0 && geometryTree->currentItem() != NULL)
  {
    if (geometryTree->currentItem()->parent()) 
    {      
      this->saveGeometryItem(geometryTree->currentItem()->parent()->text(0));            
    }
    else 
    {
      this->saveGeometryItem(geometryTree->currentItem()->text(0));            
    }
  }else
  {
      QMessageBox::information(this,"Non geometry selected", "There is not a geometry selected, choose one and try again.");
  }
}

/***********************************************************************************//**
 *
 */
void MSQGeometryWidget::changeSettingsSelectedGeometry(QTreeWidgetItem *item, int column)
{
  if (item->parent())
  {
    // for changing colors
	if (item->text(0) == "Color")
    {
      MSQGeometryItem *itemG = getGeometryItem(item->parent()->text(0));

      QColor color = QColorDialog::getColor(itemG->getColor(), this, "Select a color",
                     QColorDialog::ShowAlphaChannel);
      if (color.isValid())
      {
        qreal R, G, B, a;
        color.getRgbF(&R, &G, &B, &a);
        itemG->setColor((double) R, (double) G, (double) B, (double) a);
	      
        QPixmap pixmap(20, 20);
        pixmap.fill(color);
	      
        item->parent()->setIcon(0, QIcon(pixmap));
        item->setIcon(1, QIcon(pixmap));
      }
    }
  }
}

