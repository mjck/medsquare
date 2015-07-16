/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqGraphicsWin32Header.h

 Copyright (c) Marcel Parolin Jackowski
 All rights reserved.

 See http://www.medsquare.org for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above link for more information.

 =========================================================================*/
// .NAME vtkmsqGraphicsWin32Header - manage Windows system differences
// .SECT Description
// The vtkmsqGraphicsWin32Header captures some system differences between Unix
// and Windows operating systems. 
#ifndef __vtkmsqGraphicsWin32Header_h
#define __vtkmsqGraphicsWin32Header_h

#include <vtkmsqConfigure.h>

#if defined(WIN32) && !defined(VTKMSQ_STATIC)
#if defined(vtkmsqGraphics_EXPORTS)
#define VTK_MSQ_GRAPHICS_EXPORT __declspec( dllexport ) 
#else
#define VTK_MSQ_GRAPHICS_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_MSQ_GRAPHICS_EXPORT
#endif

#endif
