/*=========================================================================

 Program:   MedSquare
 Module:    MSQComponentWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQComponentWidget.h"

/***********************************************************************************//**
 * 
 */
MSQComponentWidget::MSQComponentWidget(const char *name)
{
  QFont font;
  font.setPointSize(11);

  QLabel *label = new QLabel(tr(name));
  label->setFont(font);

  this->box = new QSpinBox();
  this->box->setFont(font);
  this->box->setSingleStep(1);
  this->box->setEnabled(false);

  this->connect(box, SIGNAL(valueChanged(int)), this, SLOT(setComponent(int)));

  this->layout = new QHBoxLayout();
  this->layout->addWidget(label, 10, Qt::AlignRight);
  this->layout->addWidget(box, 0, Qt::AlignLeft);
}

/***********************************************************************************//**
 * 
 */
MSQComponentWidget::~MSQComponentWidget()
{
}

/***********************************************************************************//**
 * 
 */
void MSQComponentWidget::setComponent(int volume)
{
  if (volume == MSQ_SLICE_MIDDLE)
  {
    int comp = image->GetNumberOfScalarComponents();

    this->box->setRange(0, comp - 1);
    this->box->setValue(0);
    this->box->setEnabled(comp > 1);

    return;
  }

  emit componentChanged(volume);
}

/***********************************************************************************//**
 * 
 */
QHBoxLayout *MSQComponentWidget::getLayout()
{
  return this->layout;
}

/***********************************************************************************//**
 * 
 */
void MSQComponentWidget::setImage(vtkImageData *image)
{
  this->image = image;
}
