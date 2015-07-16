/*=========================================================================

 Program:   MedSquare
 Module:    MSQWindowLevelWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQWindowLevelWidget.h"

#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkImageAccumulate.h"
#include "vtkImageExtractComponents.h"

/***********************************************************************************//**
 * 
 */
MSQWindowLevelWidget::MSQWindowLevelWidget(MedSquare *medSquare) :
    QDockWidget(tr("Window and Level"), medSquare), medSquare(medSquare)
{
  this->image = NULL;
  this->properties = NULL;

  // optimal range
  this->optimalRange[0] = 0;
  this->optimalRange[1] = 1.0;

  // normalized range
  this->intFactor = 1.0;
  this->integerRange[0] = 0.0;
  this->integerRange[1] = 1.0;

  // build interface elements
  this->buildWidget();
}

/***********************************************************************************//**
 * 
 */
MSQWindowLevelWidget::~MSQWindowLevelWidget()
{
  this->optimalRange[0] = 0.0;
  this->optimalRange[1] = 1.0;

  this->integerRange[0] = 0.0;
  this->integerRange[1] = 1.0;

  this->intFactor = 1.0;
}

/***********************************************************************************//**
 * Build widget components
 */
void MSQWindowLevelWidget::buildWidget()
{
  this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  // create placeholder widget
  QWidget *dockContents = new QWidget();
  dockContents->setFixedWidth(200);

  // set default font size
  QFont font;
  font.setPointSize(11);

  // create window/level scrollbar
  this->windowSlider = new QSlider(Qt::Horizontal);
  this->windowSlider->setEnabled(false);
  QLabel *windowLabel = new QLabel(tr("Window:"));
  windowLabel->setFont(font);
  this->levelSlider = new QSlider(Qt::Horizontal);
  this->levelSlider->setEnabled(false);
  QLabel *levelLabel = new QLabel(tr("Level:"));
  levelLabel->setFont(font);
  this->opacitySlider = new QSlider(Qt::Horizontal);
  this->opacitySlider->setEnabled(false);
  this->opacitySlider->setMinimum(0);
  this->opacitySlider->setMaximum(100);
  this->opacitySlider->setSingleStep(1);
  QLabel *opacityLabel = new QLabel(tr("Opacity:"));
  opacityLabel->setFont(font);

  // spin boxes
  windowBox = new QDoubleSpinBox();
  windowBox->setFont(font);
  windowBox->setSingleStep(0.1);
  windowBox->setDecimals(1);
  windowBox->setEnabled(false);

  // spin boxes
  levelBox = new QDoubleSpinBox();
  levelBox->setFont(font);
  levelBox->setSingleStep(0.1);
  levelBox->setDecimals(1);
  levelBox->setEnabled(false);

  // spin boxes
  opacityBox = new QDoubleSpinBox();
  opacityBox->setFont(font);
  opacityBox->setSingleStep(0.01);
  opacityBox->setDecimals(2);
  opacityBox->setRange(0.0, 1.0);
  opacityBox->setEnabled(false);

  // window and level
  connect(windowSlider, SIGNAL(valueChanged(int)), this, SLOT(setWindowValue(int)));

  connect(windowBox, SIGNAL(valueChanged(double)), this,
      SLOT(setWindowValueAsDouble(double)));

  connect(levelSlider, SIGNAL(valueChanged(int)), this, SLOT(setLevelValue(int)));

  connect(levelBox, SIGNAL(valueChanged(double)), this,
      SLOT(setLevelValueAsDouble(double)));

  connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(setOpacityValue(int)));

  connect(opacityBox, SIGNAL(valueChanged(double)), this,
      SLOT(setOpacityValueAsDouble(double)));

  QVBoxLayout *layout = new QVBoxLayout();

  QHBoxLayout *windowLayout = new QHBoxLayout();
  layout->addWidget(windowLabel);
  windowLayout->addWidget(windowBox);
  windowLayout->addWidget(windowSlider, 10);

  layout->addLayout(windowLayout);
  layout->addSpacing(10);

  QHBoxLayout *levelLayout = new QHBoxLayout();
  layout->addWidget(levelLabel);
  levelLayout->addWidget(levelBox);
  levelLayout->addWidget(levelSlider, 10);

  layout->addLayout(levelLayout);
  layout->addSpacing(10);

  QHBoxLayout *opacityLayout = new QHBoxLayout();
  layout->addWidget(opacityLabel);
  opacityLayout->addWidget(opacityBox);
  opacityLayout->addWidget(opacitySlider, 10);

  layout->addLayout(opacityLayout);
  layout->addSpacing(10);

  layout->addStretch();

  // set layout
  dockContents->setLayout(layout);

  // set dock contents
  this->setWidget(dockContents);
}

/***********************************************************************************//**
 * This function is called when an image is loaded
 */
void MSQWindowLevelWidget::setInput(vtkImageData *image,
    vtkMedicalImageProperties *properties)
{
  // initialize new image
  this->image = image;
  this->properties = properties;

  // reset colormap for the given image
  double range[2];
  this->image->GetPointData()->GetScalars()->GetRange(range);

  // is this a floating point image?
  int ndecimals = this->numberDecimals(range[1] - range[0]);
  this->intFactor = pow(10.0, ndecimals > 3 ? 3.0 : (double) ndecimals);

  // define optimal range
  this->optimalRange[0] = range[0];
  this->optimalRange[1] = range[1];
  this->calculateOptimalRange(0.02, 0.98, this->optimalRange);

  double windowLevel[2];
  double normWindowLevel[2];

  // Check if dataset contains preset window/level info
  if (this->properties != NULL && this->properties->GetNumberOfWindowLevelPresets() > 0)
  {
    double *wl = properties->GetNthWindowLevelPreset(0);

    windowLevel[0] = wl[0];
    windowLevel[1] = wl[1];

  }
  else
  {
    // determine default window/level
    this->calculateWindowLevel(this->optimalRange, windowLevel);
  }

  // normalize window/level values
  normWindowLevel[0] = windowLevel[0] * this->intFactor;
  normWindowLevel[1] = windowLevel[1] * this->intFactor;

  // normalize range to integer values
  this->integerRange[0] = range[0] * this->intFactor;
  this->integerRange[1] = range[1] * this->intFactor;

  // window varies in size
  windowSlider->setMinimum(1);
  windowSlider->setMaximum((int) (this->integerRange[1] - this->integerRange[0]) * 2);
  windowSlider->setValue((int) normWindowLevel[0]);
  windowSlider->setEnabled(true);

  windowBox->setRange(1, (range[1] - range[0]) * 2);
  windowBox->setValue(windowLevel[0]);
  windowBox->setEnabled(true);
  windowBox->setSingleStep(pow(10.0, floor(log10(range[1] - range[0]) - 2)));

  // level is the center of the window
  levelSlider->setMinimum( static_cast<int> ( this->integerRange[0] - (this->integerRange[1] - this->integerRange[0]) / 2 ) );
  levelSlider->setMaximum( static_cast<int> ( this->integerRange[1] + (this->integerRange[1] - this->integerRange[0]) / 2 ) );
  levelSlider->setValue((int) normWindowLevel[1]);
  levelSlider->setEnabled(true);

  levelBox->setRange(range[0] - (range[1] - range[0]) / 2,
      range[1] + (range[1] - range[0]) / 2);
  levelBox->setValue(windowLevel[1]);
  levelBox->setEnabled(true);
  levelBox->setSingleStep(pow(10.0, floor(log10(range[1] - range[0]) - 2)));

  opacitySlider->setValue(100);
  opacitySlider->setEnabled(true);
  opacityBox->setEnabled(true);
}

/***********************************************************************************//**
 * Determine default window and level
 */
void MSQWindowLevelWidget::calculateOptimalRange(double percentLow, double percentHigh,
    double optimalRange[2])
{
  double range[2];
  this->image->GetPointData()->GetScalars()->GetRange(range);

  int bins = 512;

  if ((range[1] - range[0]) < 20)
    bins = int((range[1] - range[0]) * this->intFactor) + 1;
  else if (range[1] - range[0] < bins)
    bins = int(range[1] - range[0]) + 1;

  double spacing = (range[1] - range[0]) / double(bins - 1);

  // Calculate histogram
  vtkImageAccumulate* accumulate = vtkImageAccumulate::New();

  // For multiple volumes, use the first one...
  if (this->image->GetNumberOfScalarComponents() > 1)
  {
    vtkImageExtractComponents* ext = vtkImageExtractComponents::New();
    ext->SetInput(image);
    ext->SetComponents(0);
    ext->Update();
    accumulate->SetInput(ext->GetOutput());
    ext->Delete();
  }
  else
  {
    accumulate->SetInput(this->image);
  }

  // Calculate histogram
  accumulate->SetComponentExtent(0, bins - 1, 0, 0, 0, 0);
  accumulate->SetComponentOrigin(range[0], 0, 0);
  accumulate->SetComponentSpacing(spacing, 1, 1);
  accumulate->Update();

  // Get histogram data
  vtkDataArray* data = accumulate->GetOutput()->GetPointData()->GetScalars();

  double total = accumulate->GetVoxelCount();

  int foundperlow = 0, foundperhigh = 0;

  double v = 0.0;
  for (int ib = 0; ib < bins; ib++)
  {
    v += data->GetComponent(ib, 0);
    if (foundperlow == 0)
    {
      if (v / total > percentLow)
      {
        foundperlow = 1;
        optimalRange[0] = range[0] + ib * spacing;
      }
    }

    if (foundperhigh == 0)
    {
      if (v / total > percentHigh)
      {
        foundperhigh = 1;
        optimalRange[1] = range[0] + ib * spacing;
        ib = bins;
      }
    }
  }

  accumulate->Delete();
}

/***********************************************************************************//**
 * Retrieve optimal range for those interested
 */
void MSQWindowLevelWidget::getOptimalRange(double range[2])
{
  range[0] = this->optimalRange[0];
  range[1] = this->optimalRange[1];
}

/***********************************************************************************//**
 * Determine default window and level
 */
void MSQWindowLevelWidget::calculateWindowLevel(double range[2], double windowLevel[2])
{
  // these are simple heuristics - they must go!
  windowLevel[0] = range[1] - range[0];
  windowLevel[1] = range[0] + 1.25 * (windowLevel[0] / 2.0);
}

/***********************************************************************************//**
 * Sets the window value
 */
void MSQWindowLevelWidget::setWindow(double value)
{
  this->windowSlider->setValue((int) value);
}

/***********************************************************************************//**
 * Gets the window value
 */
double MSQWindowLevelWidget::getWindow()
{
  return (double) this->windowSlider->value() / this->intFactor;
}

/***********************************************************************************//**
 * Sets the level value
 */
void MSQWindowLevelWidget::setLevel(double value)
{
  this->levelSlider->setValue((int) value);
}

/***********************************************************************************//**
 * Gets the level value
 */
double MSQWindowLevelWidget::getLevel()
{
  return (double) this->levelSlider->value() / this->intFactor;
}

/***********************************************************************************//**
 * SLOT: sets window value
 */
void MSQWindowLevelWidget::setWindowValue(int value)
{
  double dValue = (double) value / this->intFactor;

  windowBox->setValue(dValue);

  emit windowChanged(dValue);
}

/***********************************************************************************//**
 * SLOT: sets window value
 */
void MSQWindowLevelWidget::setWindowValueAsDouble(double value)
{
  windowSlider->setValue ( static_cast<int> ( value * this->intFactor ) );
}

/***********************************************************************************//**
 * SLOT: sets level value
 */
void MSQWindowLevelWidget::setLevelValueAsDouble(double value)
{
  levelSlider->setValue ( static_cast<int> ( value * this->intFactor ) );
}

/***********************************************************************************//**
 * SLOT: sets level value
 */
void MSQWindowLevelWidget::setLevelValue(int value)
{
  double dValue = (double) value / this->intFactor;
  levelBox->setValue(dValue);
  emit levelChanged(dValue);
}

/***********************************************************************************//**
 * SLOT: sets opacity value
 */
void MSQWindowLevelWidget::setOpacityValueAsDouble(double value)
{
  opacitySlider->setValue ( static_cast<int> ( value * 100 ) );
}

/***********************************************************************************//**
 * SLOT: sets opacity value
 */
void MSQWindowLevelWidget::setOpacityValue(int value)
{
	double dValue = (double) value / 100.0;
	opacityBox->setValue(dValue);
	emit opacityChanged(dValue);
}

/***********************************************************************************//**
 * 
 */
int MSQWindowLevelWidget::numberDecimals(double value)
{
  int number = 0;

  value -= (int) value;
  while (value > 0)
  {
    number++;
    value *= 10;
    value -= (int) value;
  }
  return number;
}

