/*=========================================================================

 Program:   Visualization Toolkit
 Module:    $RCSfile: vtkmsqIOWin32Header.h,v $

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
// .NAME vtkmsqIOWin32Header - manage Windows system differences
// .SECTION Description
// The vtkmsqIOWin32Header captures some system differences between Unix
// and Windows operating systems. 
#ifndef __vtkmsqIOWin32Header_h
#define __vtkmsqIOWin32Header_h

#include <vtkmsqConfigure.h>

#if defined(WIN32) && !defined(VTKMSQ_STATIC)
#if defined(vtkmsqIO_EXPORTS)
#define VTK_MSQ_IO_EXPORT __declspec( dllexport ) 
#else
#define VTK_MSQ_IO_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_MSQ_IO_EXPORT
#endif

#endif
