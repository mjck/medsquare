//
// .NAME vtkmsqInteractorStyleImage3D - implement a 3D image interactor 

#ifndef VTKMSQ_INTERACTOR_STYLE_IMAGE3D_H
#define VTKMSQ_INTERACTOR_STYLE_IMAGE3D_H

#include "vtkmsqInteractorStyleImage.h"

class vtkmsqInteractorStyleImage3D: public vtkmsqInteractorStyleImage
{
public:
  static vtkmsqInteractorStyleImage3D *New();vtkTypeMacro(vtkmsqInteractorStyleImage3D, vtkmsqInteractorStyleImage)
  ;
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();

protected:
  vtkmsqInteractorStyleImage3D();
  ~vtkmsqInteractorStyleImage3D();

private:

  vtkmsqInteractorStyleImage3D(const vtkmsqInteractorStyleImage3D&); // Not implemented.
  void operator=(const vtkmsqInteractorStyleImage3D&); // Not implemented.
};

#endif
