/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryItem.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQGeometryItem.h"

#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"

/***********************************************************************************//**
 *
 */
MSQGeometryItem::MSQGeometryItem()
{
  this->name = "Polydata";
  this->polydata = NULL;
  this->actor = NULL;
  this->visible = false;
}


/***********************************************************************************//**
 *
 */
MSQGeometryItem::MSQGeometryItem(const QString& name, vtkPolyData *polydata, vtkActor *actor, bool visible)
{
  this->name = name;
  this->polydata = polydata;
  this->actor = actor;
  if(visible)
    this->opacity = 1.0;
  else
    this->opacity=0;
}


/***********************************************************************************//**
 *
 */
MSQGeometryItem::~MSQGeometryItem()
{
  this->polydata->Delete();
  this->actor->Delete();
}

/***********************************************************************************//**
 *
 */
QString MSQGeometryItem::getName()
{
  return this->name;
}

/***********************************************************************************//**
 *
 */
vtkPolyData* MSQGeometryItem::getPolyData()
{
  return this->polydata;
}

/***********************************************************************************//**
 *
 */
vtkActor* MSQGeometryItem::getActor()
{
  return this->actor;
}

/***********************************************************************************//**
 *
 */
void MSQGeometryItem::show()
{
  this->visible = true;
}

/***********************************************************************************//**
 *
 */
void MSQGeometryItem::hide()
{
  this->visible = false;
}

/***********************************************************************************//**
 *
 */
void MSQGeometryItem::createTreeItem(QTreeWidget *tree)
{
  QPixmap *pixmap = new QPixmap(20, 20);
  pixmap->fill(this->getColor());

  QTreeWidgetItem *treeItem = new QTreeWidgetItem(tree);
  treeItem->setText(0, this->name);
  treeItem->setIcon(0, QIcon(*pixmap));

  // set default font size
  QFont font;
  font.setPointSize(11);

  QFont boldFont;
  boldFont.setPointSize(11);
  boldFont.setBold(true);

  treeItem->setFont(0, boldFont);
  treeItem->setFont(1, font);

  QTreeWidgetItem *itemPontos = new QTreeWidgetItem(treeItem);
  itemPontos->setFont(0, boldFont);
  itemPontos->setText(0, "Number of points");
  itemPontos->setFont(1, font);
  itemPontos->setText(1, QString("%L1").arg(this->polydata->GetNumberOfPoints()));
  treeItem->addChild(itemPontos);

  QTreeWidgetItem *itemFaces = new QTreeWidgetItem(treeItem);
  itemFaces->setFont(0, boldFont);
  itemFaces->setText(0, "Number of faces");
  itemFaces->setFont(1, font);
  itemFaces->setText(1, QString("%L1").arg(this->polydata->GetNumberOfPolys()));

  treeItem->addChild(itemFaces);

  QTreeWidgetItem *itemColor = new QTreeWidgetItem(treeItem);
  itemColor->setFont(0, boldFont);
  itemColor->setText(0, "Color");
  itemColor->setFont(1, font);
  itemColor->setIcon(1, QIcon(*pixmap));
  treeItem->addChild(itemColor);

  //create item of visualization with a combobox
  QTreeWidgetItem *itemVisualization = new QTreeWidgetItem(treeItem);
  itemVisualization->setFont(0, boldFont);
  itemVisualization->setText(0, "Type");
  itemVisualization->setFont(1, font);
  treeItem->addChild(itemVisualization);

  QComboBox *comboVisualization=new QComboBox(tree);
  comboVisualization->addItem("Surfaces");
  comboVisualization->addItem("Wireframe");
  comboVisualization->addItem("Points");
  comboVisualization->setEditable(false);
  comboVisualization->setFont(font);
  comboVisualization->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  tree->setItemWidget(itemVisualization,1,comboVisualization);

  connect(comboVisualization, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSelectedVisualizationType(int)));

  QTreeWidgetItem *itemVisible = new QTreeWidgetItem(treeItem);
  itemVisible->setFont(0, boldFont);
  itemVisible->setText(0, "Visible");
  itemVisible->setFont(1, font);
  treeItem->addChild(itemVisible);

  QCheckBox *checkboxVisible=new QCheckBox(tree);
  checkboxVisible->setCheckState(Qt::Checked);
  tree->setItemWidget(itemVisible,1,checkboxVisible);

  connect(checkboxVisible, SIGNAL(toggled(bool)), this, SLOT(changeVisibility(bool)));
}

/***********************************************************************************//**
 *
 */
void MSQGeometryItem::changeVisibility(bool checked)
{
  if (checked)
    actor->GetProperty()->SetOpacity(this->opacity);
  else
    actor->GetProperty()->SetOpacity(0);
  emit itemChanged(this);
}

/***********************************************************************************//**
 *
 */
void MSQGeometryItem::changeSelectedVisualizationType(int type)
{
  switch(type)
    {
    case 0:
      actor->GetProperty()->SetRepresentationToSurface();
      break;
    case 1:
      actor->GetProperty()->SetRepresentationToWireframe();
      break;
    case 2:
      actor->GetProperty()->SetRepresentationToPoints();
      break;
    }
  emit itemChanged(this);
}


/***********************************************************************************//**
 *
 */
void MSQGeometryItem::setColor(double R, double G, double B, double alpha)
{
  this->actor->GetProperty()->SetColor(R, G, B);
  this->actor->GetProperty()->SetOpacity(alpha);
  this->opacity=alpha;
}

/***********************************************************************************//**
 *
 */
void MSQGeometryItem::setColor(QColor* color)
{
  qreal R, G, B, a;
  color->getRgbF(&R, &G, &B, &a);
  setColor((double) R, (double) G, (double) B, (double) a);
}

/***********************************************************************************//**
 *
 */
QColor MSQGeometryItem::getColor()
{
  double rgb[3];
  double opacity;
  this->actor->GetProperty()->GetColor(rgb);
  opacity = this->actor->GetProperty()->GetOpacity();
  return QColor::fromRgbF((qreal)rgb[0], (qreal)rgb[1], (qreal)rgb[2], (qreal)opacity);
}
