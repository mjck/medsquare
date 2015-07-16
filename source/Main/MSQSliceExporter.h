/*=========================================================================

 Program:   MedSquare
 Module:    MSQAxialRenderWidget.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_SLICE_EXPORTER_H
#define MSQ_SLICE_EXPORTER_H

#include "qobject.h"
#include "qstring.h"

class MedSquare;
class vtkObject;
class vtkmsqImageItem;

class MSQSliceExporter : public QObject
{
  Q_OBJECT

public:
  MSQSliceExporter(MedSquare *medSquare);
  void exportSlices(vtkmsqImageItem *image,QString path,QString prefix, QString format);
private:
  MedSquare *medSquare;
};

#endif
