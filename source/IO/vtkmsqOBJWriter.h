// .NAME vtkmsqOBJWriter - write a Wavefront OBJ file.
// .SECTION Description

#ifndef __vtkmsqOBJWriter_h
#define __vtkmsqOBJWriter_h

#include "vtkPolyDataAlgorithm.h" //superclass

class vtkmsqOBJWriter : public vtkPolyDataAlgorithm
{
public:
  static vtkmsqOBJWriter *New();
  vtkTypeRevisionMacro(vtkmsqOBJWriter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the name of the file to write out.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

protected:
  vtkmsqOBJWriter();
  ~vtkmsqOBJWriter();

private:
  vtkmsqOBJWriter(const vtkmsqOBJWriter&);  // Not implemented.
  void operator=(const vtkmsqOBJWriter&);  // Not implemented.

  char *FileName;

};

#endif

