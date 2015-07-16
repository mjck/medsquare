// .NAME vtkmsqLookupTable - map scalar values into colors via a lookup table
// .SECTION Description
// vtkLookupTable is an object that is used by mapper objects to map scalar 
// values into rgba (red-green-blue-alpha transparency) color specification, 
// or rgba into scalar values. The color table can be created by direct 
// insertion of color values, or by specifying  hue, saturation, value, and 
// alpha range and generating a table.
//
// .SECTION Caveats
// You need to explicitly call Build() when constructing the LUT by hand.
//
// .SECTION See Also
// vtkLogLookupTable vtkWindowLevelLookupTable

#ifndef __vtkmsqLookupTable_h
#define __vtkmsqLookupTable_h

#include "vtkmsqImagingWin32Header.h"
#include "vtkLookupTable.h"
#include "vtkColorTransferFunction.h"

#include <string>

class VTK_MSQ_IMAGING_EXPORT vtkmsqLookupTable: public vtkLookupTable
{
public:
  // Description:
  // Construct with range=[0,1]; and hsv ranges set up for rainbow color table 
  // (from red to blue).
  static vtkmsqLookupTable *New();

  vtkTypeMacro(vtkmsqLookupTable,vtkLookupTable)
  ;
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates a black-red-yellow-white color map. 
  // Use SetNumberOfTableValues to allocate array prior to calling this method
  void SetColormapToHot();
  void SetColormapToStep();
  vtkColorTransferFunction* GetColorTransferFunctionHot();
  void DeepCopy(vtkmsqLookupTable* newTable);

  void SetOpacity(double value);
  void SetName(std::string newName);
  std::string GetName();

  void SetWindow(double value);
  void SetLevel(double value);

protected:
  vtkmsqLookupTable(int sze = 256, int ext = 256);
  ~vtkmsqLookupTable();

private:
  vtkmsqLookupTable(const vtkmsqLookupTable&); // Not implemented.
  void operator=(const vtkmsqLookupTable&); // Not implemented.

  std::string Name;
  double Window, Level;

  void UpdateWindowLevel();
};

#endif

