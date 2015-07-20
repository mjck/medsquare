#ifndef MSQDicomExplorer_H
#define MSQDicomExplorer_H

#include <QtGui>
#include "QVTKWidget.h"

#include "gdcmSubject.h"
#include "gdcmCommand.h"
#include "gdcmEvent.h"
#include "gdcmSmartPointer.h"
#include "gdcmProgressEvent.h"
#include "gdcmScanner.h"
#include "gdcmDataElement.h"
#include "gdcmAttribute.h"
#include "gdcmSerieHelper.h"

#include "MSQDicomHeaderViewer.h"
#include "MSQDicomImageViewer.h"
#include "MSQDicomImageSorter.h"
#include "MSQDicomQualityControl.h"

#include "MSQColormapFactory.h"

#define MAX_COLORMAPS 5

class MSQFileWithName : public gdcm::FileWithName
{
public:
  MSQFileWithName(gdcm::File &f):gdcm::FileWithName(f),enabled(true){}
  bool enabled;
};

class MSQDicomExplorer : public QMainWindow
{
Q_OBJECT

public:
  // Constructor/Destructor
  MSQDicomExplorer(QWidget* parent = 0);
  ~MSQDicomExplorer();

  QProgressBar *progressBar();

  void updateStatusBar(QString message, bool showProgressBar, int timeout = 0);
  void warningMessage(const QString &text, const QString &info);
  typedef bool (*SortFunction)(gdcm::DataSet const &, gdcm::DataSet const &);

signals:


public slots:
  void updateProgressBar(vtkObject *caller, unsigned long eventId, void *clientData, void* callData);
  //void fileOpen();
  //void fileClose(int iClosed);
  //void fileCloseSelected();
  //void setCurrentImage(int iCurrent);
  //void enableInspector(bool enable);

private slots:
  void layerChanged(int);
  void colormapChanged(int);
  void opacityChanged(int);
  void selectColormap(QAction *action);
  void dicomSelectionChanged();
  void dicomItemChanged(QTreeWidgetItem *, int);
  //void removeSelection();
  void viewShowHeader();
  void viewShowImage();
  void viewShowTools();
  void fileExit();
  void fileOpenDir();
  void fileCopySelected();
  void fileExportAs2D();
  void fileExportAs3D();
  void fileExportAs4D();
  void fileSort(bool restore=false);
  //void fileCheckQuality();
  void fileRestore();
  void fileFilter();
  void helpAbout();

private:
  // Qt actions
  QAction *afileSource;
  QAction *afileExportAs2D;
  QAction *afileExit;
  QAction *aviewHeader;
  QAction *aviewImage;
  QAction *aoptionsSaveValues;
  //QAction *asortFiles;
  QAction *aviewTools;
  QAction *aeditCopyTo;
  QAction *aeditDelete;
  QAction *aeditRestore;
  QAction *ahelpAbout;

  bool viewHeader;
  bool viewImage;
  bool viewTools;
  bool initializing;

  // Current file path
  QString currentFileName;
  QString currentFilter;
  QString currentPath;

  // Qt widgets
  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *viewMenu;
  QMenu *colormapMenu;
  QMenu *exportMenu;
  //QMenu *toolsMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *viewToolBar;
  QToolBar *layerToolBar;
  QTabWidget *toolSet;
  QComboBox *layerCombo;
  QComboBox *colormapCombo;
  QComboBox *opacityCombo;
  
  QSplitter *mainSplitter;
  QList<int> mainSplitterSize;
  QSplitter *leftSplitter;
  QList<int> leftSplitterSize;

  QProgressBar *myProgressBar;
  QProgressDialog *mProgressDialog; 
  QTreeWidget *dicomTree, *tagTree;
  QCheckBox *mAppendValues;

  MSQDicomHeaderViewer *headerViewer;
  MSQDicomImageViewer *imageViewer;
  MSQDicomImageSorter *sortingControl;
  MSQDicomQualityControl *qualityControl;

  long fileCount;
  long totalFiles;
  long totalSorted;

  std::vector<std::string> mFilenames;
  std::vector< gdcm::SmartPointer<MSQFileWithName> > mFileList;

  int layer;
  int layerColormap[2];
  int layerOpacity[2];

  MSQColormapFactory *colormapFactory;
  vtkmsqLookupTable *colormaps[MAX_COLORMAPS];
  int currentColormap;

  QString basePath();

  // helper functions
  void initialize();
  void createMenus();
  void createActions();
  void createStatusBar();
  void createToolBars();
  void createLayerToolBar(QToolBar *toolbar);
  void createInterface();
  void reset();

  void updateProgress(gdcm::Subject *caller, const gdcm::Event &evt);
  void setCurrentFile(const QString& fileName);
  void readDirectory(const QString& dirName);
  //bool sortFiles(std::vector<std::string> const & filenames, std::vector<gdcm::Tag> const& tags, std::vector<int> const& order);
  std::string GetStringValueFromTag(const gdcm::Tag& t, const gdcm::File& ds);
  void addToDicomTree(std::string fileName, unsigned int index, bool enabled, const QVector<gdcm::Tag> &tags, 
    const QStringList &descriptions, const QStringList &aliases, const QVector<bool> &groups);

  double GetSliceSpacingFromDataset(const gdcm::DataSet& ds);
  int GetDominantOrientation(const double *dircos);

  void fileCopySelectedRecursive(QTreeWidgetItem *item, bool selected, const QString& dirName);

  //short equalize(short input, double window, double center);
  //void fileCheckQualityRecursive(QTreeWidgetItem *item, double topperc);
  //void statistics(gdcm::Image const & gimage, char *buffer, double window, double center, double *entropy, double *mean);

  bool exportToAnalyze(const QString& fileName, const QString& fileNameAnalyze);
  bool exportToAnalyze(const QStringList& fileNames, const QString& fileNameAnalyze, int components=1);

  void fileExportToAnalyze(QString preffix, QTreeWidgetItem *item, long count);
  void fileExport2DRecursive(QString preffix, QTreeWidgetItem *item, bool selected, long *count);
  void fileExport3DRecursive(QStringList& fileNames, QTreeWidgetItem *item, bool selected, long *count);
  void fileExport4DRecursive(QStringList& fileNames, QTreeWidgetItem *item, bool selected, long *count, int *comp);

  void toggleItem(QTreeWidgetItem *item, bool enabled);
  int countFiles(const QString &path);//, bool countDirs=false);

  //void selectColormap(QAction *action);
  //MSQImageIO *msq_imageIO;
};

#endif // MSQDicomExplorer_H
