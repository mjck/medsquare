/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqRawHeader.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqRawHeader.h"

/** \cond 0 */
//vtkCxxRevisionMacro(vtkmsqRawHeader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkmsqRawHeader);
/** \endcond */

/***********************************************************************************//**
 * 
 */
vtkmsqRawHeader::vtkmsqRawHeader()
{
  this->Dimensions[0] = 1;
  this->Dimensions[1] = 1;
  this->Dimensions[2] = 1;
  this->Spacing[0] = 1;
  this->Spacing[1] = 1;
  this->Spacing[2] = 1;
  this->Origin[0] = 0;
  this->Origin[1] = 0;
  this->Origin[2] = 0;
  this->LittleEndian = true;
  this->Volume = 1;
  this->Orientation = RAWAXIAL;
  this->Type = SHORT;
  this->Offset = 0;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqRawHeader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "dimensions: " << this->Dimensions[0] << ", " << this->Dimensions[1]
      << ", " << this->Dimensions[2] << "\n";
  os << indent << "spacing:  " << this->Spacing[0] << ", " << this->Spacing[1] << ", "
      << this->Spacing[2] << "\n";
  os << indent << "origin:  " << this->Origin[0] << ", " << this->Origin[1] << ", "
      << this->Origin[2] << "\n";
  os << indent << "is little endian?: " << this->LittleEndian << "\n";
  os << indent << "volume: " << this->Volume << "\n";
  os << indent << "orientation: " << this->Orientation << "\n";
  os << indent << "data type: " << this->Type << "\n";
  os << indent << "offset: " << this->Offset << "\n";
}

