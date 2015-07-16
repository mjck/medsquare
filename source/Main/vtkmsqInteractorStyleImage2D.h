//
// .NAME vtkmsqInteractorStyleImage2D - implement a 2D image interactor 

#ifndef VTKMSQ_INTERACTOR_STYLE_IMAGE2D_H
#define VTKMSQ_INTERACTOR_STYLE_IMAGE2D_H

#define VTKIS_CLICKED_VOXEL 1024

#include "vtkmsqInteractorStyleImage.h"

class vtkmsqInteractorStyleImage2D: public vtkmsqInteractorStyleImage
{
public:
  static vtkmsqInteractorStyleImage2D *New();
  vtkTypeMacro(vtkmsqInteractorStyleImage2D, vtkmsqInteractorStyleImage);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();
  virtual void OnMouseMove();

  virtual void StartClickedVoxel();
  virtual void EndClickedVoxel();

protected:
  vtkmsqInteractorStyleImage2D();
  ~vtkmsqInteractorStyleImage2D();

private:
  vtkmsqInteractorStyleImage2D(const vtkmsqInteractorStyleImage2D&); // Not implemented.
  void operator=(const vtkmsqInteractorStyleImage2D&); // Not implemented.
};

#endif
