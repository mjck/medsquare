/*
 * MSQOrientationWidget.h
 *
 *  Created on: 07/02/2012
 *      Author: William Seiti Mizuta
 */

#ifndef MSQORIENTATIONWIDGET_H
#define MSQORIENTATIONWIDGET_H

#include <QtGui>

#include <vtkImageData.h>

#include <vtkmsqMedicalImageProperties.h>

class MSQOrientationWidget: public QObject
{
Q_OBJECT
public:
  MSQOrientationWidget(int axis, const char *label);
  virtual ~MSQOrientationWidget();

  void increment(int increment);

  QVBoxLayout *getLayout();
  int getAxis();
  int getValue();
  void setInput(vtkImageData *newImage, vtkmsqMedicalImageProperties *newProperties);
  void setAxis(int axis);

signals:
  void sliceChanged(int axis, int slice);

public slots:
  void setValue(int slice = MSQ_SLICE_MIDDLE);

private:
  int axis;

  QSpinBox *box;
  QScrollBar *bar;
  QVBoxLayout *layout;

  vtkImageData *image;
  vtkmsqMedicalImageProperties *properties;

  static const int MSQ_SLICE_MIDDLE = -1;
};

#endif /* MSQORIENTATIONWIDGET_H */
