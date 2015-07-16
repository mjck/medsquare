/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqSagittalImagePlane.h

 Copyright (c) Marcel Parolin Jackowski
 All rights reserved.

 See http://www.medsquare.org for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above link for more information.

 =========================================================================*/
// .NAME vtkmsqSagittalImagePlane - create a image slice with outline in 3D
// .SECTION Description
// To be done.
//
#ifndef __vtkmsqSagittalImagePlane_h
#define __vtkmsqSagittalImagePlane_h

#include "vtkmsqGraphicsWin32Header.h"
#include "vtkmsqImagePlane.h"

#include "vtkAssembly.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"
#include "vtkImageReslice.h"
#include "vtkPropPicker.h"

class vtkImageData;
class vtkLookupTable;
class vtkScalarsToColors;
class vtkPolyDataMapper;
class vtkRenderer;
class vtkImageActor;
class vtkActor;

class VTK_MSQ_GRAPHICS_EXPORT vtkmsqSagittalImagePlane: public vtkmsqImagePlane
{
public:
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkTypeMacro(vtkmsqSagittalImagePlane,vtkmsqImagePlane);

  // Description:
  // Construct an image plane perpendicular to z-axis, width
  // and height 1.0, and centered at the origin.
  static vtkmsqSagittalImagePlane *New();

protected:
  vtkmsqSagittalImagePlane();
  virtual ~vtkmsqSagittalImagePlane();

  virtual vtkSmartPointer<vtkMatrix4x4> GetReslicingMatrix(int slice);
  virtual void UpdateCoords(vtkSmartPointer<vtkMatrix4x4> resliceMatrix);
  virtual void AdjustPickPosition(double pickedPosition[3]);
  virtual void ComputePickedImageCoordinates(double pickedPosition[3], double imageCoords[3]);

private:
  vtkmsqSagittalImagePlane(const vtkmsqSagittalImagePlane&); // Not implemented.
  void operator=(const vtkmsqSagittalImagePlane&); // Not implemented.
};

#endif
