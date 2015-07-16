/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqImagePlane.h

 Copyright (c) Marcel Parolin Jackowski
 All rights reserved.

 See http://www.medsquare.org for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above link for more information.

 =========================================================================*/
// .NAME vtkmsqImagePlane - create a image slice with outline in 3D
// .SECTION Description
// To be done.
//
#ifndef __vtkmsqImagePlane_h
#define __vtkmsqImagePlane_h

#include "vtkAssembly.h"

#include "vtkSmartPointer.h"
#include "vtkmsqGraphicsWin32Header.h"

class vtkmsqFrameSource;
class vtkmsqImageItem;
class vtkmsqLookupTable;
class vtkmsqMedicalImageProperties;

class vtkActor;
class vtkImageChangeInformation;
class vtkImageData;
class vtkImageReslice;
class vtkPlaneSource;
class vtkPropPicker;
class vtkRenderer;
class vtkTexture;
class vtkCallbackCommand;

class VTK_MSQ_GRAPHICS_EXPORT vtkmsqImagePlane: public vtkAssembly
{
public:
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkTypeMacro(vtkmsqImagePlane, vtkAssembly);

  static vtkMatrix4x4 *AxialPlaneOrientationMatrix();
  static vtkMatrix4x4 *SagittalPlaneOrientationMatrix();
  static vtkMatrix4x4 *CoronalPlaneOrientationMatrix();

  // Description:
  // Set plane orientation and center
  void SetPlaneOrientationMatrix(vtkSmartPointer<vtkMatrix4x4> orientation);

  void SetOrientationToAxial();
  void SetOrientationToSagittal();
  void SetOrientationToCoronal();

  // Description:
  // Set input image data
  void SetInput(vtkmsqImageItem *imageItem);

  void SetWindow(double window);
  void SetLevel(double level);

  // Description:
  // Set/Get slice number
  void SetSliceNumber(int slice);
  vtkGetMacro(SliceNumber,int);

  // Description:
  // Set active component from lookup table
  void SetActiveComponent(int comp);

  // Description:
  // Compute picking error tolerance based on current image size
  double GetPickingErrorTolerance();

  // Description:
  // Transform 2-d selection into 3-d image coordinates and return intensity
  double Pick(double selectionX, double selectionY, vtkRenderer *renderer, double imageCoords[3]);

  void SetOpacity(double value);

  double* GetOrigin();
  double* GetPoint1();
  double* GetPoint2();

  // Description:
  // Reloads the lookup table from the image item used as input
  void UpdateLookupTable();

protected:
  vtkmsqImagePlane();
  virtual ~vtkmsqImagePlane();

  // Description:
  // Adjust the coordinate orthogonal to this image plane, so that it is always inside our bounds
  virtual void AdjustPickPosition(double pickedPosition[3]) = 0;

  // Description:
  // Convert the given picked position to voxel coordinates
  virtual void ComputePickedImageCoordinates(double pickedPosition[3], double imageCoords[3]) = 0;

  // Needed by subclasses to compute the picked voxel
  int SliceNumber;

  vtkmsqImageItem *InputImageItem;
  vtkImageData *InputImage;
  vtkmsqMedicalImageProperties *InputProperties;
  vtkmsqLookupTable *LookupTable;
  vtkImageChangeInformation *ImageChange;
  vtkImageData *ChangedImage;

  vtkmsqFrameSource *FrameSource;
  vtkPlaneSource *PlaneSource;

  vtkTexture *ImageTexture;
  vtkImageReslice *ImageReslice;
  vtkPropPicker *ImagePicker;

  vtkActor *FrameActor;
  vtkActor *ImageActor;

  vtkSmartPointer<vtkMatrix4x4> PlaneOrientationMatrix;

  vtkCallbackCommand *UpdateColormap;

private:
  vtkmsqImagePlane(const vtkmsqImagePlane&); // Not implemented.
  void operator=(const vtkmsqImagePlane&); // Not implemented.

  void BuildPlane();
  double ImageIntensityAt(double position[3]);
  void UpdateOpacity();

  // Description:
  // Update the props used in this image plane based on the given reslicing matrix
  void UpdateCoords(vtkSmartPointer<vtkMatrix4x4> resliceMatrix);

  // Description:
  // Set/Get lookup table
  void SetLookupTable(vtkmsqLookupTable *colormap);

  vtkSmartPointer<vtkMatrix4x4> FindRotationFromOrientation(int orientation);
  vtkSmartPointer<vtkMatrix4x4> FindTranslationToCenter(double multiplier = 1.0);
};

#endif

