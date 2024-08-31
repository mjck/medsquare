/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqImagePlane.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqImagePlane.h"

#include "vtkmsqFrameSource.h"
#include "vtkmsqImageItem.h"
#include "vtkmsqLookupTable.h"
#include "vtkmsqMedicalImageProperties.h"

#include "vtkActor.h"
#include "vtkCell.h"
#include "vtkImageData.h"
#include "vtkImageReslice.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkPropPicker.h"
#include "vtkTexture.h"
#include "vtkCallbackCommand.h"

void UpdateColormapCallback(vtkObject *caller, unsigned long eid, void *clientData, void *callData)
{
  static_cast<vtkmsqImagePlane *>(clientData)->UpdateLookupTable();
}

/***********************************************************************************//**
 * Instantiate this object.
 */
vtkmsqImagePlane::vtkmsqImagePlane()
{
  this->SliceNumber = 0;

  this->InputImageItem = NULL;
  this->InputImage = NULL;
  this->InputProperties = NULL;
  this->UpdateColormap = NULL;

  // Build image plane
  this->BuildPlane();
}

/***********************************************************************************//**
 *
 */
vtkmsqImagePlane::~vtkmsqImagePlane()
{
  if (this->InputImage)
    this->InputImage->Delete();

  if (this->ImageReslice)
    this->ImageReslice->Delete();

  if (this->ImageTexture)
    this->ImageTexture->Delete();

  if (this->ImageActor)
    this->ImageActor->Delete();

  if (this->ImagePicker)
    this->ImagePicker->Delete();

  if (this->FrameSource)
    this->FrameSource->Delete();

  if (this->FrameActor)
    this->FrameActor->Delete();

  if (this->InputProperties)
    this->InputProperties->Delete();
}

/***********************************************************************************//**
 *
 */
vtkMatrix4x4 *vtkmsqImagePlane::AxialPlaneOrientationMatrix()
{
  return vtkMatrix4x4::New();
}

/***********************************************************************************//**
 *
 */
vtkMatrix4x4 *vtkmsqImagePlane::CoronalPlaneOrientationMatrix()
{
  vtkMatrix4x4 *planeOrientation = vtkMatrix4x4::New();

  static double elements[16] = { 1, 0, 0, 0,
                                 0, 0, 1, 0,
                                 0,-1, 0, 0,
                                 0, 0, 0, 1 };
  planeOrientation->DeepCopy(elements);
  return planeOrientation;
}

/***********************************************************************************//**
 *
 */
vtkMatrix4x4 *vtkmsqImagePlane::SagittalPlaneOrientationMatrix()
{
  vtkMatrix4x4 *planeOrientation = vtkMatrix4x4::New();
  static double elements[16] = {  0, 0, 1, 0,
                                  1, 0, 0, 0,
                                  0, 1, 0, 0,
                                  0, 0, 0, 1 };
  planeOrientation->DeepCopy(elements);
  return planeOrientation;
}

/***********************************************************************************//**
 *  This function is called during the construction of vtkmsqImagePlane.
 *
 *  Receive a rotation matrix, that is, a 4x4 matrix where the last column indicates no translation
 *
 *  Set this->PlaneOrientationMatrix as the received matrix. This matrix indicates
 *  the orientation of the plane in space. Affects how the plane appear in the visualization window
 *  and how the volumetric data is sliced.
 *
 */
void vtkmsqImagePlane::SetPlaneOrientationMatrix(vtkSmartPointer<vtkMatrix4x4>  orientation)
{
  this->PlaneOrientationMatrix = orientation;
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetOrientationToAxial()
{
  this->SetPlaneOrientationMatrix(vtkmsqImagePlane::AxialPlaneOrientationMatrix());
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetOrientationToSagittal()
{
  this->SetPlaneOrientationMatrix(vtkmsqImagePlane::SagittalPlaneOrientationMatrix());
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetOrientationToCoronal()
{
  this->SetPlaneOrientationMatrix(vtkmsqImagePlane::CoronalPlaneOrientationMatrix());
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetInput(vtkmsqImageItem *newImageItem)
{
  this->InputImageItem = newImageItem;

  if (this->UpdateColormap)
  {
    this->UpdateColormap->Delete();
  }
  this->UpdateColormap = vtkCallbackCommand::New();
  this->UpdateColormap->SetCallback(UpdateColormapCallback);
  this->UpdateColormap->SetClientData(this);
  this->InputImageItem->AddObserver("ColormapChanged", this->UpdateColormap);

  // Copy image data
  this->InputImage->ShallowCopy(newImageItem->GetImage());
  this->InputProperties->DeepCopy(newImageItem->GetProperties());

  // Sets image reslice
  this->ImageReslice->SetInputData(this->InputImage);

  // Sets lookup table
  this->SetLookupTable(newImageItem->GetColormap());

  // set image actor data
  this->ImageTexture->SetInputConnection(this->ImageReslice->GetOutputPort());

  this->Modified();
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::UpdateLookupTable()
{
  this->SetLookupTable(this->InputImageItem->GetColormap());
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetLookupTable(vtkmsqLookupTable *colormap)
{
  // Assigns new colormap
  this->LookupTable = colormap;
  this->LookupTable->SetVectorComponent(0);
  this->LookupTable->SetLevel(128);
  this->LookupTable->SetWindow(256);
  this->LookupTable->Modified();

  this->ImageTexture->SetLookupTable(this->LookupTable);
  this->Modified();
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetSliceNumber(int slice)
{
  this->SliceNumber = slice;

  vtkSmartPointer<vtkMatrix4x4> resliceAxes = this->InputImageItem->FindReslicingMatrix(slice, this->PlaneOrientationMatrix);
  vtkSmartPointer<vtkMatrix4x4> resliceAxes2 = this->InputImageItem->FindReslicingMatrix2(slice, this->PlaneOrientationMatrix);

  // select slice
  this->ImageReslice->SetResliceAxes(resliceAxes2);

  this->UpdateCoords(resliceAxes);

  this->Modified();
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetWindow(double window)
{
  this->LookupTable->SetWindow(window);
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetLevel(double level)
{
  this->LookupTable->SetLevel(level);
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetActiveComponent(int volume)
{
  this->LookupTable->SetVectorComponent(volume);
}

/***********************************************************************************//**
 *
 */
double vtkmsqImagePlane::GetPickingErrorTolerance()
{
  double tol2 = this->InputImage->GetLength();
  tol2 = tol2 ? tol2 * tol2 / 1000.0 : 0.001;
  return tol2;
}

/***********************************************************************************//**
 *
 */
double vtkmsqImagePlane::Pick(double selectionX, double selectionY, vtkRenderer *renderer,
    double imageCoords[3])
{
  // pick at mouse location
  if (this->ImagePicker->Pick(selectionX, selectionY, 0.0, renderer) == 0) {
    return -1;
  }

  double pos[3];
  this->ImagePicker->GetPickPosition(pos);
  this->AdjustPickPosition(pos);

  vtkActor *pickedActor = (vtkActor *) this->ImagePicker->GetViewProp();
  cout << pickedActor << endl;
  
  this->ComputePickedImageCoordinates(pos, imageCoords);

  if (MSQ_REORIENT){
    int extent[6];
    double spacing[3];
    double origin[6];
    this->InputImage->GetExtent(extent);
    this->InputImage->GetSpacing(spacing);
    this->InputImage->GetOrigin(origin);
    this->InputProperties->GetOriginalDouble3(pos, pos);

    vtkSmartPointer<vtkMatrix4x4> dircosMatrix = this->InputProperties->GetDirectionCosineMatrixPerpendicular();

    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 3; j++){
        if (dircosMatrix->GetElement(i, j) < 0)
        {
          pos[i] = -pos[i] + 2*origin[i] + extent[2*i+1] * spacing[i]; //flip about the point: origin + (extent*spacing) / 2
        }
      }
    }
  }

  return this->ImageIntensityAt(pos);
}

/***********************************************************************************//**
 *
 */
double vtkmsqImagePlane::ImageIntensityAt(double position[3])
{
  int subId;
  double *tuple;
  double pcoords[3], weights[8];

  vtkPointData *pointData = vtkPointData::New();
  vtkPointData* pd = this->InputImage->GetPointData();
  if (!pd)
    return -1;

  pointData->InterpolateAllocate(pd, 1, 1);

  // Find the cell that contains pos
  vtkCell* cell = this->InputImage->FindAndGetCell(position, NULL, -1, this->GetPickingErrorTolerance(), subId, pcoords,
      weights);

  if (cell)
  {
    // Interpolate the point data
    pointData->InterpolatePoint(pd, 0, cell->PointIds, weights);
    tuple = pointData->GetScalars()->GetTuple(0);
  }

  return tuple[0];
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::BuildPlane()
{
  // Create image data
  this->InputImage = vtkImageData::New();
  this->InputProperties = vtkmsqMedicalImageProperties::New();

  // Create reslice object
  this->ImageReslice = vtkImageReslice::New();
  this->ImageReslice->SetOutputDimensionality(2);

  // Create default lookup table
  this->LookupTable = vtkmsqLookupTable::New();
  this->LookupTable->SetNumberOfTableValues(256);
  this->LookupTable->SetTableRange(0, 256);

  // Create frame polydata
  this->FrameSource = vtkmsqFrameSource::New();
  this->FrameSource->SetCornerSize(0.15);

  // Create mapper
  vtkSmartPointer<vtkPolyDataMapper> frameMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  frameMapper->SetInputConnection(this->FrameSource->GetOutputPort());

  // Create frame actor
  this->FrameActor = vtkActor::New();
  this->FrameActor->SetMapper(frameMapper);
  this->FrameActor->PickableOff();
  this->FrameActor->GetProperty()->SetLineWidth(2.0);
  this->FrameActor->GetProperty()->SetAmbient(1.0);
  this->FrameActor->GetProperty()->SetDiffuse(0.0);
  this->FrameActor->GetProperty()->SetSpecular(0.0);
  this->FrameActor->GetProperty()->SetRepresentationToWireframe();

  // Create image plane
  this->PlaneSource = vtkPlaneSource::New();
  this->PlaneSource->SetXResolution(1);
  this->PlaneSource->SetYResolution(1);

  // Create image plane mapper
  vtkSmartPointer<vtkPolyDataMapper> planeMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  planeMapper->SetInputConnection(this->PlaneSource->GetOutputPort());

  // Create texture
  this->ImageTexture = vtkTexture::New();
  this->ImageTexture->InterpolateOn();
  this->ImageTexture->SetLookupTable(this->LookupTable);
  this->ImageTexture->MapColorScalarsThroughLookupTableOn();

  // Create image actor
  this->ImageActor = vtkActor::New();
  this->ImageActor->GetProperty()->SetInterpolationToFlat();
  this->ImageActor->GetProperty()->SetAmbient(1.0);
  this->ImageActor->GetProperty()->SetDiffuse(0.0);
  this->ImageActor->GetProperty()->SetSpecular(0.0);
  this->ImageActor->SetMapper(planeMapper);
  this->ImageActor->SetTexture(this->ImageTexture);

  this->SetOpacity(1.0);

  // Create a picker for the image actor
  this->ImagePicker = vtkPropPicker::New();
  this->ImagePicker->PickFromListOn();
  this->ImagePicker->AddPickList(this->ImageActor);

  // Add the image plane to this assembly
  this->AddPart(this->ImageActor);
  this->AddPart(this->FrameActor);
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::SetOpacity(double value)
{
  this->ImageActor->GetProperty()->SetOpacity(value);
  this->FrameActor->GetProperty()->SetOpacity(0.4 * value);
}

/***********************************************************************************//**
 *
 */
void vtkmsqImagePlane::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

/***********************************************************************************//**
 *
 */
vtkSmartPointer<vtkMatrix4x4> vtkmsqImagePlane::FindRotationFromOrientation(int orientation)
{
  static double sagittalRotation[16] = { 1, 0, 0, 0,
                                         0, 1, 0, 0,
                                         0, 0, 1, 0,
                                         0, 0, 0, 1 };
  static double coronalRotation[16] = { 1, 0, 0, 0,
                                        0, 1, 0, 0,
                                        0, 0, 1, 0,
                                        0, 0, 0, 1 };

  vtkSmartPointer<vtkMatrix4x4> rotationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

  switch (orientation)
  {
    case vtkMedicalImageProperties::AXIAL:
      break;
    case vtkMedicalImageProperties::SAGITTAL:
      rotationMatrix->DeepCopy(sagittalRotation);
      break;
    case vtkMedicalImageProperties::CORONAL:
      rotationMatrix->DeepCopy(coronalRotation);
      break;
    default:
      // TODO print error
      break;
  }

  return rotationMatrix;
}


/***********************************************************************************//**
 *  This function is called by SetSliceNumber 6 times for each slice update.
 *  3 times for the 3d widget and once for each orthogonal widget.
 *
 *  Update values of plane source and frame source. The plane coordinates indicate
 *  the position of the visualization plane in the 3d widget. The frame coordinates indicate
 *  the position of the colored brackets in the 3d widget.
 *
 *  If the desired position of the colored brackets coincides with the corners
 *  of the planes, PlaneSource and ImageSource must be the same.
 *
 *  These variables have nothing to do with the reslicing of the volume data.
 *  The reslice is given by ResliceMatrix only.
 *
 *  IMPORTANT:
 *
 *  The updated values of PlaneSource and FrameSource are:
 *  Origin: lower left corner of the plane
 *  Point1: upper left corner of the plane
 *  Point2: lower right corner of the plane
 *
 *  These points are found from the resliceMatrix and InputImage->Origin.
 *
 *  The last column of the resliceMatrix give the center of the plane, imageCenter.
 *
 *  ImageHalf is the ABSOLUTE(modulus) dimensions of the plane divided by 2 and is found
 *  from the difference between imageCenter and InputImage->Origin.
 *  Notice that here, the origin refers to the 3d input data.
 *  PlaneSource->Origin and FrameSource->Origin are a different thing.
 *
 *  The Origin of the planes is imageCenter - c0 * imageHalf - c1 * imageHalf
 *  where c0 is the first column of resliceMatrix and c1 is the second.
 *
 *  This is so because c0 indicates the direction of the abscissa of the plane,
 *  and c1 indicates the direction of the ordinate.
 *
 *  Point1 is the Origin + 2 * c0 * imageHalf and
 *  Point2 is the Origin + 2 * c1 * imageHalf
 *  for the same reason.
 *
 */
void vtkmsqImagePlane::UpdateCoords(vtkSmartPointer<vtkMatrix4x4> resliceMatrix)
{
  double planeOrigin[4], point1[4], point2[4];
  double imageOrigin[3];
  this->InputImage->GetOrigin(imageOrigin);

  if (MSQ_REORIENT){
    this->InputProperties->GetReorientedDouble3(imageOrigin, imageOrigin, 0);
  }


  double c0[] = {resliceMatrix->GetElement(0, 0),
	             resliceMatrix->GetElement(1, 0),
	             resliceMatrix->GetElement(2, 0),
	             resliceMatrix->GetElement(3, 0)
                };
  double c1[] = {resliceMatrix->GetElement(0, 1),
                 resliceMatrix->GetElement(1, 1),
                 resliceMatrix->GetElement(2, 1),
                 resliceMatrix->GetElement(3, 1),
                };
  double c3[] = {resliceMatrix->GetElement(0, 3),
                 resliceMatrix->GetElement(1, 3),
                 resliceMatrix->GetElement(2, 3),
                 resliceMatrix->GetElement(3, 3),
                };

  double imageCenter[3];
  double imageHalf[3];
  for(int i = 0; i < 3; i++){
	imageCenter[i] = c3[i];
    imageHalf[i] =  (c0[i] + c1[i]) * (imageCenter[i] - imageOrigin[i]);
    if (imageHalf[i] < 0)
    {
      imageHalf[i] = -imageHalf[i];
    }
  }

  for (int i = 0; i < 3; i++){
    planeOrigin[i] = imageCenter[i] - c0[i] * imageHalf[i] - c1[i] * imageHalf[i];
    point1[i] = planeOrigin[i] + 2 * c0[i] * imageHalf[i];
    point2[i] = planeOrigin[i] + 2 * c1[i] * imageHalf[i];
  }

  // update frame coords
  this->FrameSource->SetOrigin(planeOrigin);
  this->FrameSource->SetPoint1(point1);
  this->FrameSource->SetPoint2(point2);

  // update plane coords
  this->PlaneSource->SetOrigin(planeOrigin);
  this->PlaneSource->SetPoint1(point1);
  this->PlaneSource->SetPoint2(point2);

}

/***********************************************************************************//**
 *
 */
double* vtkmsqImagePlane::GetOrigin()
{
  return this->FrameSource->GetOrigin();
}

/***********************************************************************************//**
 *
 */
double* vtkmsqImagePlane::GetPoint1()
{
  return this->FrameSource->GetPoint1();
}

/***********************************************************************************//**
 *
 */
double* vtkmsqImagePlane::GetPoint2()
{
  return this->FrameSource->GetPoint2();
}
