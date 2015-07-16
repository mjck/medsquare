/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqFrameSource.h

 Copyright (c) Marcel Parolin Jackowski
 All rights reserved.

 See http://www.medsquare.org for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above link for more information.

 =========================================================================*/
// .NAME vtkmsqFrameSource - create a frame for a quadrilateral in 3D
// .SECTION Description
// To be done.
//
#ifndef __vtkmsqFrameSource_h
#define __vtkmsqFrameSource_h

#include "vtkmsqGraphicsWin32Header.h"
#include "vtkPolyDataAlgorithm.h"

class VTK_MSQ_GRAPHICS_EXPORT vtkmsqFrameSource: public vtkPolyDataAlgorithm
{
public:
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkTypeMacro(vtkmsqFrameSource,vtkPolyDataAlgorithm);

  // Description:
  // Construct plane perpendicular to z-axis, width
  // and height 1.0, and centered at the origin.
  static vtkmsqFrameSource *New();

  // Description:
  // Specify a point defining the origin of the plane.
  vtkSetVector3Macro(Origin,double)
  ;vtkGetVectorMacro(Origin,double,3)
  ;

  // Description:
  // Specify a point defining the top-left corner of the plane.
  void SetPoint1(double x, double y, double z);
  void SetPoint1(double pnt[3]);vtkGetVectorMacro(Point1,double,3)
  ;

  // Description:
  // Specify a point defining the bottom-right corner of the plane.
  void SetPoint2(double x, double y, double z);
  void SetPoint2(double pnt[3]);vtkGetVectorMacro(Point2,double,3)
  ;

  // Description
  // Specify the size of the corner. A value of 1.0, will; draw
  // the entire outline of the plane. A value less than that will
  // draw corners.
  vtkGetMacro(CornerSize, double)
  ;vtkSetMacro(CornerSize, double)
  ;

  // Description:
  // Set/Get the center of the plane. Works in conjunction with the plane
  // normal to position the plane. Don't use this method to define the plane.
  // Instead, use it to move the plane to a new center point.
  void SetCenter(double x, double y, double z);
  void SetCenter(double center[3]);vtkGetVectorMacro(Center,double,3)
  ;

  // Description:
  // Set/Get the plane normal. Works in conjunction with the plane center to
  // orient the plane. Don't use this method to define the plane. Instead, use
  // it to rotate the plane around the current center point.
  void SetNormal(double nx, double ny, double nz);
  void SetNormal(double n[3]);vtkGetVectorMacro(Normal,double,3)
  ;

  // Description:
  // Translate the plane in the direction of the normal by the
  // distance specified.  Negative values move the plane in the
  // opposite direction.

protected:
  vtkmsqFrameSource();
  ~vtkmsqFrameSource()
  {
  }
  ;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  double CornerSize;
  double Point1[3];
  double Point2[3];
  double Center[3];
  double Origin[3];
  double Normal[3];

  int UpdatePlane(double v1[3], double v2[3]);
private:
  vtkmsqFrameSource(const vtkmsqFrameSource&); // Not implemented.
  void operator=(const vtkmsqFrameSource&); // Not implemented.
};

#endif
