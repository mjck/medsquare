/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqRectangleActor2D.h

 Copyright (c) Marcel Parolin Jackowski, Choukri Mekkaoui
 All rights reserved.

 See http://www.medsquare.org for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above link for more information.

 =========================================================================*/
// .NAME vtkmsqRectangleActor2D - create a rectangular frame in 2D
// .SECTION Description
// To be done.
//
#ifndef __vtkmsqRectangleActor2D_h
#define __vtkmsqRectangleActor2D_h

#include "vtkmsqGraphicsWin32Header.h"
#include "vtkActor2D.h"

class vtkPolyData;
class vtkPolyDataMapper2D;

class VTK_MSQ_GRAPHICS_EXPORT vtkmsqRectangleActor2D: public vtkActor2D
{
public:
  vtkTypeMacro(vtkmsqRectangleActor2D,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Instantiate object.
  static vtkmsqRectangleActor2D *New();

  // Description:
  // Specify the position of the first point defining the axis.
  // Note: backward compatibility only, use vtkActor2D's Position instead.
  virtual vtkCoordinate *GetPoint1Coordinate()
    { return this->GetPositionCoordinate(); };
  virtual void SetPoint1(double x[2]) { this->SetPosition(x); };
  virtual void SetPoint1(double x, double y) { this->SetPosition(x,y); };
  virtual double *GetPoint1() { return this->GetPosition(); };

  // Description:
  // Specify the position of the second point defining the rectangle. 
  // Note: backward compatibility only, use vtkActor2D's Position2 instead.
  virtual vtkCoordinate *GetPoint2Coordinate()
    { return this->GetPosition2Coordinate(); };
  virtual void SetPoint2(double x[2]) { this->SetPosition2(x); };
  virtual void SetPoint2(double x, double y) { this->SetPosition2(x,y); };
  virtual double *GetPoint2() { return this->GetPosition2(); };

  virtual void SetVisibility(int visible) { this->RectangleActor->SetVisibility(visible); };
  virtual int GetVisibility() { return this->RectangleActor->GetVisibility(); };

  // Description:
  // Draw the axis.
  int RenderOverlay(vtkViewport* viewport);
  int RenderOpaqueGeometry(vtkViewport* viewport);
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport *) {return 0;}

  // Description:
  // Does this prop have some translucent polygonal geometry?
  virtual int HasTranslucentPolygonalGeometry();

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

protected:
  vtkmsqRectangleActor2D();
  ~vtkmsqRectangleActor2D();
 
  virtual void BuildRectangle(vtkViewport *viewport);

  vtkPolyData         *Rectangle;
  vtkPolyDataMapper2D *RectangleMapper;
  vtkActor2D          *RectangleActor;

  vtkTimeStamp  BuildTime;

private:
  vtkmsqRectangleActor2D(const vtkmsqRectangleActor2D&);  // Not implemented.
  void operator=(const vtkmsqRectangleActor2D&);  // Not implemented.
};


#endif
