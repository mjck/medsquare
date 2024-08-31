//
// .NAME vtkmsqAnalyzeWriter
// .SECTION Description
// vtkmsqAnalyzeWriter writes out Analyze images
// .SECTION See Also
// vtkImageWriter

#ifndef __vtkmsqAnalyzeWriter_h
#define __vtkmsqAnalyzeWriter_h

#include "vtkImageWriter.h"
#include "vtkMedicalImageProperties.h"
#include "vtkmsqIOWin32Header.h"
#include "vtkmsqAnalyzeHeader.h"

class VTK_MSQ_IO_EXPORT vtkmsqAnalyzeWriter: public vtkImageWriter
{
public:
vtkTypeMacro(vtkmsqAnalyzeWriter,vtkImageWriter)
  ;
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with default parameters
  static vtkmsqAnalyzeWriter *New();

  // Description:
  // Get/Set compression option 
  vtkGetMacro(Compression, int)
  ;vtkSetMacro(Compression, int)
  ;vtkBooleanMacro(Compression, int)
  ;

  // Description:
  // Get/Set property object
  vtkSetObjectMacro(MedicalImageProperties,vtkMedicalImageProperties)
  ;

  // Description:
  // Can we create the Analyze image on the file system ?
  virtual int CanWriteFile(const char * FileNameToWrite);

  // This is called by the superclass.
  // This is the method you should override.
  virtual void Write();

protected:
  vtkmsqAnalyzeWriter();
  ~vtkmsqAnalyzeWriter()
  {
  }
  ;

  //BTX
  struct analyze_dsr header; // Analyze header
  //ETX

  int Compression; // zlib compression on/off
  vtkMedicalImageProperties *MedicalImageProperties;

private:
  vtkmsqAnalyzeWriter(const vtkmsqAnalyzeWriter&); // Not implemented.
  void operator=(const vtkmsqAnalyzeWriter&); // Not implemented.

  //BTX
  void InitializeHeader(struct analyze_dsr *hdr);
  //ETX

  int WriteHeader(const char *fileName); // Write .hdr
  int WriteImage(const char *fileName); // Write .img or .img.gz

};

#endif

