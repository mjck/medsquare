/*
 * MSQImageItem.cxx
 *
 *  Created on: Oct 15, 2012
 *      Author: williammizuta
 */

#include "MSQImageItem.h"

#include "vtkmsqImageItem.h"
#include "vtkmsqLookupTable.h"
#include "vtkmsqMedicalImageProperties.h"

#include "vtkImageData.h"
#include "vtkPointData.h"

MSQImageItem::MSQImageItem(vtkmsqImageItem *imageItem) : imageItem(imageItem)
{
  this->font.setPointSize(11);

  this->boldFont.setPointSize(11);
  this->boldFont.setBold(true);
}

MSQImageItem::~MSQImageItem()
{
}

void MSQImageItem::createTreeItem(QTreeWidget *tree)
{
  int dimensions[3];

  QTreeWidgetItem *topItem = new QTreeWidgetItem(tree);
  topItem->setIcon(0, QIcon(":/images/user.png"));
  topItem->setFont(0, boldFont);
  topItem->setFont(1, boldFont);
  topItem->setText(0, "No file");

  vtkImageData* image = imageItem->GetImage();
  vtkmsqMedicalImageProperties* properties = imageItem->GetProperties();
  vtkmsqLookupTable* colormap = imageItem->GetColormap();

  if (properties->GetNumberOfUserDefinedValues() == 0 || !properties->GetUserDefinedValue("Filename"))
    return;

  QFileInfo *file = new QFileInfo(properties->GetUserDefinedValue("Filename"));
  topItem->setText(0, file->fileName());
  tree->setFirstItemColumnSpanned(topItem, true);

  QTreeWidgetItem *pathItem = new QTreeWidgetItem(topItem);
  pathItem->setText(0, "Path");
  pathItem->setFont(0, boldFont);
  pathItem->setText(1, file->absolutePath());
  pathItem->setFont(1, font);

  if (image)
  {
    double origin[3];
    image->GetOrigin(origin);
    QTreeWidgetItem *origItem = new QTreeWidgetItem(topItem);
    origItem->setText(0, "Origin");
    origItem->setFont(0, boldFont);
    origItem->setText(1, QString("(%1, %2, %3)").arg(origin[0]).arg(origin[1]).arg(origin[2]));
    origItem->setFont(1, font);

    image->GetDimensions(dimensions);
    QTreeWidgetItem *dimsItem = new QTreeWidgetItem(topItem);
    dimsItem->setText(0, "Dimensions");
    dimsItem->setFont(0, boldFont);
    dimsItem->setText(1, QString("%1 x %2 x %3").arg(dimensions[0]).arg(dimensions[1]).arg(dimensions[2]));
    dimsItem->setFont(1, font);

    QTreeWidgetItem *compItem = new QTreeWidgetItem(topItem);
    compItem->setText(0, "Components");
    compItem->setFont(0, boldFont);
    compItem->setText(1, QString("%1").arg(image->GetNumberOfScalarComponents()));
    compItem->setFont(1, font);

    double spacing[3];
    image->GetSpacing(spacing);
    QTreeWidgetItem *spacingItem = new QTreeWidgetItem(topItem);
    spacingItem->setText(0, "Spacing");
    spacingItem->setFont(0, boldFont);
    spacingItem->setText(1, QString("%1 x %2 x %3 mm").arg(spacing[0]).arg(spacing[1]).arg(spacing[2]));
    spacingItem->setFont(1, font);

    QTreeWidgetItem *dataTypeItem = new QTreeWidgetItem(topItem);
    dataTypeItem->setText(0, "Data type");
    dataTypeItem->setFont(0, boldFont);
    dataTypeItem->setText(1, QString("%1 (%2 bytes)").arg(image->GetScalarTypeAsString()).arg(image->GetScalarSize()));
    dataTypeItem->setFont(1, font);

    double range[3];
    image->GetPointData()->GetScalars()->GetRange(range);
    QTreeWidgetItem *rangeItem = new QTreeWidgetItem(topItem);
    rangeItem->setText(0, "Range");
    rangeItem->setFont(0, boldFont);
    rangeItem->setText(1, QString("[%1, %2]").arg(range[0]).arg(range[1]));
    rangeItem->setFont(1, font);
  }

  int orient = properties->GetOrientationType();
  QTreeWidgetItem *orientItem = new QTreeWidgetItem(topItem);
  orientItem->setText(0, "Orientation");
  orientItem->setFont(0, boldFont);
  orientItem->setText(1, QString(properties->GetStringFromOrientationType(orient)));
  orientItem->setFont(1, font);

  QTreeWidgetItem *cosineItem = new QTreeWidgetItem(orientItem);
  cosineItem->setText(0, "Cosines");
  cosineItem->setFont(0, boldFont);

  double cosine[6];
  properties->GetDirectionCosine(cosine);
  cosineItem->setText(1, QString("(%1, %2, %3) (%4, %5, %6)").arg(cosine[0]).arg(cosine[1]).arg(cosine[2]).arg(cosine[3]).arg(cosine[4]).arg(cosine[5]));
  cosineItem->setFont(1, font);

  QTreeWidgetItem *sizeItem = new QTreeWidgetItem(topItem);
  sizeItem->setText(0, "Image size");
  sizeItem->setFont(0, boldFont);
  sizeItem->setText(1, QString("%L1 bytes").arg((long) dimensions[0] * dimensions[1] * dimensions[2] * image->GetNumberOfScalarComponents() * image->GetScalarSize()));
  sizeItem->setFont(1, font);

  if (colormap)
  {
    QTreeWidgetItem *colormapItem = new QTreeWidgetItem(topItem);
    colormapItem->setText(0, "Colormap");
    colormapItem->setFont(0, boldFont);
    colormapItem->setText(1, QString(colormap->GetName().c_str()));
    colormapItem->setFont(1, font);
  }

  if (properties->GetNumberOfGradientValues() > 0)
    {
      QTreeWidgetItem *diffusionItem = new QTreeWidgetItem(topItem);
      diffusionItem->setText(0, "Diffusion");
      diffusionItem->setFont(0, boldFont);

      QTreeWidgetItem *diffbvalsItem = new QTreeWidgetItem(diffusionItem);
      diffbvalsItem->setText(0, "B-values");
      diffbvalsItem->setFont(0, boldFont);
      diffbvalsItem->setText(1, QString("%1").arg(properties->GetNumberOfBValues()));
      diffbvalsItem->setFont(1, font);

      int num_grad = properties->GetNumberOfGradientValues();
      QTreeWidgetItem *diffgradsItem = new QTreeWidgetItem(diffusionItem);
      diffgradsItem->setText(0, "Gradients");
      diffgradsItem->setFont(0, boldFont);
      diffgradsItem->setText(1, QString("%1 directions").arg(num_grad));
      diffgradsItem->setFont(1, font);

      for(int i=0;i<num_grad;i++)
	{
	  double vals[4];
	  QTreeWidgetItem *gradItem = new QTreeWidgetItem(diffgradsItem);
	  properties->GetNthDiffusionGradient(i, vals);
	  gradItem->setText(1, QString("%1: (%2, %3, %4)").arg(i).arg(vals[0], 0, 'g', 6).arg(vals[1], 0, 'g', 6).arg(vals[2], 0, 'g', 6));
	  gradItem->setFont(1, font);
	  gradItem->setText(2, QString("%1 s/mm^2").arg(vals[3]));
	  gradItem->setFont(2, font);
	}
    }
}
