/*
 * MSQComponentWidget.h
 *
 *  Created on: 08/02/2012
 *      Author: William Seiti Mizuta
 */

#ifndef MSQCOMPONENTWIDGET_H
#define MSQCOMPONENTWIDGET_H

#include <QtGui>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>

#include <vtkImageData.h>

class MSQComponentWidget: public QObject
{
Q_OBJECT
public:
  MSQComponentWidget(const char *label);
  virtual ~MSQComponentWidget();

  QHBoxLayout *getLayout();
  void setImage(vtkImageData *image);

signals:
  void componentChanged(int volume);

public slots:
  void setComponent(int volume = MSQ_SLICE_MIDDLE);

private:
  QSpinBox *box;
  QHBoxLayout *layout;
  vtkImageData *image;

  static const int MSQ_SLICE_MIDDLE = -1;
};

#endif /* MSQCOMPONENTWIDGET_H */
