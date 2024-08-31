//BEGIN OF FILE

/*=========================================================================

 Program:   MedSquare
 Module:    MedSquare.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

/***********************************************************************************//**
 * \file MedSquare.cxx
 *
 * \brief This file implements the GUI.
 *
 * Creates the necessary widgets and populates them with references to
 * the appropriate functions.
 *
 */

/***********************************************************************************/

#include "MedSquare.h"

#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkImageChangeInformation.h"
#include "vtkmsqAnalyzeReader.h"
#include "vtkmsqAnalyzeWriter.h"
#include "vtkmsqPhilipsRECReader.h"
#include "vtkmsqBruker2DSEQReader.h"
#include "vtkmsqNiftiReader.h"
#include "vtkGDCMImageReader.h"
#include "vtkmsqRawReader.h"
#include "vtkmsqLookupTable.h"
#include "vtkmsqMedicalImageProperties.h"
#include "vtkMedicalImageProperties.h"
#include "vtkmsqImageItem.h"

#include "gdcmSystem.h"
#include "gdcmDirectory.h"
#include "gdcmIPPSorter.h"

#include "MSQOrthogonalViewer.h"
#include "MSQImportDICOMDialog.h"
#include "MSQInspectorWidget.h"
#include "MSQImageIO.h"
#include "MSQOpenRawDialog.h"
#include "MSQExportSliceDialog.h"
#include "MSQSliceExporter.h"

/***********************************************************************************//**
 *
 */
MedSquare::MedSquare(QWidget* p) : QMainWindow(p)
{
  this->imageSelected = -1;

  // initialize interface
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  // set current path
  this->currentPath = QDir::currentPath();

  // set current file name
  setCurrentFile("No file");
  setUnifiedTitleAndToolBarOnMac(true);

  // create image info widget
  this->inspectorWidget = new MSQInspectorWidget(this);
  this->inspectorWidget->hide();
  this->addDockWidget(Qt::LeftDockWidgetArea, this->inspectorWidget);
  this->viewMenu->insertAction(this->aviewZoomIn, this->inspectorWidget->toggleViewAction());
  this->viewMenu->insertSeparator(this->aviewZoomIn);

  // set up icons and text for inspector action
  this->inspectorWidget->toggleViewAction()->setIcon(QIcon(":/images/info.png"));
  this->inspectorWidget->toggleViewAction()->setIconText(tr("Inspector"));
  this->inspectorWidget->toggleViewAction()->setStatusTip(tr("Show or hide inspector"));
  this->fileToolBar->addAction(this->inspectorWidget->toggleViewAction());

  this->openGeometry = new QAction(tr("Open Geometry..."), this);
  this->openGeometry->setEnabled(false);
  this->addActionToFileMenu(openGeometry, true);

  connect(this, SIGNAL(imageLoaded()), this->inspectorWidget, SLOT(setInput()));
  connect(this->openGeometry, SIGNAL(triggered()), this->inspectorWidget, SLOT(loadGeometry()));

  // create default viewer
  this->viewer = new MSQOrthogonalViewer(this);

  // create dicom import dialog
  this->dicomDialog = new MSQImportDICOMDialog(this);
  connect(dicomDialog, SIGNAL(filesSelected(const QStringList&, const QString&, const double)), this, SLOT(importDicomFiles(const QStringList &, const QString&, const double)));
  connect(this->inspectorWidget, SIGNAL(geometryChanged(MSQGeometryItem*)), this->viewer, SLOT(updateGeometry(MSQGeometryItem*)));
  connect(this->inspectorWidget, SIGNAL(geometryInserted(MSQGeometryItem*)), this->viewer, SLOT(addGeometry(MSQGeometryItem*)));
  connect(this->inspectorWidget, SIGNAL(geometryRemoved(MSQGeometryItem*)), this->viewer, SLOT(removeGeometry(MSQGeometryItem*)));

  // create an instance of image IO
  this->msq_imageIO = new MSQImageIO(this);

  // initial resize
  QTimer::singleShot(1500, this, SLOT(resizeAndShow()));
}

/***********************************************************************************//**
 * Initilal resize
 */
void MedSquare::resizeAndShow() 
{
  QRect rec = QApplication::primaryScreen()->geometry();
  this->resize(rec.width()*0.8, rec.height()*0.8);
  this->show();
}

/***********************************************************************************//**
 * Add action to controls
 */
void MedSquare::addActionToFileMenu(QAction *action, bool addSeparator)
{
  fileMenu->insertAction(this->afileImport, action);
  if (addSeparator)
    fileMenu->insertSeparator(this->afileImport);
}

/***********************************************************************************//**
 * Add action to controls
 */
void MedSquare::addActionToViewMenu(QAction *action, bool addSeparator)
{
  viewMenu->insertAction(this->aviewZoomIn, action);
  if (addSeparator)
    viewMenu->insertSeparator(this->aviewZoomIn);
}

/***********************************************************************************//**
 * Add action to controls
 */
void MedSquare::addActionToToolsMenu(QAction *action, bool addSeparator)
{
  if (addSeparator)
    toolsMenu->addSeparator();
  toolsMenu->addAction(action);
}

/***********************************************************************************//**
 * Add menu to controls
 */
void MedSquare::addMenuToFileMenu(QMenu *menu, bool addSeparator)
{
  fileMenu->insertMenu(this->afileExit, menu);
  if (addSeparator)
    fileMenu->insertSeparator(this->afileExit);
}

/***********************************************************************************//**
 * Add menu to controls
 */
void MedSquare::addMenuToViewMenu(QMenu *menu, bool addSeparator)
{
  if (addSeparator)
    viewMenu->addSeparator();
  viewMenu->addMenu(menu);
}

/***********************************************************************************//**
 * Returns progress bar widget
 */
QProgressBar *MedSquare::progressBar()
{
  return this->myProgressBar;
}

/***********************************************************************************//**
 *
 */
void MedSquare::createActions()
{
  afileOpen = new QAction(QIcon(":/images/fileopen.png"), tr("&Open..."), this);
  afileOpen->setShortcuts(QKeySequence::Open);
  afileOpen->setStatusTip(tr("Open an existing image file"));
  afileOpen->setIconVisibleInMenu(false);
  connect(afileOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));

  afileImport = new QAction(QIcon(":/images/dicom.png"), tr("&Import DICOM..."), this);
  afileImport->setStatusTip(tr("Import a DICOM image file"));
  afileImport->setIconVisibleInMenu(false);
  afileImport->setIconText(tr("DICOM"));
  connect(afileImport, SIGNAL(triggered()), this, SLOT(fileImportDicom()));

  aexportSlices = new QAction(tr("Export..."), this);
  aexportSlices->setEnabled(false);
  aexportSlices->setStatusTip(tr("Export current image's slices to separate files"));
  connect(this->aexportSlices, SIGNAL(triggered()), this, SLOT(exportSlices()));

  afileCompress = new QAction(tr("Use compression"), this);
  afileCompress->setStatusTip(tr("Compress images upon saving"));
  afileCompress->setCheckable(true);
  afileCompress->setChecked(true);

  afileSave = new QAction(QIcon(":/images/filesave.png"), tr("&Save..."), this);
  afileSave->setShortcuts(QKeySequence::Save);
  afileSave->setStatusTip(tr("Save current image to disk"));
  afileSave->setIconVisibleInMenu(false);
  connect(afileSave, SIGNAL(triggered()), this, SLOT(fileSave()));

  // No image yet to save
  afileSave->setEnabled(false);

  afileExit = new QAction(tr("E&xit"), this);
  afileExit->setShortcuts(QKeySequence::Quit);
  afileExit->setStatusTip(tr("Exit MedSquare"));
  connect(afileExit, SIGNAL(triggered()), this, SLOT(fileExit()));

  aviewZoomIn = new QAction(QIcon(":/images/magplus.png"), tr("Zoom In"), this);
  aviewZoomIn->setShortcuts(QKeySequence::ZoomIn);
  aviewZoomIn->setStatusTip(tr("Zoom in image"));
  aviewZoomIn->setIconVisibleInMenu(false);
  aviewZoomIn->setEnabled(false);
  connect(aviewZoomIn, SIGNAL(triggered()), this, SLOT(viewZoomIn()));

  aviewZoomOut = new QAction(QIcon(":/images/magminus.png"), tr("Zoom Out"), this);
  aviewZoomOut->setShortcuts(QKeySequence::ZoomOut);
  aviewZoomOut->setStatusTip(tr("Zoom out image"));
  aviewZoomOut->setIconVisibleInMenu(false);
  aviewZoomOut->setEnabled(false);
  connect(aviewZoomOut, SIGNAL(triggered()), this, SLOT(viewZoomOut()));

  aviewReset = new QAction(tr("Reset Camera"), this);
  aviewReset->setStatusTip(tr("Reset image camera"));
  aviewReset->setIconVisibleInMenu(false);
  connect(aviewReset, SIGNAL(triggered()), this, SLOT(viewResetAll()));

  ahelpAbout = new QAction(tr("&About MedSquare"), this);
  ahelpAbout->setStatusTip(tr("Show information about MedSquare"));
  connect(ahelpAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));

  // Viewers
  agViewers = new QActionGroup(this);

  auseOrthogonalViewer = new QAction(tr("Orthogonal Viewer"), this);
  auseOrthogonalViewer->setStatusTip(tr("Use an orthogonal viewer for the current image"));
  auseOrthogonalViewer->setActionGroup(agViewers);
  auseOrthogonalViewer->setCheckable(true);
  auseOrthogonalViewer->setChecked(true);
  connect(auseOrthogonalViewer, SIGNAL(triggered()), this, SLOT(useOrthogonalViewer()));
}

/***********************************************************************************//**
 *
 */
void MedSquare::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(afileOpen);
  fileMenu->addAction(afileImport);
  fileMenu->addAction(aexportSlices);
  fileMenu->addSeparator();
  fileMenu->addAction(afileCompress);
  fileMenu->addAction(afileSave);
  fileMenu->addSeparator();
  fileMenu->addAction(afileExit);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(aviewZoomIn);
  viewMenu->addAction(aviewZoomOut);
  viewMenu->addAction(aviewReset);

  toolsMenu = menuBar()->addMenu(tr("&Tools"));
  toolsMenu->addAction(auseOrthogonalViewer);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(ahelpAbout);
}

/***********************************************************************************//**
 *
 */
void MedSquare::createToolBars()
{
  fileToolBar = addToolBar(tr("Toolbar"));
  fileToolBar->addAction(afileOpen);
  fileToolBar->addAction(afileSave);
  fileToolBar->addAction(afileImport);
  fileToolBar->addSeparator();
  fileToolBar->addAction(aviewZoomIn);
  fileToolBar->addAction(aviewZoomOut);

  // Show text below icon
  fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

/***********************************************************************************//**
 *
 */
void MedSquare::createStatusBar()
{
  // create progress bar
  myProgressBar = new QProgressBar();
  myProgressBar->hide();
  myProgressBar->setMinimum(0);
  myProgressBar->setMaximum(100);
  statusBar()->setStyleSheet("font: 11px");
  statusBar()->addPermanentWidget(myProgressBar);
  statusBar()->showMessage(tr("Ready"), 0);
}

/***********************************************************************************//**
 *
 */
void MedSquare::setCurrentFile(const QString &fileName)
{
  this->currentFileName = fileName;
  setWindowTitle(QString("MedSquare: %1[*]").arg(fileName));
}

/***********************************************************************************//**
 * Public slot to allow everyone to choose which image is selected and viewable.
 *
 * \author Daniel Oliveira Dantas
 */
void MedSquare::setCurrentImage(int iCurrent)
{
  if (iCurrent >= 0 && iCurrent < this->getImageOpenNum() /*and iCurrent != this->imageSelected*/){
    this->imageSelected = iCurrent;

    const char *filename = this->getImagePropertiesAt(this->imageSelected)->GetUserDefinedValue("Filename");
    this->setCurrentFile(filename);

    //apagar
    //printf("**********\n");
    //for(int i = 0; i < this->getImageOpenNum(); i++)
    //{
    //    printf("%s: %s: iCurrent = %d, Colormap = %s\n", __FILE__, __FUNCTION__, i, this->getImageLookupTableAt(i)->GetName().c_str());
    //}
    //printf("\n");
    //printf("%s: %s: iCurrent = %d, Colormap = %s\n", __FILE__, __FUNCTION__, iCurrent, this->getImageLookupTableAt(this->imageSelected)->GetName().c_str());

    this->useOrthogonalViewer();
    //this->viewer->setCurrentColormap(this->getImageLookupTableAt(this->imageSelected));

    //printf("\n");
    //for(int i = 0; i < this->getImageOpenNum(); i++)
    //{
    //    printf("%s: %s: iCurrent = %d, Colormap = %s\n", __FILE__, __FUNCTION__, i, this->getImageLookupTableAt(i)->GetName().c_str());
    //}
    //printf("\n");
    //printf("%s: %s: iCurrent = %d, Colormap = %s\n", __FILE__, __FUNCTION__, iCurrent, this->getImageLookupTableAt(this->imageSelected)->GetName().c_str());
    //printf("**********\n");


  }
}

/***********************************************************************************//**
 *
 */
void MedSquare::useOrthogonalViewer()
{
  if (this->viewer && this->imageSelected >= 0 && this->imageSelected < this->getImageOpenNum())
  {
    viewer->setInput(this->imageList.value(this->imageSelected));
  }
}

/***********************************************************************************//**
 *
 */
void MedSquare::exportSlices()
{
  MSQExportSliceDialog* exportDialog=new MSQExportSliceDialog(this);
  exportDialog->show();   
}

/***********************************************************************************//**
 * Update progress bar
 */
void MedSquare::updateProgressBar(vtkObject *caller, unsigned long eventId,
    void *clientData, void* callData)
{
  int progress = static_cast<int> ( *(static_cast<double*> (callData) ) * 100.0);
  myProgressBar->setValue(progress);

  // make sure we process our own events
  QApplication::processEvents();
}

/***********************************************************************************//**
 * Update the status bar
 */
void MedSquare::updateStatusBar(QString message, bool showProgressBar, int timeout)
{
  if (showProgressBar)
  {
    myProgressBar->setValue(0);
    myProgressBar->show();
  }
  else
  {
    myProgressBar->hide();
  }

  statusBar()->showMessage(message, timeout);
}

/***********************************************************************************//**
 * New image
 */
int MedSquare::updateImageAndProperties(vtkImageData *newImage, vtkmsqMedicalImageProperties *newProperties)
{
  vtkmsqImageItem *newImageItem = vtkmsqImageItem::New();
  newImageItem->SetImage(newImage);
  newImageItem->SetProperties(newProperties);

  this->imageList.append(newImageItem);
  this->imageSelected = this->getImageOpenNum() - 1;

  return 1;
}
/***********************************************************************************//**
 * Enable/disable data manager
 */
void MedSquare::enableInspector(bool enable)
{
  this->inspectorWidget->setEnabled(enable);
  this->openGeometry->setEnabled(enable);
}
/***********************************************************************************//**
 * Create new image
 */
void MedSquare::fileNew()
{
  // TODO
}

/***********************************************************************************//**
 *
 */
void MedSquare::importDicomFiles(const QStringList& selected, const QString& seriesName,
    double sliceSpacing)
{
  if (!selected.isEmpty())
  {
    int success = msq_imageIO->loadDICOMImage(selected, sliceSpacing);

    QFileInfo fileInfo(selected.first());

    // in case no error sets filename
    if (success)
    {

      // sets filename
      setCurrentFile(seriesName);

      // store filename
      this->getImagePropertiesAt(this->imageSelected)->AddUserDefinedValue((const char*) "Filename",
          seriesName.toLatin1());

      // reset display
      viewer->setInput(this->imageList.value(this->imageSelected));

      // yes we can save it now
      this->afileSave->setEnabled(true);
      this->aviewZoomIn->setEnabled(true);
      this->aviewZoomOut->setEnabled(true);

      emit imageLoaded();
    }
    else if (success == 0)
      warningMessage(
          tr("Error reading DICOM series %1 from directory %2.").arg(seriesName).arg(
              fileInfo.filePath()), tr("Make sure file is of correct type and retry."));
    // ready for more
    updateStatusBar(tr("Ready"), false);
  }
}

/***********************************************************************************//**
 *
 */
void MedSquare::fileImportDicom()
{
  dicomDialog->show();
  dicomDialog->raise();
  dicomDialog->activateWindow();
}

/***********************************************************************************//**
 * Refresh screen.
 *
 * Call after changing colormap.
 *
 * \author: Daniel Oliveira Dantas
 */
void MedSquare::refresh()
{
    this->viewer->refresh();
}

/***********************************************************************************//**
 * Action to be taken upon file open
 */
void MedSquare::fileOpen()
{
  QFileDialog dialog(this, tr("Open Image"), currentPath,
      tr("Analyze (*.hdr *.img *img.gz);;"
          "Philips REC/PAR (*.par *.rec *rec.gz);;"
          "NIfTI (*.nii *nii.gz);;"
          "Bruker 2DSEQ (*2dseq);;"
          "MetaImage (*.mha *.mhd);;"
          "Raw (*)"));

  dialog.setNameFilterDetailsVisible(false);
  dialog.setFileMode(QFileDialog::ExistingFile);
  QString fileName;
  if (dialog.exec())
  {
    // gets filename selected as well as current filter
    fileName = dialog.selectedFiles().first();
    currentFilter = dialog.selectedNameFilter();

    // store path for next time
    currentPath = QFileInfo(fileName).path();
  }

  int success = 1;

  // In case a file was chosen try opening it
  if (!fileName.isEmpty())
  {
    // do apropriate reading
    if (currentFilter.contains("Analyze"))
      success = msq_imageIO->loadAnalyzeImage(fileName);

    else if (currentFilter.contains("Philips REC/PAR"))
      success = msq_imageIO->loadPhilipsRECPARImage(fileName);

    else if (currentFilter.contains("NIfTI"))
      success = msq_imageIO->loadNiftiImage(fileName);

    else if (currentFilter.contains("Bruker 2DSEQ"))
      success = msq_imageIO->loadBruker2DSEQImage(fileName);

    else if (currentFilter.contains("MetaImage"))
      success = msq_imageIO->loadMetaImage(fileName);

    else
      success = msq_imageIO->loadRawImage(fileName);

    // in case no error sets filename
    if (success == 1)
    {
      // sets filename
      setCurrentFile(fileName);

      // store filename
      this->getImagePropertiesAt(this->imageSelected)->AddUserDefinedValue((const char*) "Filename",
          fileName.toLatin1());

      // reset display
      viewer->setInput(this->imageList.value(this->imageSelected));

      emit imageLoaded();

      // yes we can save it now
      this->afileSave->setEnabled(true);
      this->aviewZoomIn->setEnabled(true);
      this->aviewZoomOut->setEnabled(true);
      this->aexportSlices->setEnabled(true);
    }
    else if (success == 0){
      warningMessage(tr("Error reading file %1.").arg(fileName),
          tr("Make sure file is of correct type and retry."));
    }
    // ready for more
    updateStatusBar(tr("Ready"), false);
  }
}

/***********************************************************************************//**
 * Action to be taken upon file save
 */
void MedSquare::fileSave()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image as Analyze"),
      currentFileName, tr("Analyze (*.hdr *.img)"), &currentFilter);

  // In case a file was chosen try saving it
  if (!fileName.isEmpty())
  {
    // do appropriate reading
    if (currentFilter == "Analyze (*.hdr *.img)")
      msq_imageIO->saveAnalyzeImage(fileName, this->getImageDataAt(this->imageSelected), this->getImagePropertiesAt(this->imageSelected),
          afileCompress->isChecked());
    // ready for more
    updateStatusBar(tr("Ready"), false);
  }
}

/***********************************************************************************//**
 * Action to be taken upon file close
 *
 * \author Daniel Oliveira Dantas
 */
void MedSquare::fileClose(int iClosed)
{
	if (iClosed < this->getImageOpenNum())
	{
		this->imageList.removeAt(iClosed);
	}

	if (this->getImageOpenNum() > 0)
	{
	  this->imageSelected = 0;

	  // reset display
      viewer->setInput(this->imageList.value(this->imageSelected));
	  // reset info
	}
	else
	{
      this->imageSelected = -1;
      viewer->setInput(NULL);
	}

    emit imageLoaded();

	printf("%s: %s: Closed image. There are %d open images\n", __FILE__, __FUNCTION__, this->getImageOpenNum());
}

/***********************************************************************************//**
 * Action to be taken upon file close
 *
 * \author Daniel Oliveira Dantas
 */
void MedSquare::fileCloseSelected()
{
  if (this->getImageSelectedIdx() >= 0 && this->getImageSelectedIdx() < this->getImageOpenNum())
  {
	this->fileClose(this->getImageSelectedIdx());
  }
}

/***********************************************************************************//**
 *
 */
void MedSquare::warningMessage(const QString &text, const QString &info)
{
  QMessageBox msgBox;
  msgBox.setText(text);
  msgBox.setInformativeText(info);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.exec();
}

/***********************************************************************************//**
 *
 */
void MedSquare::viewZoomIn()
{
  this->viewer->zoomIn();
}

/***********************************************************************************//**
 *
 */
void MedSquare::viewZoomOut()
{
  this->viewer->zoomOut();
}

/***********************************************************************************//**
 *
 */
void MedSquare::viewResetAll()
{
  this->viewer->reset();
}

/***********************************************************************************//**
 *
 */
void MedSquare::helpAbout()
{
  QMessageBox::about(this, tr("About MedSquare"),
      tr("<b>MedSquare</b> is an open-source medical image exploration and "
          "analysis software."));
}

/***********************************************************************************//**
 *
 */
vtkmsqImageItem* MedSquare::getCurrentImage()
{
  return this->imageList.value(this->imageSelected);  
}

/***********************************************************************************//**
 *
 */
vtkImageData* MedSquare::getImageDataAt(int i)
{
  if (i >= 0 && i < this->getImageOpenNum())
  {
	vtkmsqImageItem* imageItem = this->imageList.value(i);
    return imageItem->GetImage();
  }
  return NULL;
}

/***********************************************************************************//**
 *
 */
vtkmsqMedicalImageProperties* MedSquare::getImagePropertiesAt(int i)
{
  if (i >= 0 && i < this->getImageOpenNum())
  {
    vtkmsqImageItem* imageItem = this->imageList.value(i);
	return imageItem->GetProperties();
  }
  return NULL;
}

/***********************************************************************************//**
 *
 */
vtkmsqLookupTable* MedSquare::getImageLookupTableAt(int i)
{
  if (i >= 0 && i < this->getImageOpenNum())
  {
    vtkmsqImageItem* imageItem = this->imageList.value(i);
	return imageItem->GetColormap();
  }
  return NULL;
}


/***********************************************************************************//**
 *
 */
int MedSquare::getImageSelectedIdx()
{
  return this->imageSelected;
}

/***********************************************************************************//**
 *
 */
void MedSquare::setImageLookupTableAt(int imageIndex, vtkmsqLookupTable *colormap)
{
	this->imageList.value(imageIndex)->SetColormap(colormap);
	if (this->imageSelected == imageIndex){
	  this->viewer->setCurrentColormap(colormap);
	}
	else
	{
	  this->setCurrentImage(imageIndex);
	}
}

/***********************************************************************************//**
 *
 */
int MedSquare::getImageOpenNum()
{
  return this->imageList.length();
}


/***********************************************************************************//**
 * Exit MedSquare
 */
void MedSquare::fileExit()
{
  qApp->exit();
}

/***********************************************************************************//**
 *
 */
MedSquare::~MedSquare()
{
  while(!this->imageList.isEmpty()){
	this->imageList.removeAt(0);
  }
}

//END OF FILE
