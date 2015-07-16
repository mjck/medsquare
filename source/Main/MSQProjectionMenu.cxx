/*===================a======================================================

 Program:   MedSquarelse
 Module:    MSQProjectionMenu.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQProjectionMenu.h"

#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkImageData.h"
#include "vtkImageShiftScale.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkColorTransferFunction.h"
#include "vtkTransform.h"
#include "vtkCallbackCommand.h"

#include "MSQColormapFactory.h"
#include "vtkmsqImageItem.h"
#include "vtkmsqMedicalImageProperties.h"

void updateColormapCallback(vtkObject *caller, unsigned long eid, void *clientData, void *callData)
{
  static_cast<MSQProjectionMenu *>(clientData)->changedColormap();
}

/***********************************************************************************//**
 *
 */
MSQProjectionMenu::MSQProjectionMenu(QWidget *parent) : QMenu(tr("Projection"), parent)
{
  this->image = NULL;
  this->updateColormap = NULL;

  QAction *noneProjection = new QAction(tr("None"), this);
  QAction *maximumProjection = new QAction(tr("Maximum intensity"), this);
  QAction *minimumProjection = new QAction(tr("Minimum intensity"), this);
  QAction *compositeProjection = new QAction(tr("Mean intensity"), this);

  noneProjection->setCheckable(true);
  maximumProjection->setCheckable(true);
  minimumProjection->setCheckable(true);
  compositeProjection->setCheckable(true);

  this->addAction(noneProjection);
  this->addSeparator();
  this->addAction(maximumProjection);
  this->addAction(minimumProjection);
  this->addAction(compositeProjection);

  this->projections = new QActionGroup(parent);
  this->projections->addAction(noneProjection);
  this->projections->addAction(maximumProjection);
  this->projections->addAction(minimumProjection);
  this->projections->addAction(compositeProjection);

  this->projectionType = -1; //TODO: change

  this->colormapFactory = new MSQColormapFactory();

  projections->setExclusive(true);
  projections->setEnabled(false);

  noneProjection->setChecked(true);


  connect(noneProjection, SIGNAL(triggered()), this, SLOT(removeProjections()));
  connect(maximumProjection, SIGNAL(triggered()), this, SLOT(maximumProjectionAction()));
  connect(minimumProjection, SIGNAL(triggered()), this, SLOT(minimumProjectionAction()));
  connect(compositeProjection, SIGNAL(triggered()), this, SLOT(compositeProjectionAction()));
}

/***********************************************************************************//**
 *
 */
MSQProjectionMenu::~MSQProjectionMenu()
{
  if (this->image)
  {
    this->image->Delete();
  }
}

/***********************************************************************************//**
 *
 */
void MSQProjectionMenu::removeProjections()
{
  this->projectionType=-1;//TODO: this should be refactored
  emit projectionChanged(NULL);

}

/***********************************************************************************//**
 *
 */
void MSQProjectionMenu::maximumProjectionAction()
{
  this->projectionType=vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND;


  emit projectionChanged(getProjection());
}

/***********************************************************************************//**
 *
 */
void MSQProjectionMenu::minimumProjectionAction()
{
  this->projectionType=vtkVolumeMapper::MINIMUM_INTENSITY_BLEND;
  emit projectionChanged(getProjection());
}

/***********************************************************************************//**
 *
 */
void MSQProjectionMenu::compositeProjectionAction()
{
  this->projectionType=vtkVolumeMapper::COMPOSITE_BLEND;
  emit projectionChanged(getProjection());
}

/***********************************************************************************//**
 *
 */
void MSQProjectionMenu::setInput(vtkmsqImageItem *imageItem)
{
  if (this->updateColormap != NULL)
  {
    this->updateColormap->Delete();
  }
  this->updateColormap = vtkCallbackCommand::New();
  this->updateColormap->SetCallback(updateColormapCallback);
  this->updateColormap->SetClientData(this);

  this->imageItem = imageItem;
  this->image = imageItem->GetImage();
  this->properties = imageItem->GetProperties();
  this->colormap = imageItem->GetColormap();
  this->projections->checkedAction()->trigger();
}

/***********************************************************************************//**
 *
 */
void MSQProjectionMenu::setActive(bool active)
{
  this->projections->setEnabled(active);
}

/***********************************************************************************//**
 *
 */
void MSQProjectionMenu::changedColormap()
{
  this->colormap = this->imageItem->GetColormap();
  if (this->projectionType==-1)
    emit projectionChanged(NULL);
}

/***********************************************************************************//**
 *
 */
vtkVolume* MSQProjectionMenu::getProjection()
{
  vtkImageShiftScale *scale = vtkImageShiftScale::New();
  scale->SetInput(image);
  scale->SetOutputScalarTypeToUnsignedChar();

  vtkFixedPointVolumeRayCastMapper *rayCastMapper = vtkFixedPointVolumeRayCastMapper::New();
  rayCastMapper->SetInputConnection(scale->GetOutputPort());
  rayCastMapper->SetBlendMode(this->projectionType);

  // Create a transfer function mapping scalar value to opacity
  vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
  opacityTransferFunction->AddSegment(0, 0.0, 255, 1.0);

  // Create a transfer function mapping scalar value to color (grey)
  vtkPiecewiseFunction *greyColorTransferFunction = vtkPiecewiseFunction::New();
  greyColorTransferFunction->AddSegment(0, 1.0, 255, 1.0);

  vtkVolumeProperty *property = vtkVolumeProperty::New();
  property->SetScalarOpacity(opacityTransferFunction);

  if (this->colormap!=NULL)
  {
    vtkColorTransferFunction *transfFunction=this->colormapFactory->createTransferFunction(this->colormap,255);
    property->SetColor(transfFunction);
  }
  else
  {
    property->SetColor(opacityTransferFunction);
  }

  property->SetInterpolationTypeToLinear();
  vtkVolume *volume = vtkVolume::New();
  volume->SetMapper(rayCastMapper);
  volume->SetProperty(property);

  if (MSQ_REORIENT){
	vtkTransform* transform = vtkTransform::New();

    vtkMatrix4x4 *dircosMatrix = this->properties->GetDirectionCosineMatrixPerpendicular();
    dircosMatrix->Transpose();
	transform->SetMatrix(dircosMatrix);

    vtkMatrix4x4 *translationMatrix = this->imageItem->FindTranslationToCenter();
    double position[3];
    for(int i = 0; i < 3; i++)
    {
    	position[i] = translationMatrix->GetElement(i, 3);
    }
    volume->AddPosition(&position[0]);

    volume->SetUserTransform(transform);

    for(int i = 0; i < 3; i++)
    {
    	position[i] = -position[i];
    }
    volume->AddPosition(&position[0]);
    volume->Update();
  }

  return volume;
}
