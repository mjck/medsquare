// .NAME vtkmsqMedicalImageProperties - some medical image properties.
// .SECTION Description
// vtkmsqMedicalImageProperties is a helper class that can be used by medical
// image readers and applications to encapsulate medical image/acquisition
// properties.

#ifndef __vtkmsqMedicalImageProperties_h
#define __vtkmsqMedicalImageProperties_h

#include "vtkObject.h"
#include "vtkImageData.h"
#include "vtkmsqImagingWin32Header.h"

class VTK_MSQ_IMAGING_EXPORT vtkmsqImageData: public vtkImageData
{
public:
  static vtkmsqImageData *New();
  vtkTypeRevisionMacro(vtkmsqImageData,vtkImageData);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set orientation type
  vtkGetMacro(OrientationType, int);
  vtkSetMacro(OrientationType);

protected:
  vtkmsqImageData();
  ~vtkmsqImageData();

  int OrientationType;

private:
  vtkmsqMedicalImageProperties(const vtkmsqMedicalImageProperties&); // Not implemented.
  void operator=(const vtkmsqMedicalImageProperties&); // Not implemented.
};

#endif
