/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomHeaderViewer.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOMHEADER_VIEWER_H
#define MSQ_DICOMHEADER_VIEWER_H

#include <QtGui>

class MSQDicomHeaderViewer : public QWidget
{
Q_OBJECT

public:
  MSQDicomHeaderViewer();
  virtual ~MSQDicomHeaderViewer();

  //void refresh();

  virtual void setInput(const QString& fileName);
  //virtual void setSlice(int axial, int sagittal, int coronal) = 0;
  //virtual void setInput(vtkmsqImageItem *imageItem) = 0;
  //virtual double* GetOrigin() = 0;
  //virtual double* GetPoint1() = 0;
  //virtual double* GetPoint2() = 0;

public slots:
  //virtual void setComponent(int component) = 0;
  //virtual void setLevel(double value) = 0;
  //virtual void setWindow(double value) = 0;
  //virtual void setOpacity(double value) = 0;

protected:
  //QVTKWidget2 *vtkWidget;

  QTreeWidget *treeTag; 
  //vtkmsqRectangleActor2D *frame;
  //vtkmsqInteractorStyleImage *interStyle;
  //vtkRenderer *renderer;
  
  void buildFrame();
};

#endif

