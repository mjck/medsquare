/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqRectangleActor2D.cxx

 Copyright (c) Marcel Parolin Jackowski, Choukri Mekkaoui
 All rights reserved.

 See http://www.medsquare.org for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above link for more information.

 =========================================================================*/
#include "vtkmsqRectangleActor2D.h"

#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkViewport.h"
#include "vtkWindow.h"
#include "vtkMath.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsqRectangleActor2D);
/** \endcond */

/***********************************************************************************//**
 * Instantiate this object.
 */
vtkmsqRectangleActor2D::vtkmsqRectangleActor2D()
{
  this->PositionCoordinate->SetCoordinateSystemToNormalizedViewport();
  this->PositionCoordinate->SetValue(0.0, 0.0);

  this->Position2Coordinate->SetCoordinateSystemToNormalizedViewport();
  this->Position2Coordinate->SetValue(1.0, 1.0);
  this->Position2Coordinate->SetReferenceCoordinate(NULL);

  this->Rectangle = vtkPolyData::New();
  this->RectangleMapper = vtkPolyDataMapper2D::New();
  this->RectangleMapper->SetInput(this->Rectangle);
  this->RectangleActor = vtkActor2D::New();
  this->RectangleActor->SetMapper(this->RectangleMapper);
}

/***********************************************************************************//**
 * 
 */
vtkmsqRectangleActor2D::~vtkmsqRectangleActor2D()
{
  this->Rectangle->Delete();
  this->RectangleMapper->Delete();
  this->RectangleActor->Delete();
}

/***********************************************************************************//**
 * Build the rectangle and render.
 */
int vtkmsqRectangleActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  this->BuildRectangle(viewport);
  return this->RectangleActor->RenderOpaqueGeometry(viewport);
}

/***********************************************************************************//**
 * Render the axis, ticks, title, and labels.
 */
int vtkmsqRectangleActor2D::RenderOverlay(vtkViewport *viewport)
{
  return this->RectangleActor->RenderOverlay(viewport);
}

/***********************************************************************************//**
 * Does this prop have some translucent polygonal geometry?
 */
int vtkmsqRectangleActor2D::HasTranslucentPolygonalGeometry()
{
  return 0;
}

/***********************************************************************************//**
 * Release any graphics resources that are being consumed by this actor.
 * The parameter window could be used to determine which graphic
 * resources to release.
 */
void vtkmsqRectangleActor2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->RectangleActor->ReleaseGraphicsResources(win);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqRectangleActor2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqRectangleActor2D::BuildRectangle(vtkViewport *viewport)
{
  int *x;
  vtkIdType ptIds[5];
  double p1[3], p2[3], pt[3];

  if ( ! viewport->GetVTKWindow() ||
       (viewport->GetMTime() < this->BuildTime &&
        viewport->GetVTKWindow()->GetMTime() < this->BuildTime &&
        this->GetMTime() < this->BuildTime ) )
    {
    return;
    }

  vtkDebugMacro(<<"Rebuilding rectangle");

  // Initialize and get important info
  this->Rectangle->Initialize();
  this->RectangleActor->SetProperty(this->GetProperty());

  // We'll do our computation in viewport coordinates. First determine the
  // location of the endpoints.
  x = this->PositionCoordinate->GetComputedViewportValue(viewport);
  p1[0] = x[0];
  p1[1] = x[1];
  p1[2] = 0.0;

  x = this->Position2Coordinate->GetComputedViewportValue(viewport);
  p2[0] = x[0];
  p2[1] = x[1];
  p2[2] = 0.0;

  vtkPoints *pts = vtkPoints::New();
  vtkCellArray *lines = vtkCellArray::New();
  this->Rectangle->SetPoints(pts);
  this->Rectangle->SetLines(lines);
  pts->Delete();
  lines->Delete();

  // Generate rectangle
  ptIds[0] = pts->InsertNextPoint(p1);
  
  pt[0] = p2[0];
  pt[1] = p1[1];
  pt[2] = 0.0;

  ptIds[1] = pts->InsertNextPoint(pt);
  ptIds[2] = pts->InsertNextPoint(p2);

  pt[0] = p1[0];
  pt[1] = p2[1];
  pt[2] = 0.0;

  ptIds[3] = pts->InsertNextPoint(pt);
  ptIds[4] = pts->InsertNextPoint(p1);

  lines->InsertNextCell(5, ptIds);

  this->BuildTime.Modified();
}


