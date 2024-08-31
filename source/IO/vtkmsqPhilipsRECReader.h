//
// .NAME vtkmsqPhilipsRECReader - read Philips PAR/REC image files
// .SECTION Description
// vtkmsqPhilipsRECReader is a source object that reads Philips PAR/REC image files.
// The floating point value and display value of the data read in may be 
// calculated using the following formulas taken from the PAR file:
//
// # === PIXEL VALUES =============================================================
// #  PV = pixel value in REC file, FP = floating point value, DV = displayed value on console
// #  RS = rescale slope,           RI = rescale intercept,    SS = scale slope
// #  DV = PV * RS + RI             FP = DV / (RS * SS)

//
// .SECTION See Also
// vtkImageReader2

#ifndef __vtkmsqPhilipsRECReader_h
#define __vtkmsqPhilipsRECReader_h

#include "vtkMedicalImageReader2.h"
#include "vtkmsqIOWin32Header.h"
#include "vtkmsqMedicalImageProperties.h"
#include "vtkmsqPhilipsPAR.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

class VTK_MSQ_IO_EXPORT vtkmsqPhilipsRECReader: public vtkMedicalImageReader2
{
public:
  static vtkmsqPhilipsRECReader *New();vtkTypeMacro(vtkmsqPhilipsRECReader,vtkMedicalImageReader2)
  ;
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  //BTX
  typedef std::vector<int> SliceIndexType;
  //ETX

  // Description: is the given file name a REC/PAR file?
  virtual int CanReadFile(const char* fname);

  // Description:
  // Valid extentsions
  virtual const char* GetFileExtensions()
  {
    return ".PAR .REC .par .rec";
  }

  // Description:
  // A descriptive name for this format
  virtual const char* GetDescriptiveName()
  {
    return "Philips PAR/REC image";
  }

  // Description:
  // Get/Set property object
  vtkSetObjectMacro(MedicalImageProperties, vtkmsqMedicalImageProperties);
  vtkGetObjectMacro(MedicalImageProperties, vtkmsqMedicalImageProperties);

  // Description:
  // Returns the slice index based on the desired arrangement
  int GetSliceIndex(int index);

protected:

  vtkmsqMedicalImageProperties *MedicalImageProperties;

  vtkmsqPhilipsRECReader();
  ~vtkmsqPhilipsRECReader();

  virtual int RequestInformation(vtkInformation* request,
      vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  //virtual void ExecuteData(vtkDataObject *out, vtkInformation *outInfo);
  virtual int RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  SliceIndexType *SliceIndex;

private:

  //BTX
  int GetImageTypeOffset(int imageType, int scanSequence, int volumeIndex, int slice,
      int numSlices, struct msqpar_parameter parParam,
      std::vector<std::pair<int, int> > sliceImageTypesIndex,
      std::vector<std::pair<int, int> > sliceScanSequenceIndex);

  void SetupSliceIndex(vtkmsqPhilipsRECReader::SliceIndexType *indexMatrix, int sortBlock,
      struct msqpar_parameter parParam,
      std::vector<std::pair<int, int> > imageTypesScanSequenceIndex,
      std::vector<std::pair<int, int> > sliceImageTypesIndex,
      std::vector<std::pair<int, int> > sliceScanSequenceIndex);
  //ETX
  vtkmsqPhilipsRECReader(const vtkmsqPhilipsRECReader&); // Not implemented.
  void operator=(const vtkmsqPhilipsRECReader&); // Not implemented.
};
#endif
