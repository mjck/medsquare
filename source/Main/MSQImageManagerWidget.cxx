/*=========================================================================

 Program:   MedSquare
 Module:    MSQImageInformationWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQImageManagerWidget.h"

#include "MSQColormapFactory.h"
#include "MSQImageItem.h"

#include "vtkmsqImageItem.h"
#include "vtkmsqLookupTable.h"
#include "vtkmsqMedicalImageProperties.h"

#include "vtkPointData.h"

/***********************************************************************************//**
 *
 */
MSQImageManagerWidget::MSQImageManagerWidget(MedSquare *medSquare) :
    QWidget(medSquare)
{
  this->medsquare = medSquare;

  // build interface elements
  this->buildWidget();
}

/***********************************************************************************//**
 *
 */
MSQImageManagerWidget::~MSQImageManagerWidget()
{
}

/***********************************************************************************//**
 *
 */
void MSQImageManagerWidget::updateWidget()
{
  QFont font;
  font.setPointSize(11);

  QFont boldFont;
  boldFont.setPointSize(11);
  boldFont.setBold(true);

  this->infoTree->clear();

  int imageOpenNum = this->medsquare->getImageOpenNum();

  if (imageOpenNum == 0)
  {
    QTreeWidgetItem *topItem = new QTreeWidgetItem(this->infoTree);
    topItem->setIcon(0, QIcon(":/images/user.png"));
    topItem->setFont(0, boldFont);
    topItem->setFont(1, boldFont);
    topItem->setText(0, "No file");
    return;
  }

  for (int i = 0; i < imageOpenNum; i++)
  {
    vtkmsqImageItem *imageItem = vtkmsqImageItem::New();
    imageItem->SetImage(this->medsquare->getImageDataAt(i));
    imageItem->SetProperties(this->medsquare->getImagePropertiesAt(i));
    imageItem->SetColormap(this->medsquare->getImageLookupTableAt(i));
    MSQImageItem *treeItem = new MSQImageItem(imageItem);
    treeItem->createTreeItem(this->infoTree);
  }

  this->infoTree->expandToDepth(1);
  for(int i = 0; i < this->infoTree->topLevelItemCount(); i++){
	  this->infoTree->topLevelItem(i)->setExpanded(false);
	  if (i == this->medsquare->getImageSelectedIdx())
	  {
	    this->infoTree->topLevelItem(i)->setExpanded(true);
	  }
  }
  this->medsquare->refresh();

}

/***********************************************************************************//**
 * Response to a double click in image item
 */
void MSQImageManagerWidget::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
  if ( item )
    {
      QTreeWidgetItem* itemParent = item;
      while(itemParent->parent() != NULL) {
	    itemParent = itemParent->parent();
      }
      this->medsquare->setCurrentImage(this->infoTree->indexOfTopLevelItem(itemParent));
      //apagar:
      //this->medsquare->getImagePropertiesAt(this->medsquare->getImageSelectedIdx())->Print(cout);
      //printf("Colormap = %s\n", this->medsquare->getImageLookupTableAt(this->medsquare->getImageSelectedIdx())->GetName().c_str());

    }
}

/***********************************************************************************//**
 * Build widget components
 */
void MSQImageManagerWidget::buildWidget()
{
  // set default font size
  QFont font;
  font.setPointSize(11);

  // create information table
  this->infoTree = new QTreeWidget;
  this->infoTree->setExpandsOnDoubleClick(false);

  connect(this->infoTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));

  //this->infoTree->setColumnCount(1);

  QTreeWidgetItem* headerItem = new QTreeWidgetItem();
  headerItem->setText(0, QString("Property"));
  headerItem->setText(1, QString("Value"));
  headerItem->setText(2, QString(" "));
  this->infoTree->setHeaderItem(headerItem);
  this->infoTree->header()->setMovable(false);

  this->infoTree->setAlternatingRowColors(true);
  this->infoTree->setTextElideMode(Qt::ElideMiddle);
  this->infoTree->setWordWrap(true);
  this->infoTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->infoTree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  //this->infoTree->header()->setResizeMode(QHeaderView::ResizeToContents);
  this->infoTree->setColumnWidth(0, 150);
  this->infoTree->setColumnWidth(1, 150);

  //add buttons
  QAction *addAction;
  QAction *removeAction;
  QAction *configAction;

  addAction = new QAction(QIcon(":/images/plus.png"), tr("&Open..."), this);
  addAction->setStatusTip(tr("Add image"));
  addAction->setEnabled(true);
  connect(addAction, SIGNAL(triggered()), this->medsquare, SLOT(fileOpen()));

  removeAction = new QAction(QIcon(":/images/minus.png"), tr("&Remove"), this);
  removeAction->setStatusTip(tr("Remove image"));
  removeAction->setEnabled(true);
  connect(removeAction, SIGNAL(triggered()), this->medsquare, SLOT(fileCloseSelected()));

  configAction = new QAction(QIcon(":/images/spanner.png"), tr("&Tools"), this);
  configAction->setStatusTip(tr("Image tools"));
  configAction->setEnabled(false);

  QToolBar *toolBar = new QToolBar;
  toolBar->setIconSize(QSize(12,12));
  toolBar->addAction(addAction);
  toolBar->addSeparator();
  toolBar->addAction(removeAction);
  toolBar->addSeparator();
  toolBar->addAction(configAction);

  // set layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(2);

  layout->addWidget(this->infoTree);
  layout->addWidget(toolBar);

  this->setLayout(layout);

  //update widget
  this->updateWidget();

  // build context menu
  this->infoTree->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this->infoTree, SIGNAL(customContextMenuRequested(const QPoint&)),
      this, SLOT(showContextMenu(const QPoint&)));
}

/***********************************************************************************//**
 * This function is called when an image is loaded
 */
void MSQImageManagerWidget::setInput()
{
  // emit signals upon new image
  this->updateWidget();
}

/***********************************************************************************//**
 *
 *
 * \author: Daniel Oliveira Dantas
 */
void MSQImageManagerWidget::showContextMenu(const QPoint& pos)
{
    // for most widgets
    QPoint globalPos = this->infoTree->mapToGlobal(pos);
    int imageIndex = this->findImageIndex(pos);

    QMenu contextMenu;

    QAction* displayFile = contextMenu.addAction(tr("Display"));

    vtkmsqLookupTable *colormap = NULL;
    MSQColormapFactory *msqColormap = new MSQColormapFactory();

    QMenu *colormapMenu = new QMenu(tr("Colormaps"));
    contextMenu.addMenu(colormapMenu);

    contextMenu.addSeparator();

    QAction* closeFile = contextMenu.addAction(tr("Close"));

    QAction *grayColormap = new QAction(tr("Grayscale"), this);
    colormapMenu->addAction(grayColormap);
    QAction *hueColormap = new QAction(tr("Hue"), this);
    colormapMenu->addAction(hueColormap);
    QAction *satColormap = new QAction(tr("Saturation"), this);
    colormapMenu->addAction(satColormap);
    QAction *hotColormap = new QAction(tr("Hot"), this);
    colormapMenu->addAction(hotColormap);

    colormapMenu->addSeparator();

    QAction *customColormap = new QAction(tr("Open..."), this);
    colormapMenu->addAction(customColormap);

    QAction* selectedItem = contextMenu.exec(globalPos);
    if (selectedItem)
    {
      if (selectedItem == closeFile)
      {
        this->medsquare->fileClose(imageIndex);
      }
      else if (selectedItem == displayFile)
      {
        this->medsquare->setCurrentImage(imageIndex);
      }
      else if (selectedItem == grayColormap)
      {
        colormap = msqColormap->createColormap(MSQ_LUT_GRAY, 256);
    	this->medsquare->setImageLookupTableAt(imageIndex, colormap);
        this->updateWidget();
      }
      else if (selectedItem == hueColormap)
      {
        colormap = msqColormap->createColormap(MSQ_LUT_HUE, 256);
    	this->medsquare->setImageLookupTableAt(imageIndex, colormap);
        this->updateWidget();
      }
      else if (selectedItem == satColormap)
      {
        colormap = msqColormap->createColormap(MSQ_LUT_SATURATION, 256);
    	this->medsquare->setImageLookupTableAt(imageIndex, colormap);
        this->updateWidget();
      }
      else if (selectedItem == hotColormap)
      {
        colormap = msqColormap->createColormap(MSQ_LUT_HOT, 256);
    	this->medsquare->setImageLookupTableAt(imageIndex, colormap);
        this->updateWidget();
      }
      else if (selectedItem == customColormap)
      {
        QString currentFileName = "./../colormap";
        QString colormapName = QFileDialog::getOpenFileName(this->medsquare,
            tr("Open colormaps"), currentFileName);
        if (!colormapName.isEmpty())
        {
          colormap = msqColormap->loadColormap(colormapName.toStdString());
          if (colormap == NULL)
            this->medsquare->warningMessage(tr("Error reading file %1.").arg(colormapName),
                tr("Make sure file is of correct type and retry."));
          else
        	this->medsquare->setImageLookupTableAt(imageIndex, colormap);
          this->updateWidget();
        }
      }
    }

}

/***********************************************************************************//**
 * Find index of image (MedSquare->imageArray[]) relative to clicked area of the
 * information tree. Receives as parameter the position of the click, as
 *
 * The next line of code shows how to print item contents, very useful
 * for debugging and very hard to find documented.
 *
 * printf("%s\n",item->text(0).toAscii().data());
 *
 * \author Daniel Oliveira Dantas
 */
int MSQImageManagerWidget::findImageIndex(const QPoint& pos)
{
    QTreeWidgetItem * item = 0 ;
    item = this->infoTree->itemAt(pos) ;
    if ( item )
    {
      QTreeWidgetItem* itemParent = item;
      while(itemParent->parent() != NULL){
    	  itemParent = itemParent->parent();
      }
      return this->infoTree->indexOfTopLevelItem(itemParent);
    }
    return -1;
}
