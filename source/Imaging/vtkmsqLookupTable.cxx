/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqLookupTable.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqLookupTable.h"

#include "vtkObjectFactory.h"

/** \cond 0 */
vtkStandardNewMacro(vtkmsqLookupTable);
/** \endcond */

/***********************************************************************************//**
 * Construct with range=(0,1); and hsv ranges set up for rainbow color table
 * (from red to blue).
 * 
 */
vtkmsqLookupTable::vtkmsqLookupTable(int sze, int ext) :
    vtkLookupTable(sze, ext)
{
	this->Name = "";
}

/***********************************************************************************//**
 *
 */
vtkmsqLookupTable::~vtkmsqLookupTable()
{
}

/***********************************************************************************//**
 *
 */
vtkColorTransferFunction* vtkmsqLookupTable::GetColorTransferFunctionHot()
{
  int i;

  vtkColorTransferFunction* colorTransfer=vtkColorTransferFunction::New();
  int m = this->GetNumberOfTableValues(); // get table size
  int n = (int) (3.0 / 8 * m);
  double rgb[m][3];
  int indx = 0;
  for (i = 1; i <= n; i++)
  {
    rgb[indx][0]=i / (double) n;//) * 255.0f + 0.5f; // (1:n)'/n
    
    indx++;
  }
  for (i = 1; i <= m - n; i++)
  {
    rgb[indx][0] = 1.0; // ones(m-n,1)
    indx++;
  }

  indx = 0;
  for (i = 1; i <= n; i++)
  {
    rgb[indx][1] = 0.0; // zeros(n,1)
    indx++;
  }
  for (i = 1; i <= n; i++)
  {
    rgb[indx][1] = i / (double) n;//) * 255.0f + 0.5f); // (1:n)'/n
    indx++;
  }
  for (i = 1; i <= m - 2 * n; i++)
  {
    rgb[indx][1] = 1.0;//255; // ones(m-2*n,1)
    indx++;
  }

  indx = 0;
  for (i = 1; i <= 2 * n; i++)
  {
    rgb[indx][2] = 0.0; // zeros(2*n,1)
    indx++;
  }
  for (i = 1; i <= m - 2 * n; i++)
  {
    rgb[indx][2] = i / (double) (m - 2 * n); // (1:m-2*n)'/(m-2*n)
    indx++;
  }
  colorTransfer->BuildFunctionFromTable(this->TableRange[0],this->TableRange[1],m,&rgb[0][0]);
  return colorTransfer;
}

/***********************************************************************************//**
 *
 */
void vtkmsqLookupTable::SetColormapToHot()
{
  int i;
  unsigned char *rgba;
  int m = this->GetNumberOfTableValues(); // get table size
  int n = (int) (3.0 / 8 * m);

  int indx = 0;
  for (i = 1; i <= n; i++)
  {
    rgba = this->Table->WritePointer(4 * indx, 4);
    rgba[0] = static_cast<unsigned char>((i / (double) n) * 255.0f + 0.5f); // (1:n)'/n
    rgba[3] = 255; // opaque
    indx++;
  }
  for (i = 1; i <= m - n; i++)
  {
    rgba = this->Table->WritePointer(4 * indx, 4);
    rgba[0] = 255; // ones(m-n,1)
    rgba[3] = 255; // opaque
    indx++;
  }

  indx = 0;
  for (i = 1; i <= n; i++)
  {
    rgba = this->Table->WritePointer(4 * indx, 4);
    rgba[1] = 0; // zeros(n,1)
    indx++;
  }
  for (i = 1; i <= n; i++)
  {
    rgba = this->Table->WritePointer(4 * indx, 4);
    rgba[1] = static_cast<unsigned char>((i / (double) n) * 255.0f + 0.5f); // (1:n)'/n
    indx++;
  }
  for (i = 1; i <= m - 2 * n; i++)
  {
    rgba = this->Table->WritePointer(4 * indx, 4);
    rgba[1] = 255; // ones(m-2*n,1)
    indx++;
  }

  indx = 0;
  for (i = 1; i <= 2 * n; i++)
  {
    rgba = this->Table->WritePointer(4 * indx, 4);
    rgba[2] = 0; // zeros(2*n,1)
    indx++;
  }
  for (i = 1; i <= m - 2 * n; i++)
  {
    rgba = this->Table->WritePointer(4 * indx, 4);
    rgba[2] = static_cast<unsigned char>((i / (double) (m - 2 * n)) * 255.0f + 0.5f); // (1:m-2*n)'/(m-2*n)
    indx++;
  }

}

/***********************************************************************************//**
 *
 */
void vtkmsqLookupTable::SetColormapToStep()
{
  int numColors = this->GetNumberOfTableValues(); // get table size

  float numc = float(numColors - 1);

  printf("number of colors: %d\n", numColors);
  for (int i = 0; i < numColors; i++)
  {
    float v = 0.0;
    v = float(i) / float(numc);

    this->SetTableValue(i, v, 0, 0, 1.0);
  }
}

/***********************************************************************************//**
 *
 */
void vtkmsqLookupTable::SetOpacity(double value)
{
  int colors = this->GetNumberOfTableValues(); // get table size
  for (int i = 0; i < colors; i++)
  {
    double rgba[4];
    this->GetTableValue(i, rgba);
    rgba[3] = value;
    this->SetTableValue(i, rgba);
  }
}


/***********************************************************************************//**
 *
 */
void vtkmsqLookupTable::SetName(std::string newName)
{
  this->Name = newName;
}

/***********************************************************************************//**
 *
 */
std::string vtkmsqLookupTable::GetName()
{
  return this->Name;
}

/***********************************************************************************//**
 *
 * \author: Luiz Fernando Oliveira Corte Real
 */
void vtkmsqLookupTable::SetWindow(double value)
{
  this->Window = value;
  this->UpdateWindowLevel();
}

/***********************************************************************************//**
 *
 * \author: Luiz Fernando Oliveira Corte Real
 */
void vtkmsqLookupTable::SetLevel(double value)
{
  this->Level = value;
  this->UpdateWindowLevel();
}

/***********************************************************************************//**
 *
 * \author: Luiz Fernando Oliveira Corte Real
 */
void vtkmsqLookupTable::UpdateWindowLevel()
{
  this->SetTableRange(this->Level - this->Window / 2,
        this->Level + this->Window / 2);
  this->Modified();
}

/***********************************************************************************//**
 *
 */
void vtkmsqLookupTable::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

/***********************************************************************************//**
 *
 * \author: Daniel Oliveira Dantas
 */
void vtkmsqLookupTable::DeepCopy(vtkmsqLookupTable* newTable)
{
  this->Superclass::DeepCopy(newTable);
  this->SetName(newTable->GetName());
}

