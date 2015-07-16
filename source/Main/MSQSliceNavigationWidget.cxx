/*=========================================================================

 Program:   MedSquare
 Module:    MSQSliceNavigationWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQSliceNavigationWidget.h"

/***********************************************************************************//**
 * 
 */
MSQSliceNavigationWidget::MSQSliceNavigationWidget(MedSquare *medSquare) :
    QDockWidget(tr("Slice Navigation"), medSquare)
{
  this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  // create placeholder widget
  QWidget *dockContents = new QWidget();
  dockContents->setFixedWidth(200);

  this->axial = new MSQOrientationWidget(AXIAL_RENDERER, "Axial:");
  this->coronal = new MSQOrientationWidget(CORONAL_RENDERER, "Coronal:");
  this->sagittal = new MSQOrientationWidget(SAGITTAL_RENDERER, "Sagital:");

  connect(axial, SIGNAL(sliceChanged(int, int)), this, SIGNAL(sliceChanged(int, int)));
  connect(coronal, SIGNAL(sliceChanged(int, int)), this, SIGNAL(sliceChanged(int, int)));
  connect(sagittal, SIGNAL(sliceChanged(int, int)), this, SIGNAL(sliceChanged(int, int)));

  QVBoxLayout *layout = new QVBoxLayout();

  layout->addLayout(this->sagittal->getLayout());
  layout->addSpacing(10);
  layout->addLayout(this->coronal->getLayout());
  layout->addSpacing(10);
  layout->addLayout(this->axial->getLayout());
  layout->addSpacing(10);

  this->component = new MSQComponentWidget("Component:");
  connect(component, SIGNAL(componentChanged(int)), this, SIGNAL(componentChanged(int)));

  layout->addLayout(component->getLayout());
  layout->addStretch();

  // set layout
  dockContents->setLayout(layout);

  // set dock contents
  this->setWidget(dockContents);
}

/***********************************************************************************//**
 * 
 */
MSQSliceNavigationWidget::~MSQSliceNavigationWidget()
{
}

/***********************************************************************************//**
 * This function is called when an image is loaded
 */
void MSQSliceNavigationWidget::setInput(vtkImageData *image, vtkmsqMedicalImageProperties *properties)
{
  this->image = image;

  this->axial->setInput(image, properties);
  this->coronal->setInput(image, properties);
  this->sagittal->setInput(image, properties);
  this->component->setImage(image);

  // emit signals upon new image
  this->component->setComponent();
  this->axial->setValue();
  this->coronal->setValue();
  this->sagittal->setValue();
}

/***********************************************************************************//**
 * 
 */
void MSQSliceNavigationWidget::incrementAxial(int increment)
{
  this->axial->increment(increment);
}

/***********************************************************************************//**
 * 
 */
void MSQSliceNavigationWidget::incrementCoronal(int increment)
{
  this->coronal->increment(increment);
}

/***********************************************************************************//**
 * 
 */
void MSQSliceNavigationWidget::incrementSagittal(int increment)
{
  this->sagittal->increment(increment);
}

/***********************************************************************************//**
 * 
 */
void MSQSliceNavigationWidget::setAxial(int value)
{
  this->axial->setValue(value);
}

/***********************************************************************************//**
 * 
 */
void MSQSliceNavigationWidget::setCoronal(int value)
{
  this->coronal->setValue(value);
}

/***********************************************************************************//**
 * 
 */
void MSQSliceNavigationWidget::setSagittal(int value)
{
  this->sagittal->setValue(value);
}
