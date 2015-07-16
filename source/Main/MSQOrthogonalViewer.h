/*=========================================================================

 Program:   MedSquare
 Module:    MSQOrthogonalViewer.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_ORTHOGONAL_VIEWER_H
#define MSQ_ORTHOGONAL_VIEWER_H

#include <QtGui>

#include "MSQSliceNavigationWidget.h"
#include "MSQViewer.h"

class vtkActor;
class vtkImageData;
class vtkmsqLookupTable;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkRenderer;
class vtkVolume;

class vtkmsqFrameSource;
class vtkmsqMedicalImageProperties;

class MSQColormapFactory;
class MSQGeometryItem;
class MSQGeometryWidget;
class MSQMipAction;
class MSQProjectionMenu;
class MSQRenderWidget;
class MSQWindowLevelWidget;

class MedSquare;

class MSQOrthogonalViewer: public MSQViewer
{
Q_OBJECT
public:
  MSQOrthogonalViewer(MedSquare *medSquare);
  ~MSQOrthogonalViewer();

  // set input to the viewer
  void setInput(vtkmsqImageItem *newImageItem);

  // verify if there's an image loaded
  bool hasImageLoaded();

  // sets current colormap
  void setCurrentColormap(vtkmsqLookupTable *newColormap);

  // maximize/restore the window of this renderer
  void toggleMaximizeWindow(vtkRenderer *renderer);

  // make this renderer the current view
  void setCurrentRenderer(vtkRenderer *renderer);

  // manipulation of selected slices
  void updateSlices(vtkRenderer *pokedRenderer, int x, int y);
  void incrementSlice(int increment, vtkRenderer *renderer);

  // manipulation of window/level
  void setWindow(double value);
  void setLevel(double value);
  double getWindow();
  double getLevel();

  // viewer options
  void zoomIn();
  void zoomOut();
  void reset();

  void refresh();

  MSQOrientationType getOrientationType(vtkRenderer *renderer);

signals:
  void colormapChanged(int);

public slots:
  void addGeometry(MSQGeometryItem *item);
  void removeGeometry(MSQGeometryItem *item);
  void updateGeometry(MSQGeometryItem *item);

private slots:
  void setCurrentSliceAxial(int slice);
  void setCurrentSliceCoronal(int slice);
  void setCurrentSliceSagittal(int slice);
  void setCurrentSlice(int axis, int slice);
  void selectColormap(QAction *action);
  void updateProjection(vtkVolume *volume);
  //void addGeometryDifference();

private:
  MedSquare *medSquare;

  // current image
  vtkmsqImageItem *currentImageItem;
  vtkmsqImageItem *emptyImageItem;
  vtkImageData *currentImage;
  vtkImageData *emptyImage;
  vtkmsqMedicalImageProperties *currentProperties;
  vtkmsqMedicalImageProperties *emptyImageProperties;
  double range[2];

  MSQRenderWidget *widgets[4];
  int currentRenderer;

  // canvas
  QSplitter *splitLeftRight;
  QList<int> splitterSize;

  // cursor
  vtkActor *cursor[3];
  vtkmsqFrameSource *cursorData[3];
  vtkPolyDataMapper *cursorMapper[3];

  // access to colormap functions
  MSQColormapFactory *msqColormap;

  // scene manipulation widgets
  MSQSliceNavigationWidget *sliceNavigationWidget;
  MSQWindowLevelWidget *windowlevelWidget;

  MSQProjectionMenu *projectionMenu;
  vtkVolume *projection;

  void initializeEmptyImage();
  void createSliceNavigationDockWidget();
  void createWindowLevelDockWidget();
  void createGeometryDockWidget();
  void createGeometryDifferenceAction();
  void createProjectionMenu();
  void createScreenShotAction();

  void enableWidgets(bool enable);
  void updateFrameSelection();

  QMenu *createColormapMenu();

  void createVTKPipeline();
  void createCursors();
  void createInteractor();
  void updateInteractor();
};

#endif
