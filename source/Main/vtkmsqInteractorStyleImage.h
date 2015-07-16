//
// .NAME vtkmsqInteractorStyleImage - implement an image intearctor

#ifndef VTKMSQ_INTERACTOR_STYLE_IMAGE_H
#define VTKMSQ_INTERACTOR_STYLE_IMAGE_H

#include "vtkInteractorStyleTrackballCamera.h"

class MSQOrthogonalViewer;

#define VTKIS_WINDOW_LEVEL 1025

class vtkmsqInteractorStyleImage: public vtkInteractorStyleTrackballCamera
{
public:
  static vtkmsqInteractorStyleImage *New();
  vtkTypeMacro(vtkmsqInteractorStyleImage, vtkInteractorStyleTrackballCamera);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetOrthogonalViewer(MSQOrthogonalViewer *viewer);
  MSQOrthogonalViewer *GetOrthogonalViewer();

  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMouseMove();

  virtual void StartWindowLevel();
  virtual void EndWindowLevel();
  virtual void ResetWindowLevel();

protected:
  vtkmsqInteractorStyleImage();
  ~vtkmsqInteractorStyleImage();

  int WindowLevelStartPosition[2];
  double InitialWindow;
  double InitialLevel;

private:

  MSQOrthogonalViewer *OrthoViewer;

  void toggleMaximizeViewport(vtkRenderer *pokedRenderer);
  void updateWindowLevel(vtkRenderer *pokedRenderer, int x, int y);

  vtkmsqInteractorStyleImage(const vtkmsqInteractorStyleImage&); // Not implemented.
  void operator=(const vtkmsqInteractorStyleImage&); // Not implemented.
};

#endif
