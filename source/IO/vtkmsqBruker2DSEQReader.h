#ifndef __vtkmsqBruker2DSEQReader_h
#define __vtkmsqBruker2DSEQReader_h

#include "vtkMedicalImageReader2.h"
#include "vtkmsqIOWin32Header.h"
#include "vtkmsqMedicalImageProperties.h"

//
// This reader requires the following files be present:
// 2dseq, acqp, d3proc, reco
//
class VTK_MSQ_IO_EXPORT vtkmsqBruker2DSEQReader: public vtkMedicalImageReader2
{
public:
  static vtkmsqBruker2DSEQReader *New();vtkTypeRevisionMacro(vtkmsqBruker2DSEQReader,vtkMedicalImageReader2)
  ;
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Is the given file an Analyze file?
  virtual int CanReadFile(const char* fname);

  // Description:
  // Turn on/off automatic byte swapping
  // By default it is on, allowing the reader to determine word
  // endianess from hints in the Analyze header
  vtkGetMacro(AutoByteSwapping, int)
  ;vtkSetMacro(AutoByteSwapping, int)
  ;vtkBooleanMacro(AutoByteSwapping, int)
  ;

  // Description:
  // Get/Set property object
  vtkGetObjectMacro(MedicalImageProperties,vtkmsqMedicalImageProperties)
  ;vtkSetObjectMacro(MedicalImageProperties,vtkmsqMedicalImageProperties)
  ;

  // Description:
  // Valid extensions
  virtual const char* GetFileExtensions()
  {
    return ".2dseq";
  }

  // Description: 
  // A descriptive name for this format
  virtual const char* GetDescriptiveName()
  {
    return "Bruker 2DSEQ";
  }

protected:
  vtkmsqMedicalImageProperties *MedicalImageProperties;
  int AutoByteSwapping; // automatic byte swapping based on header hints

  vtkmsqBruker2DSEQReader();
  ~vtkmsqBruker2DSEQReader();

  virtual int RequestInformation(vtkInformation* request,
      vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  virtual void ExecuteData(vtkDataObject *);

private:
  vtkmsqBruker2DSEQReader(const vtkmsqBruker2DSEQReader&); // Not implemented.
  void operator=(const vtkmsqBruker2DSEQReader&); // Not implemented.
};
#endif

