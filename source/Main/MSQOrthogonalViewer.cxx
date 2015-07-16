/*=========================================================================

 Program:   MedSquare
 Module:    MSQOrthogonalViewer.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQOrthogonalViewer.h"

#include "MSQ3DRenderWidget.h"
#include "MSQAxialRenderWidget.h"
#include "MSQColormapFactory.h"
#include "MSQCoronalRenderWidget.h"
#include "MSQGeometryDifference.h"
#include "MSQGeometryDialog.h"
#include "MSQGeometryItem.h"
#include "MSQGeometryWidget.h"
#include "MSQOrientationColors.h"
#include "MSQProjectionMenu.h"
#include "MSQSagittalRenderWidget.h"
#include "MSQWindowLevelWidget.h"

#include "vtkmsqFrameSource.h"
#include "vtkmsqMedicalImageProperties.h"

#include "vtkActor.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkScalarBarActor.h"

#define VTK_CREATE(type, name) \
   vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

/***********************************************************************************//**
 *
 */
MSQOrthogonalViewer::MSQOrthogonalViewer(MedSquare *medSquare) : medSquare(medSquare)
{
  // initialize image and properties
  initializeEmptyImage();

  // set default font size
  QFont font;
  font.setPointSize(11);

  // define default splitter subdivision size
  QList<int> list_size;
  list_size.push_back(400);
  list_size.push_back(400);

  // create left-right splitter
  splitLeftRight = new QSplitter(medSquare);

  // create top-down splitter
  QSplitter *splitTopDown = new QSplitter(Qt::Vertical, splitLeftRight);

  // create widgets
  widgets[0] = new MSQSagittalRenderWidget(this);
  widgets[1] = new MSQCoronalRenderWidget(this);
  widgets[2] = new MSQAxialRenderWidget(this);
  widgets[3] = new MSQ3DRenderWidget(this);

  splitTopDown->addWidget(widgets[0]);
  splitTopDown->addWidget(widgets[1]);
  splitTopDown->addWidget(widgets[2]);
  splitLeftRight->addWidget(widgets[3]);

  // adjust splitter sizes
  splitLeftRight->setSizes(list_size);
  splitLeftRight->setStretchFactor(1, 2);

  //sets main widget as left-right splitter
  medSquare->setCentralWidget(splitLeftRight);

  // create an instance of colormap helper class
  this->msqColormap = new MSQColormapFactory();

  createScreenShotAction();

  // create slice navigation widget
  createSliceNavigationDockWidget();

  // create window/level widget
  createWindowLevelDockWidget();

  //createGeometryDifferenceAction();

  // create MIP action
  createProjectionMenu();

  // build menus
  //this->medSquare->addMenuToViewMenu(createColormapMenu(), true);

  // build pipeline
  createCursors();
  updateInteractor();
  createVTKPipeline();

  // display initial image data
  setInput(this->emptyImageItem);

  widgets[0]->createOrientationMarkerWidgets();
  widgets[1]->createOrientationMarkerWidgets();
  widgets[2]->createOrientationMarkerWidgets();
  widgets[3]->createOrientationMarkerWidgets();

  // current renderer
  this->currentRenderer = 0;

}

/***********************************************************************************//**
 *
 */
MSQOrthogonalViewer::~MSQOrthogonalViewer()
{
  for (int i = 0; i < 3; i++)
  {
    cursor[i]->Delete();
    cursorData[i]->Delete();
    cursorMapper[i]->Delete();
  }

  emptyImage->Delete();
  emptyImageProperties->Delete();

  delete this->sliceNavigationWidget;
  delete this->windowlevelWidget;
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::initializeEmptyImage()
{
  // Creates an empty 3x3x3 short image
  this->emptyImage = vtkImageData::New();
  this->emptyImage->SetDimensions(3, 3, 3);
  this->emptyImage->SetScalarTypeToShort();
  this->emptyImage->SetNumberOfScalarComponents(1);
  this->emptyImage->SetSpacing(1, 1, 1);
  this->emptyImage->SetOrigin(0, 0, 0);
  this->emptyImage->AllocateScalars();
  this->emptyImage->GetPointData()->GetScalars()->FillComponent(0, 0);

  this->emptyImageProperties = vtkmsqMedicalImageProperties::New();
  this->emptyImageProperties->SetOrientationType(vtkMedicalImageProperties::AXIAL);

  this->emptyImageItem = vtkmsqImageItem::New();
  this->emptyImageItem->SetImage(this->emptyImage);
  this->emptyImageItem->SetProperties(this->emptyImageProperties);
}

/***********************************************************************************//**
 *
 * \author: Daniel Oliveira Dantas
 */
void MSQOrthogonalViewer::refresh()
{
	  this->widgets[0]->refresh();
	  this->widgets[1]->refresh();
	  this->widgets[2]->refresh();
	  this->widgets[3]->refresh();
}

/***********************************************************************************//**
 *
 * \author: Daniel Oliveira Dantas
 */
void MSQOrthogonalViewer::setInput(vtkmsqImageItem* newImageItem)
{

  if (newImageItem == NULL){
	this->setInput(this->emptyImageItem);
	return;
  }

  this->currentImageItem = newImageItem;

  vtkImageData *newImage = newImageItem->GetImage();
  vtkmsqMedicalImageProperties *newProperties = newImageItem->GetProperties();

  this->currentImage = newImage;
  this->currentProperties = newProperties;

  int extent[6];
  this->currentImage->GetExtent(extent);

  this->widgets[0]->setInput(newImageItem);
  this->widgets[1]->setInput(newImageItem);
  this->widgets[2]->setInput(newImageItem);
  this->widgets[3]->setInput(newImageItem);

  // assign current image to colormap control
  this->windowlevelWidget->setInput(this->currentImage, this->currentProperties);
  this->windowlevelWidget->getOptimalRange(this->range);

  // assign current image to slice navigation control
  this->sliceNavigationWidget->setInput(this->currentImage, this->currentProperties);

  this->projectionMenu->setInput(this->currentImageItem);
  this->projectionMenu->setActive(this->hasImageLoaded());

  this->widgets[0]->reset();
  this->widgets[1]->reset();
  this->widgets[2]->reset();
  this->widgets[3]->reset();

  this->medSquare->enableInspector(this->hasImageLoaded());

  this->updateInteractor();
  this->updateFrameSelection();
}

/***********************************************************************************//**
 *
 */
bool MSQOrthogonalViewer::hasImageLoaded()
{
  return this->emptyImage != this->currentImage;
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::setCurrentRenderer(vtkRenderer *renderer)
{
  for (int i=0; i<4; i++)
  {
    if (widgets[i]->has(renderer))
    {
      this->currentRenderer = i;
      break;
    }
  }

  updateFrameSelection();
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::updateFrameSelection()
{
  this->widgets[0]->setFrameEnabled(this->currentRenderer == 0);
  this->widgets[1]->setFrameEnabled(this->currentRenderer == 1);
  this->widgets[2]->setFrameEnabled(this->currentRenderer == 2);
  this->widgets[3]->setFrameEnabled(this->currentRenderer == 3);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::toggleMaximizeWindow(vtkRenderer *renderer)
{
  // handle 3d splitter size
  if (widgets[3]->has(renderer))
  {
    if (!widgets[0]->isVisible() && !widgets[1]->isVisible() && !widgets[2]->isVisible())
      // Restore
      splitLeftRight->setSizes(splitterSize);
    else {
      // Maximize
      splitterSize = splitLeftRight->sizes();
      QList<int> maximized;
      maximized << 0 << splitterSize[1];
      splitLeftRight->setSizes(maximized);
    }
  }

  // define default splitter subdivision size
  widgets[0]->toggleMaximized(renderer);
  widgets[1]->toggleMaximized(renderer);
  widgets[2]->toggleMaximized(renderer);
  widgets[3]->toggleMaximized(renderer);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::updateInteractor()
{
  if (this->hasImageLoaded())
  {
    widgets[0]->enableInteraction();
    widgets[1]->enableInteraction();
    widgets[2]->enableInteraction();
    widgets[3]->enableInteraction();
  }
  else
  {
    widgets[0]->disableInteraction();
    widgets[1]->disableInteraction();
    widgets[2]->disableInteraction();
    widgets[3]->disableInteraction();
  }
}

/***********************************************************************************//**
 *
 */
MSQOrientationType MSQOrthogonalViewer::getOrientationType(vtkRenderer *renderer)
{

  if (widgets[0]->has(renderer))
  {
    return SAGITTAL;
  }
  else if (widgets[1]->has(renderer))
  {
    return CORONAL;
  }
  else if (widgets[2]->has(renderer))
  {
    return AXIAL;
  }
  else
    return OTHER;
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::updateSlices(vtkRenderer *pokedRenderer, int x, int y)
{
  char intensityMessage[256];
  double intensity;
  double image_coord[3];

  int rendererIndex = 0;
  if (widgets[0]->has(pokedRenderer))
    rendererIndex = SAGITTAL_RENDERER;
  else if (widgets[1]->has(pokedRenderer))
    rendererIndex = CORONAL_RENDERER;
  else
    rendererIndex = AXIAL_RENDERER;

  // pick at mouse location
  if ((intensity = this->widgets[rendererIndex]->pick(x, y, image_coord)) == -1)
    return;

  switch (rendererIndex)
  {
    case AXIAL_RENDERER:
      // XY
      sliceNavigationWidget->setCoronal  ( static_cast<int> ( image_coord[1] ) );
      sliceNavigationWidget->setSagittal ( static_cast<int> ( image_coord[0] ) );
      break;

    case SAGITTAL_RENDERER:
      // YZ
      sliceNavigationWidget->setAxial   ( static_cast<int> ( image_coord[2] ) );
      sliceNavigationWidget->setCoronal ( static_cast<int> ( image_coord[1] ) );
      break;

    case CORONAL_RENDERER:
      // XZ
      sliceNavigationWidget->setSagittal ( static_cast<int> ( image_coord[0] ) );
      sliceNavigationWidget->setAxial    ( static_cast<int> ( image_coord[2] ) );
      break;

    default:
      break;
  }

  sprintf(intensityMessage, "Intensity at [%g, %g, %g] = %g", image_coord[0],
      image_coord[1], image_coord[2], intensity);

  this->medSquare->updateStatusBar(QString(intensityMessage), false, 5000);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::incrementSlice(int increment, vtkRenderer *renderer)
{
  if (!this->hasImageLoaded())
  {
    return;
  }

  MSQOrientationType orient = this->getOrientationType(renderer);

  switch (orient)
  {
    case AXIAL:
      this->sliceNavigationWidget->incrementAxial(increment);
      break;
    case CORONAL:
      this->sliceNavigationWidget->incrementCoronal(increment);
      break;
    case SAGITTAL:
      this->sliceNavigationWidget->incrementSagittal(increment);
      break;
    case OTHER:
      break;
  }
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::zoomIn()
{
  widgets[0]->dolly(pow(1.1, 1.0));
  widgets[1]->dolly(pow(1.1, 1.0));
  widgets[2]->dolly(pow(1.1, 1.0));
  widgets[3]->dolly(pow(1.1, 1.0));
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::zoomOut()
{
  widgets[0]->dolly(pow(1.1, -1.0));
  widgets[1]->dolly(pow(1.1, -1.0));
  widgets[2]->dolly(pow(1.1, -1.0));
  widgets[3]->dolly(pow(1.1, -1.0));
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::reset()
{
  widgets[0]->reset();
  widgets[1]->reset();
  widgets[2]->reset();
  widgets[3]->reset();
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::setWindow(double value)
{
  this->windowlevelWidget->setWindow(value);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::setLevel(double value)
{
  this->windowlevelWidget->setLevel(value);
}

/***********************************************************************************//**
 *
 */
double MSQOrthogonalViewer::getWindow()
{
  return this->windowlevelWidget->getWindow();
}

/***********************************************************************************//**
 *
 */
double MSQOrthogonalViewer::getLevel()
{
  return this->windowlevelWidget->getLevel();
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::createSliceNavigationDockWidget()
{
  this->sliceNavigationWidget = new MSQSliceNavigationWidget(medSquare);
  this->medSquare->addDockWidget(Qt::RightDockWidgetArea, this->sliceNavigationWidget);

  connect(sliceNavigationWidget, SIGNAL(componentChanged(int)), widgets[0], SLOT(setComponent(int)));
  connect(sliceNavigationWidget, SIGNAL(componentChanged(int)), widgets[1], SLOT(setComponent(int)));
  connect(sliceNavigationWidget, SIGNAL(componentChanged(int)), widgets[2], SLOT(setComponent(int)));
  connect(sliceNavigationWidget, SIGNAL(componentChanged(int)), widgets[3], SLOT(setComponent(int)));
  connect(sliceNavigationWidget, SIGNAL(sliceChanged(int, int)), this, SLOT(setCurrentSlice(int, int)));

  this->medSquare->addActionToToolsMenu(this->sliceNavigationWidget->toggleViewAction(), true);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::createWindowLevelDockWidget()
{
  this->windowlevelWidget = new MSQWindowLevelWidget(medSquare);
  this->medSquare->addDockWidget(Qt::RightDockWidgetArea, this->windowlevelWidget);

  connect(this->windowlevelWidget, SIGNAL(windowChanged(double)), widgets[0], SLOT(setWindow(double)));
  connect(this->windowlevelWidget, SIGNAL(windowChanged(double)), widgets[1], SLOT(setWindow(double)));
  connect(this->windowlevelWidget, SIGNAL(windowChanged(double)), widgets[2], SLOT(setWindow(double)));
  connect(this->windowlevelWidget, SIGNAL(windowChanged(double)), widgets[3], SLOT(setWindow(double)));

  connect(this->windowlevelWidget, SIGNAL(levelChanged(double)), widgets[0], SLOT(setLevel(double)));
  connect(this->windowlevelWidget, SIGNAL(levelChanged(double)), widgets[1], SLOT(setLevel(double)));
  connect(this->windowlevelWidget, SIGNAL(levelChanged(double)), widgets[2], SLOT(setLevel(double)));
  connect(this->windowlevelWidget, SIGNAL(levelChanged(double)), widgets[3], SLOT(setLevel(double)));

  connect(this->windowlevelWidget, SIGNAL(opacityChanged(double)), widgets[3], SLOT(setOpacity(double)));

  this->medSquare->addActionToToolsMenu(this->windowlevelWidget->toggleViewAction());
}

//--------------------------------------------------------------------------------------
/*void MSQOrthogonalViewer::createGeometryDifferenceAction()
{
  QAction *geometryDifferenceAction = new QAction(tr("Geometry difference"), this);
  this->medSquare->addActionToToolsMenu(geometryDifferenceAction);
  connect(geometryDifferenceAction, SIGNAL(triggered()), this, SLOT(addGeometryDifference()));
}

//--------------------------------------------------------------------------------------
void MSQOrthogonalViewer::addGeometryDifference()
{
  MSQGeometryDifference *geometryDifference = new MSQGeometryDifference(this->medSquare, this->geometryWidget->getGeometryItems());
  this->geometryWidget->addGeometryItem(geometryDifference->getGeometryItem());
  this->widgets[3]->addActor(geometryDifference->getScalarBar());
}
*/
/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::createProjectionMenu()
{
  this->projectionMenu = new MSQProjectionMenu(this);
  this->medSquare->addMenuToViewMenu(projectionMenu, true);
  connect(this->projectionMenu, SIGNAL(projectionChanged(vtkVolume*)), this, SLOT(updateProjection(vtkVolume*)));
  connect(this,SIGNAL(colormapChanged(int)),this->projectionMenu,SLOT(changedColormap()));
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::updateProjection(vtkVolume *volume)
{
  if(volume == NULL)
  {
    widgets[3]->removeVolume(this->projection);
  }
  else
  {
    if (this->projection)
    {
      widgets[3]->removeVolume(this->projection);
    }
    this->projection = volume;
    widgets[3]->addVolume(this->projection);
  }
}

/***********************************************************************************//**
 * Instantiate VTK objects and make the appropriate connections
 */
void MSQOrthogonalViewer::createVTKPipeline()
{
  this->widgets[0]->addActor(this->cursor[1]);
  this->widgets[0]->addActor(this->cursor[2]);

  this->widgets[1]->addActor(this->cursor[0]);
  this->widgets[1]->addActor(this->cursor[2]);

  this->widgets[2]->addActor(this->cursor[0]);
  this->widgets[2]->addActor(this->cursor[1]);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::addGeometry(MSQGeometryItem *item)
{
  widgets[3]->addActor(item->getActor());
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::removeGeometry(MSQGeometryItem *item)
{
  widgets[3]->removeActor(item->getActor());
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::updateGeometry(MSQGeometryItem *item)
{
  widgets[3]->refresh();
}

/***********************************************************************************//**
 * Sets current axial slice by reslicing the image data
 */
void MSQOrthogonalViewer::setCurrentSliceAxial(int slice)
{
  this->setCurrentSlice(this->sliceNavigationWidget->getAxialAxis(), slice);
}

/***********************************************************************************//**
 * Sets current coronal slice by reslicing the image data
 */
void MSQOrthogonalViewer::setCurrentSliceCoronal(int slice)
{
  this->setCurrentSlice(this->sliceNavigationWidget->getCoronalAxis(), slice);
}

/***********************************************************************************//**
 * Sets current sagittal slice by reslicing the image data
 */
void MSQOrthogonalViewer::setCurrentSliceSagittal(int slice)
{
  this->setCurrentSlice(this->sliceNavigationWidget->getSagittalAxis(), slice);
}

/***********************************************************************************//**
 * Sets current slice by reslicing the image data
 */
void MSQOrthogonalViewer::setCurrentSlice(int axis, int slice)
{
  int extent[6];
  double spacing[3];
  double origin[3];
  double* p0;
  double* p1;
  double* p2;

  // get image extents, spacing and origin
  currentImage->GetExtent(extent);
  currentImage->GetSpacing(spacing);
  currentImage->GetOrigin(origin);

  if (MSQ_REORIENT)
  {
    this->currentProperties->GetReorientedExtent(extent, extent, 0);
    this->currentProperties->GetReorientedDouble3(origin, origin, 0);
    this->currentProperties->GetReorientedDouble3(spacing, spacing);
  }

  switch (axis)
  {
    case SAGITTAL_RENDERER:
      this->widgets[0]->setSagittalSlice(slice);
      this->widgets[1]->setSagittalSlice(slice);
      this->widgets[2]->setSagittalSlice(slice);
      this->widgets[3]->setSagittalSlice(slice);

      p0 = this->widgets[0]->GetOrigin();
      p1 = this->widgets[0]->GetPoint1();
      p2 = this->widgets[0]->GetPoint2();

      cursorData[0]->SetOrigin(p0);
      cursorData[0]->SetPoint1(p1);
      cursorData[0]->SetPoint2(p2);

      break;

    case CORONAL_RENDERER:
      this->widgets[0]->setCoronalSlice(slice);
      this->widgets[1]->setCoronalSlice(slice);
      this->widgets[2]->setCoronalSlice(slice);
      this->widgets[3]->setCoronalSlice(slice);

      p0 = this->widgets[1]->GetOrigin();
      p1 = this->widgets[1]->GetPoint1();
      p2 = this->widgets[1]->GetPoint2();

      cursorData[1]->SetOrigin(p0);
      cursorData[1]->SetPoint1(p1);
      cursorData[1]->SetPoint2(p2);

      break;

    case AXIAL_RENDERER:
    default:
      this->widgets[0]->setAxialSlice(slice);
      this->widgets[1]->setAxialSlice(slice);
      this->widgets[2]->setAxialSlice(slice);
      this->widgets[3]->setAxialSlice(slice);

      p0 = this->widgets[2]->GetOrigin();
      p1 = this->widgets[2]->GetPoint1();
      p2 = this->widgets[2]->GetPoint2();

      cursorData[2]->SetOrigin(p0);
      cursorData[2]->SetPoint1(p1);
      cursorData[2]->SetPoint2(p2);

      break;
  }
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::setCurrentColormap(vtkmsqLookupTable *newColormap)
{
  this->currentImageItem->SetColormap(newColormap);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::selectColormap(QAction *action)
{
  if (!strcmp(action->text().toLocal8Bit().constData(), "Open..."))
  {
    QString currentFileName = "./../colormap";
    QString colormapName = QFileDialog::getOpenFileName(this->medSquare,
        tr("Open colormaps"), currentFileName);
    if (!colormapName.isEmpty())
    {
      vtkmsqLookupTable *custom_cmap = msqColormap->loadColormap(colormapName.toStdString());
      if (custom_cmap == NULL)
        this->medSquare->warningMessage(tr("Error reading file %1.").arg(colormapName),
            tr("Make sure file is of correct type and retry."));
      else
        setCurrentColormap(custom_cmap);
    }
  }
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Grayscale"))
  {
    setCurrentColormap(msqColormap->createColormap(MSQ_LUT_GRAY, 256));
    emit colormapChanged(MSQ_LUT_GRAY);
  }
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Hue"))
  {
    setCurrentColormap(msqColormap->createColormap(MSQ_LUT_HUE, 256));
    emit colormapChanged(MSQ_LUT_HUE);
  }
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Saturation"))
  {
    setCurrentColormap(msqColormap->createColormap(MSQ_LUT_SATURATION, 256));
    emit colormapChanged(MSQ_LUT_SATURATION);
  }
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Hot"))
  {
    setCurrentColormap(msqColormap->createColormap(MSQ_LUT_HOT, 256));
    emit colormapChanged(MSQ_LUT_HOT);
  }
}

/***********************************************************************************//**
 * Build widget menu
 */
/*
QMenu *MSQOrthogonalViewer::createColormapMenu()
{
  QMenu *colormapMenu = new QMenu(tr("Colormapss"));
  connect(colormapMenu, SIGNAL(triggered(QAction*)), this, SLOT(selectColormap(QAction*)));

  QAction *grayColormap = new QAction(tr("Grayscale"), this);
  colormapMenu->addAction(grayColormap);
  QAction *hueColormap = new QAction(tr("Hue"), this);
  colormapMenu->addAction(hueColormap);
  QAction *satColormap = new QAction(tr("Saturation"), this);
  colormapMenu->addAction(satColormap);
  QAction *hotColormap = new QAction(tr("Hot"), this);
  colormapMenu->addAction(hotColormap);

  colormapMenu->addSeparator();

  QAction *customColormap = new QAction(tr("Open..."), this);
  colormapMenu->addAction(customColormap);

  return colormapMenu;
}
*/

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::createCursors()
{
  for (int i = 0; i < 3; i++)
  {
    // create cursor
    cursorData[i] = vtkmsqFrameSource::New();

    // create cursor mapper
    cursorMapper[i] = vtkPolyDataMapper::New();
    cursorMapper[i]->SetInput(cursorData[i]->GetOutput());

    // create cursor actor
    cursor[i] = vtkActor::New();
    cursor[i]->SetMapper(cursorMapper[i]);
    cursor[i]->PickableOff();
    cursor[i]->GetProperty()->SetLineWidth(1.0);
    cursor[i]->GetProperty()->SetOpacity(0.4);
    cursor[i]->GetProperty()->SetAmbient(1.0);
    cursor[i]->GetProperty()->SetDiffuse(0.0);
    cursor[i]->GetProperty()->SetSpecular(0.0);
    cursor[i]->GetProperty()->SetRepresentationToWireframe();
  }

  cursor[0]->GetProperty()->SetColor(MSQOrientationColors::sagittalColor);
  cursor[1]->GetProperty()->SetColor(MSQOrientationColors::coronalColor);
  cursor[2]->GetProperty()->SetColor(MSQOrientationColors::axialColor);
}

/***********************************************************************************//**
 *
 */
void MSQOrthogonalViewer::createScreenShotAction()
{
  QMenu *menuScreenShot = new QMenu(tr("Take Screen Shot"), this);
  QAction *actionSagitalScreenshot = new QAction(tr("From Sagittal"), this);
  QAction *actionCoronalScreenshot = new QAction(tr("From Coronal"), this);
  QAction *actionAxialScreenshot = new QAction(tr("From Axial"), this);
  QAction *action3DScreenshot = new QAction(tr("From 3D"), this);

  menuScreenShot->addAction(actionSagitalScreenshot);
  menuScreenShot->addAction(actionCoronalScreenshot);
  menuScreenShot->addAction(actionAxialScreenshot);
  menuScreenShot->addSeparator();
  menuScreenShot->addAction(action3DScreenshot);

  connect(actionSagitalScreenshot, SIGNAL(triggered()), widgets[0], SLOT(takeSnapshot()));
  connect(actionCoronalScreenshot, SIGNAL(triggered()), widgets[1], SLOT(takeSnapshot()));
  connect(actionAxialScreenshot, SIGNAL(triggered()), widgets[2], SLOT(takeSnapshot()));
  connect(action3DScreenshot, SIGNAL(triggered()), widgets[3], SLOT(takeSnapshot()));


  this->medSquare->addMenuToFileMenu(menuScreenShot, true);
}
