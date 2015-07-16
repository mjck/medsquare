#ifndef MSQ_WINDOWLEVEL_WIDGET_H
#define MSQ_WINDOWLEVEL_WIDGET_H

#include <QtGui>

#include "vtkImageData.h"
#include "vtkMedicalImageProperties.h"

#include "MedSquare.h"

class MSQWindowLevelWidget: public QDockWidget
{
Q_OBJECT

public:
  MSQWindowLevelWidget(MedSquare *medSquare);
  ~MSQWindowLevelWidget();

  // set image input to widget
  void setInput(vtkImageData *image, vtkMedicalImageProperties *properties);

  // sets window/level
  void setWindow(double value);
  void setLevel(double value);

  // gets window/level
  double getWindow();
  double getLevel();

  // gets optimal range
  void getOptimalRange(double range[2]);

signals:
  // these are emitted upon a change in window/level
  void windowChanged(double value);
  void levelChanged(double value);
  void opacityChanged(double value);

private slots:
  void setWindowValue(int value);
  void setWindowValueAsDouble(double value);
  void setLevelValueAsDouble(double value);
  void setLevelValue(int value);
  void setOpacityValue(int value);
  void setOpacityValueAsDouble(double value);

private:
  MedSquare *medSquare;

  QSlider *windowSlider, *levelSlider, *opacitySlider;
  QDoubleSpinBox *windowBox, *levelBox, *opacityBox;

  vtkImageData *image;
  vtkMedicalImageProperties *properties;

  double optimalRange[2]; // optimal contrast range
  double integerRange[2]; // integer converted range
  double intFactor; // integer conversion factor

  void buildWidget();
  void calculateOptimalRange(double percentLow, double percentHigh,
      double optimalRange[2]);
  void calculateWindowLevel(double range[2], double windowLevel[2]);
  int numberDecimals(double value);
};

#endif
