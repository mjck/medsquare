#ifndef MSQ_SLICENAVIGATION_WIDGET_H
#define MSQ_SLICENAVIGATION_WIDGET_H

#include <QtGui>
#include <QDockWidget>

#include "vtkImageData.h"

#include "vtkmsqMedicalImageProperties.h"

#include "MedSquare.h"
#include "MSQOrientationWidget.h"
#include "MSQComponentWidget.h"

enum MSQOrientationType
{
  SAGITTAL = 0, CORONAL = 1, AXIAL = 2, OTHER
};
enum MSQAxisType
{
  SAGITTAL_RENDERER = 0, CORONAL_RENDERER = 1, AXIAL_RENDERER = 2
};

class MSQSliceNavigationWidget: public QDockWidget
{
Q_OBJECT
public:
  MSQSliceNavigationWidget(MedSquare *medSquare);
  ~MSQSliceNavigationWidget();

  // sets image input to widget
  void setInput(vtkImageData *image, vtkmsqMedicalImageProperties *properties);

  // increment slices
  void incrementAxial(int increment);
  void incrementCoronal(int increment);
  void incrementSagittal(int increment);

  // set slices
  void setAxial(int value);
  void setCoronal(int value);
  void setSagittal(int value);

  // get orientation mappings
  int getAxialAxis()
  {
    return axial->getAxis();
  }
  int getCoronalAxis()
  {
    return coronal->getAxis();
  }
  int getSagittalAxis()
  {
    return sagittal->getAxis();
  }

signals:
  // these are emitted whenever an update in component or slice occur
  void componentChanged(int volume);
  void sliceChanged(int axis, int slice);

private:
  MSQOrientationWidget *axial, *coronal, *sagittal;
  MSQComponentWidget *component;

  vtkImageData *image;
};

#endif
