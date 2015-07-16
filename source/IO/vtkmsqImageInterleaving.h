
#ifndef __vtkmsqImageInterleaving_h
#define __vtkmsqImageInterleaving_h

//#include "vtkSimpleImageToImageFilter.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

#include "vtkSimpleImageToImageFilter.h"

class vtkmsqImageInterleaving : public vtkSimpleImageToImageFilter
{
public:
  static vtkmsqImageInterleaving *New();
  vtkTypeMacro(vtkmsqImageInterleaving,vtkSimpleImageToImageFilter);

  // Set number of frames in case of frame interleaving
  vtkSetMacro(NumberOfFrames, int);
  vtkGetMacro(NumberOfFrames, int);

protected:
  vtkmsqImageInterleaving();
  ~vtkmsqImageInterleaving() {}

  int Interleaving;
  int NumberOfFrames;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual void SimpleExecute(vtkImageData* in, vtkImageData* out);

private:
  vtkmsqImageInterleaving(const vtkmsqImageInterleaving&);// {};
  void operator=(const vtkmsqImageInterleaving&);// {};
};

#endif
