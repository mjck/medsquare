#ifndef MEDSQUARE_H
#define MEDSQUARE_H

#define MSQ_MAX_IMAGE_OPEN 3

#include <QtGui>

#include "QVTKWidget.h"

class vtkmsqImageItem;
class vtkmsqLookupTable;
class vtkmsqMedicalImageProperties;

class vtkImageData;

class MSQViewer;
class MSQGeometryItem;
class MSQInspectorWidget;
class MSQImportDICOMDialog;
class MSQImageIO;

class MedSquare : public QMainWindow
{
Q_OBJECT

public:
  // Constructor/Destructor
  MedSquare(QWidget* parent = 0);
  ~MedSquare();

  void addActionToFileMenu(QAction *action, bool addSeparator = false);
  void addActionToViewMenu(QAction *action, bool addSeparator = false);
  void addActionToToolsMenu(QAction *action, bool addSeparator = false);
  void addMenuToFileMenu(QMenu *menu, bool addSeparator = false);
  void addMenuToViewMenu(QMenu *menu, bool addSeparator = false);

  QProgressBar *progressBar();

  int  updateImageAndProperties(vtkImageData *newImage, vtkmsqMedicalImageProperties *newProperties);
  void updateStatusBar(QString message, bool showProgressBar, int timeout = 0);

  void warningMessage(const QString &text, const QString &info);

  vtkmsqImageItem* getCurrentImage();  
  vtkImageData* getImageDataAt(int i);
  vtkmsqMedicalImageProperties* getImagePropertiesAt(int i);
  vtkmsqLookupTable* getImageLookupTableAt(int i);
  int getImageOpenNum();
  int getImageSelectedIdx();
  void setImageLookupTableAt(int imageIndex, vtkmsqLookupTable *colormap);

  void refresh();

signals:
  void imageLoaded();

public slots:
  void updateProgressBar(vtkObject *caller, unsigned long eventId, void *clientData, void* callData);
  void fileOpen();
  void fileClose(int iClosed);
  void fileCloseSelected();
  void setCurrentImage(int iCurrent);
  void enableInspector(bool enable);

private slots:
  void fileNew();
  void fileSave();
  void fileExit();
  void viewZoomIn();
  void viewZoomOut();
  void viewResetAll();
  void helpAbout();

  void fileImportDicom();
  void importDicomFiles(const QStringList& selected, const QString& seriesName,
      const double sliceSpacing);
  void useOrthogonalViewer();
  void exportSlices();

private:
  // Qt actions
  QAction *afileNew, *afileOpen;
  QAction *afileSave, *afileCompress;
  QAction *afileImport;
  QAction *afileExit;
  QAction *auseOrthogonalViewer;
  QAction *aviewZoomIn;
  QAction *aviewZoomOut;
  QAction *aviewReset;
  QAction *aviewInspector;
  QAction *ahelpAbout;
  QAction *openGeometry;
  QAction *aexportSlices;

  QActionGroup *agViewers;

  // Current file path
  QString currentFileName;
  QString currentFilter;
  QString currentPath;

  // Qt widgets
  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *toolsMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QProgressBar *myProgressBar;

  // helper functions
  void initialize();
  void createMenus();
  void createActions();
  void createStatusBar();
  void createToolBars();

  void setCurrentFile(const QString &fileName);

  // vtk objects
  QList<vtkmsqImageItem*> imageList;
  int imageSelected;

  MSQViewer *viewer;
  MSQInspectorWidget *inspectorWidget;
  MSQImportDICOMDialog *dicomDialog;
  MSQImageIO *msq_imageIO;
};

#endif // MEDSQUARE_H
