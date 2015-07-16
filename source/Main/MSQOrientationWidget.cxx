/*=========================================================================

 Program:   MedSquare
 Module:    MSQOrientationWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQOrientationWidget.h"

/***********************************************************************************//**
 * 
 */
MSQOrientationWidget::MSQOrientationWidget(int axis, const char *name)
{
  QFont font;
  font.setPointSize(11);

  this->axis = axis;

  this->bar = new QScrollBar(Qt::Horizontal);
  this->bar->setEnabled(false);

  this->box = new QSpinBox();
  this->box->setFont(font);
  this->box->setSingleStep(1);
  this->box->setEnabled(false);

  QLabel *label = new QLabel(tr(name));
  label->setFont(font);

  this->connect(bar, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
  this->connect(bar, SIGNAL(valueChanged(int)), box, SLOT(setValue(int)));
  this->connect(box, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
  this->connect(box, SIGNAL(valueChanged(int)), bar, SLOT(setValue(int)));

  QHBoxLayout *hLayout = new QHBoxLayout();
  hLayout->addWidget(box);
  hLayout->addWidget(bar, 10);

  this->layout = new QVBoxLayout();
  this->layout->addWidget(label);
  this->layout->addLayout(hLayout);
}

/***********************************************************************************//**
 * 
 */
MSQOrientationWidget::~MSQOrientationWidget()
{
}

/***********************************************************************************//**
 * 
 */
void MSQOrientationWidget::increment(int increment)
{
  int newValue = getValue() + increment;

  if (newValue < box->minimum())
    newValue = box->minimum();
  else if (newValue > box->maximum())
    newValue = box->maximum();

  setValue(newValue);
}

/***********************************************************************************//**
 * 
 */
void MSQOrientationWidget::setValue(int slice)
{
  if (slice == MSQ_SLICE_MIDDLE)
  {
    int extent[6];
    int index = this->axis << 1;
    if (this->image != NULL)
    {
      this->image->GetExtent(extent);
      if (MSQ_REORIENT){
        this->properties->GetReorientedExtent(extent, extent);
      }
      slice = (extent[index] + extent[index + 1]) >> 1;
      this->box->setRange(extent[index], extent[index + 1]);
      this->bar->setRange(extent[index], extent[index + 1]);
      this->box->setValue(slice);
      this->box->setEnabled(true);
      this->bar->setEnabled(true);
      emit sliceChanged(this->axis, slice);
    }
    return;
  }

  this->box->setValue(slice);

  emit sliceChanged(this->axis, slice);
}

/***********************************************************************************//**
 * 
 */
QVBoxLayout *MSQOrientationWidget::getLayout()
{
  return this->layout;
}

/***********************************************************************************//**
 * 
 */
int MSQOrientationWidget::getAxis()
{
  return this->axis;
}

/***********************************************************************************//**
 * 
 */
int MSQOrientationWidget::getValue()
{
  return this->box->value();
}

/***********************************************************************************//**
 * 
 */
void MSQOrientationWidget::setInput(vtkImageData *newImage, vtkmsqMedicalImageProperties *newProperties)
{
  this->image = newImage;
  this->properties = newProperties;
}

/***********************************************************************************//**
 * 
 */
void MSQOrientationWidget::setAxis(int axis)
{
  this->axis = axis;
}
