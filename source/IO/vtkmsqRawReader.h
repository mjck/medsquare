#ifndef __vtkmsqRawReader_h
#define __vtkmsqRawReader_h

#include "vtkMedicalImageReader2.h"
#include "vtkmsqMedicalImageProperties.h"
#include "vtkmsqIOWin32Header.h"
#include "vtkmsqRawHeader.h"

class VTK_MSQ_IO_EXPORT vtkmsqRawReader: public vtkMedicalImageReader2
{
public:
  static vtkmsqRawReader *New();
  vtkTypeMacro(vtkmsqRawReader, vtkMedicalImageReader2)
  ;
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  // Description:
  // Is the given file an Raw file with right information?
  int CanReadFile(const char* fname);

  // Description:
  // Get/Set property object
  vtkGetObjectMacro(MedicalImageProperties,vtkmsqMedicalImageProperties);
  vtkSetObjectMacro(MedicalImageProperties,vtkmsqMedicalImageProperties);

  // Description:
  // Set image orientation
  void SetOrientation(int orientation);

  // Description:
  // Valid extensions
  virtual const char* GetFileExtensions()
  {
    return ".raw .raw.gz .RAW .RAW.gz" ;
  }

  // Description: 
  // A descriptive name for this format
  virtual const char* GetDescriptiveName()
  {
    return "Raw";
  }

protected:
  // Description:
  // Medical Image properties
  vtkmsqMedicalImageProperties *MedicalImageProperties;

  vtkmsqRawReader();
  ~vtkmsqRawReader();

  //virtual void ExecuteData(vtkDataObject *, vtkInformation *outInfo);
  virtual int RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:
  vtkmsqRawReader(const vtkmsqRawReader&) VTK_DELETE_FUNCTION; // Not implemented.
  void operator=(const vtkmsqRawReader&) VTK_DELETE_FUNCTION; // Not implemented.
};
#endif
