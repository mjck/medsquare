/*=========================================================================

 Program:   MedSquare
 Module:    MedSquare.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqImageItem.h"

#include "MSQColormapFactory.h"
#include "vtkmsqMedicalImageProperties.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsqImageItem);
/** \endcond */

/***********************************************************************************//**
 *
 *
 * \author: Daniel Oliveira Dantas
 */
vtkmsqImageItem::vtkmsqImageItem()
{
  this->Image = NULL;
  this->Properties = NULL;
  this->Colormap = this->defaultColormap();
}

/***********************************************************************************//**
 *
 *
 * \author: Daniel Oliveira Dantas
 */
vtkmsqImageItem::~vtkmsqImageItem()
{
  if (this->Image != NULL)
    this->Image->Delete();

  if (this->Properties != NULL)
    this->Properties->Delete();
}

/***********************************************************************************//**
 *
 *
 * \author: Daniel Oliveira Dantas
 */
vtkmsqLookupTable* vtkmsqImageItem::defaultColormap() {
  vtkmsqLookupTable* colormap = vtkmsqLookupTable::New();

  colormap->SetSaturationRange(0, 0);
  colormap->SetValueRange(0, 1);
  colormap->SetRampToLinear();
  colormap->Build();
  colormap->SetName("Gray");

  return colormap;
}

/***********************************************************************************//**
 *
 */
void vtkmsqImageItem::SetColormap(vtkmsqLookupTable *colormap)
{
  this->Colormap = colormap;
  this->InvokeEvent("ColormapChanged");
  this->Modified();
}

/***********************************************************************************//**
 *  This function is called during the construction of vtkmsqImagePlane.
 *
 *  Returns a translation matrix, that is, a 4x4 matrix where the last column has the
 *  coordinates of the visualization volume.
 *
 *  The visualization volume is defined relatively to InputImage->GetOrigin().
 *
 *  The first dimension of the visualization volume is contained between
 *  (extent[0] * spacing[0]) and (extent[1] * spacing[0])
 *
 *  The second dimension of the visualization volume is contained between
 *  (extent[2] * spacing[0]) and (extent[3] * spacing[0])
 *
 *  The first dimension of the visualization volume is contained between
 *  (extent[4] * spacing[0]) and (extent[5] * spacing[0])
 *
 *  The center is the middle point of these three segments.
 *
 *  \author Daniel Oliveira Dantas
 */
vtkMatrix4x4* vtkmsqImageItem::FindTranslationToCenter(double multiplier) {
  int extent[6];
  double spacing[3], center[3], origin[3];

  this->Image->GetExtent(extent);
  this->Image->GetSpacing(spacing);
  this->Image->GetOrigin(origin);

  if (MSQ_REORIENT) {
    this->Properties->GetReorientedExtent(extent, extent, 0);
    this->Properties->GetReorientedDouble3(spacing, spacing);
    this->Properties->GetReorientedDouble3(origin, origin, 0);
  }

  vtkMatrix4x4* translationMatrix = vtkMatrix4x4::New();

  center[0] = spacing[0] * 0.5 * (extent[1] - extent[0]) + extent[0];
  center[1] = spacing[1] * 0.5 * (extent[3] - extent[2]) + extent[2];
  if (extent[5] == 0) {
    center[2] = 0.0;
  } else {
    center[2] = spacing[2] * 0.5 * (extent[5] - extent[4]) + extent[4];
  }

  translationMatrix->SetElement(0, 3, origin[0] + center[0] * multiplier);
  translationMatrix->SetElement(1, 3, origin[1] + center[1] * multiplier);
  translationMatrix->SetElement(2, 3, origin[2] + center[2] * multiplier);

  return translationMatrix;
}


/***********************************************************************************//**
 *  This function is called when the slice changes.
 *
 *  Returns a 4x4 transformation matrix, where the last column indicates
 *  the center of the plane in the VISUALIZATION volume. The other columns indicate
 *  the orientation of the plane in space. The first line is abscissa, second line
 *  is ordinate and third line is the normal.
 *
 *  This matrix is found as follows:
 *  Let the matrix A be the product of ImageCenteringMatrix by PlaneOrientationMatrix.
 *  Let the matrix B be the identity matrix 4x4 where the third line and fourth column is modified,
 *  receiving the slice value relative to the center (spacing * (slice - 0.5 * extent))
 *
 *  The reslicing matrix is the product of matrices A and B
 *
 */
vtkMatrix4x4* vtkmsqImageItem::FindReslicingMatrix(int slice, vtkMatrix4x4 *planeOrientationMatrix)
{
  int extent[6];
  double spacing[3];
  double origin[3];

  this->Image->GetExtent(extent);
  this->Image->GetSpacing(spacing);
  this->Image->GetOrigin(origin);

  if (MSQ_REORIENT){
    this->Properties->GetReorientedExtent(extent, extent);
    this->Properties->GetReorientedDouble3(spacing, spacing);
    this->Properties->GetReorientedDouble3(origin, origin, 0);
  }

  vtkMatrix4x4* result = vtkMatrix4x4::New();

  vtkMatrix4x4::Multiply4x4(this->FindTranslationToCenter(), planeOrientationMatrix, result);

  vtkSmartPointer<vtkMatrix4x4> resliceAxes = vtkSmartPointer<vtkMatrix4x4>::New();

  double normal[3];

  normal[0] = result->GetElement(0, 2);
  normal[1] = result->GetElement(1, 2);
  normal[2] = result->GetElement(2, 2);

  double extentMinima = extent[0] * normal[0] +
                        extent[2] * normal[1] +
                        extent[4] * normal[2];

  double extentMaxima = extent[1] * normal[0] +
                        extent[3] * normal[1] +
                        extent[5] * normal[2];

  double spacingDouble = spacing[0] * normal[0] +
                         spacing[1] * normal[1] +
                         spacing[2] * normal[2];

  double zExtent;
  if ((extent[5] - extent[4]) != 0)
  {
    zExtent = extentMaxima - extentMinima;
  }
  else
  {
    zExtent = 1.0;
    printf("%s: %s: Error: extent[5] - extent[4]) == 0 causes \"Bad plane coordinate system\"\n", __FILE__, __FUNCTION__);
  }

  resliceAxes->SetElement(2, 3, spacingDouble * (slice - 0.5 * zExtent));

  vtkMatrix4x4::Multiply4x4(result, resliceAxes, result);

  for(int i = 0; i < 3; i++){
    if (result->GetElement(i, 2) < 0)
    {
      result->SetElement(i, 3, origin[i]  + spacing[i] * extent[i] - result->GetElement(i, 3));
    }
  }

  return result;
}

/***********************************************************************************//**
 *  This function is called when the slice changes.
 *
 *  Returns a 4x4 transformation matrix, where the last column indicates
 *  the center of the plane in the INPUT DATA volume. The other columns indicate
 *  the orientation of the plane in space. The first line is abscissa, second line
 *  is ordinate and third line is the normal.
 *
 *  This function is useful when we need to reorient the input data according to
 *  the Direction Cosines.
 *
 *  The matrix is found by multiplying
 *  DirectionCosineMatrixPerpendicular by resliceMatrix
 *
 *
 *
 */
vtkMatrix4x4* vtkmsqImageItem::FindReslicingMatrix2(int slice, vtkMatrix4x4 *planeOrientationMatrix)
{
  vtkMatrix4x4* resliceMatrix = this->FindReslicingMatrix(slice, planeOrientationMatrix);

  if (MSQ_REORIENT){

    int extent[6];
    double spacing[3];
    double origin[3];
    this->Image->GetExtent(extent);
    this->Image->GetSpacing(spacing);
    this->Image->GetOrigin(origin);

    vtkMatrix4x4* buffer = vtkMatrix4x4::New();

    double translation[4];

    vtkSmartPointer<vtkMatrix4x4> dircosMatrix = this->Properties->GetDirectionCosineMatrixPerpendicular();
    vtkSmartPointer<vtkMatrix4x4> translationMatrix = this->FindTranslationToCenter();
    for(int i = 0; i < 4; i++){
      translation[i] = translationMatrix->GetElement(i, 3);
    }

    dircosMatrix->MultiplyPoint(translation, translation);

    // This line solves the problem
    vtkMatrix4x4::Multiply4x4(dircosMatrix, resliceMatrix, buffer);
    resliceMatrix->Delete();

    return buffer;
  }
  else{
    return resliceMatrix;
  }
}

/***********************************************************************************//**
 */
void vtkmsqImageItem::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
