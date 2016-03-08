
#ifndef __vtkmsqImageAverage_h
#define __vtkmsqImageAverage_h

#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

#include "vtkImageAlgorithm.h"

class vtkmsqImageAverage : public vtkImageAlgorithm
{
 public:
  static vtkmsqImageAverage *New();
  vtkTypeMacro(vtkmsqImageAverage,vtkImageAlgorithm);
  
  // Multiple Input Stuff
  // --------------------
  // Description:
  // Set an Input of this filter. 
  virtual void SetInput(int idx, vtkDataObject *input);
  
  // Description:
  // Adds an input to the first null position in the input list.
  // Expands the list memory if necessary
  //virtual void AddInput(vtkImageData *input);
  
  // Description:
  // Get one input to this filter
  vtkImageData *GetInput(int num);
  vtkImageData *GetInput();
  
protected:
  vtkmsqImageAverage();
  ~vtkmsqImageAverage() {};

  // These are called by the superclass.
  //virtual int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int FillInputPortInformation( int port, vtkInformation* info );

  void SimpleExecute(std::vector<vtkImageData *> inputs, vtkImageData *output);

 private:
  vtkmsqImageAverage(const vtkmsqImageAverage&); // Not implemented
  void operator=(const vtkmsqImageAverage&); // Not implemented
};

#endif


