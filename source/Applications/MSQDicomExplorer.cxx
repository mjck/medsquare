//BEGIN OF FILE

/*=========================================================================

 Project:   MedSquare
 Program:   MSQDicomExplorer
 Module:    MSQDicomExplorer.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

/***********************************************************************************//**
 * \file MSQDicomExplorer.cxx
 *
 * \brief This file implements the GUI for the DICOM Explorer program
 *
 */

/***********************************************************************************/

#include "MSQDicomExplorer.h"

#include "MSQDicomHeaderViewer.h"
#include "MSQDicomImageViewer.h"
#include "MSQDicomImageSorter.h"

#include "MSQTagSortItem.h"

#include "vtkMath.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkImageChangeInformation.h"
#include "vtkmsqMedicalImageProperties.h"
#include "vtkmsqGDCMMoisacImageReader.h"
#include "vtkmsqImageInterleaving.h"
#include "vtkmsqAnalyzeWriter.h"  

#include "gdcmSorter.h"
#include "gdcmElement.h"
#include "gdcmSerieHelper.h"
#include "gdcmFile.h"
#include "gdcmReader.h"
#include "gdcmImageReader.h"
#include "gdcmStringFilter.h"

#include <map>
#include <algorithm>
#include <string>

//namespace {

/*class MSQFileWithName : public gdcm::File
{
public:
  MSQFileWithName(gdcm::File &f):gdcm::File(f),filename(),index(){}
  std::string filename;
  int index;
};*/

//typedef std::pair<double, int> entropy_pair;
//bool entropy_compare(const entropy_pair& a, const entropy_pair& b)//
//{
//  return a.first > b.first;/
//}

namespace {
class SortFunctor
{
public:

  //int count;
  //QProgressDialog *ProgressDialog;
  std::vector<gdcm::Tag> SortTag;
  std::vector<int> SortOrder;
  //double epsilon;
  //bool enable_eps;

  //inline bool equal(double d1, double d2, double epsilon, bool enabled)
  //{
    //printf("(%lf == %lf): %d\n", d1, d2, fabs(d1 - d2) <= epsilon);
    //return enabled ? fabs(d1 - d2) <= epsilon : d1 == d2;
 // }

  int compare(gdcm::File const *file1, gdcm::File const *file2, const gdcm::Tag& tag, int order)
  {
    //cout << "file1: " << *file1 << " file2: " << *file2 << "tag: " << tag << std::endl;
    const gdcm::DataElement& e1 = file1->GetDataSet().GetDataElement( tag );
    const gdcm::DataElement& e2 = file2->GetDataSet().GetDataElement( tag );
  
    gdcm::StringFilter sf1;
    sf1.SetFile(*file1);

    gdcm::StringFilter sf2;
    sf2.SetFile(*file2);

    //ProgressDialog->setValue(count++);
    //cout << count << "\n";
    int res = 0;

    if (!e1.IsEmpty() && !e2.IsEmpty()) 
    {
        long l1, l2;
        double d1, d2;

        //const gdcm::ByteValue *bv1 = e1.GetByteValue();
        //const gdcm::ByteValue *bv2 = e2.GetByteValue();
        //std::string s1 = std::string( bv1->GetPointer(), bv1->GetLength() );
        //std::string s2 = std::string( bv2->GetPointer(), bv2->GetLength() );

        std::string s1 = sf1.ToString( tag );
        std::string s2 = sf2.ToString( tag );

        switch(e1.GetVR())
        {
          case gdcm::VR::IS:
          case gdcm::VR::SL:
          case gdcm::VR::SS:
          case gdcm::VR::US:
            l1 = atol(s1.c_str());
            l2 = atol(s2.c_str());
            if (l1 == l2)
              res = 0;
            else {
              if (order) {
                res = l1 > l2 ? 1 : -1;
              } else {
                res = l1 < l2 ? 1 : -1;
              }
            }
            //printf("long %ld, %ld, res=%d\n", l1, l2, res);
              //res = order ? l1 > l2 : l1 <= l2;
            break;
          case gdcm::VR::DS:
          case gdcm::VR::FL:
          case gdcm::VR::FD:
            d1 = strtod(s1.c_str(), NULL);
            d2 = strtod(s2.c_str(), NULL);
            if (d1 == d2)
            //if (equal(d1, d2, epsilon, enable_eps))
              res = 0;
            else {
              if (order) {
                res = d1 > d2 ? 1 : -1;
              } else {
                res = d1 < d2 ? 1 : -1;
              }
            }
            //printf("double %lf, %lf, res=%d\n", d1, d2, res);
            break;
          default:
            //bool r = std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end());
            //res = order ? s1 > s2 : s1 <= s2;
            if (s1 == s2)
              res = 0;
            else {
              if (order) {
                res = s1 > s2 ? 1 : -1;
              } else {
                res = s1 < s2 ? 1 : -1;
              }
            }
        } // switch
      
    } 

    return res;
  }
  
  bool operator() (gdcm::File const *file1, gdcm::File const *file2)
  {
    int res = 0;
    for(int i=0; i<SortTag.size() && res==0; i++) {
      //if (!file1)
      //  printf("file1 is null");
      //if (!file2)
      //  printf("file2 is null");
      //if (!file1 || !file2) printf("\n");
      res = compare(file1, file2, SortTag[i], SortOrder[i]);
    }
    QApplication::processEvents();
    return res >= 0 ? true : false;
  }

  //void reset()
  //{
  //  count = 0;
 // }

  SortFunctor( std::vector<gdcm::Tag> const& tags, std::vector<int> const &order )
 //   double tol = 1E-5, bool enable_tol = false)//, QProgressDialog *dialog )
  {
    SortTag = tags;
    SortOrder = order;
    //epsilon = tol;
    //enable_eps = enable_tol;
    //ProgressDialog = dialog;
    //reset();
  }
  
  };
}

/***********************************************************************************//**
 * 
 */
inline std::string MSQDicomExplorer::GetStringValueFromTag(const gdcm::Tag& t, const gdcm::File& file)
   // const gdcm::DataSet& ds)
{
  const gdcm::Global& g = gdcm::Global::GetInstance(); // sum of all knowledge !
  const gdcm::Dicts &dicts = g.GetDicts();
  const gdcm::Dict &pub = dicts.GetPublicDict(); // Part 6

  gdcm::StringFilter sf;
  sf.SetFile(file);

  if (file.GetDataSet().FindDataElement(t))
  {
    // If precision is enabled, check data type for float or double
    if (this->sortingControl->precisionEnabled()) {

      const gdcm::DataElement& de = file.GetDataSet().GetDataElement( t );
      const gdcm::ByteValue *bv = de.GetByteValue();
      const gdcm::VR& vr = de.GetVR();

      const gdcm::DictEntry& entry1 = dicts.GetDictEntry( t, NULL );
      const gdcm::VM& vm = entry1.GetVM();
      
      if ( vm == gdcm::VM::VM1 && (vr == gdcm::VR::DS || vr == gdcm::VR::FL || vr == gdcm::VR::FD ) )
      {

        std::string s1 = sf.ToString( t );
        double val = strtod(s1.c_str(), NULL);
        //printf("%lf\n",val);
        std::stringstream valuestream;
        valuestream << std::fixed << std::setprecision(this->sortingControl->precision()) << val;
        //valuestream << val << std::endl;
       
        return valuestream.str();
      }
    
    }

    return sf.ToString( t );
  }

  return std::string("None");
}

/***********************************************************************************//**
 *
 */
QString MSQDicomExplorer::basePath()
{
  QDir dir( QCoreApplication::applicationDirPath() );
  do {
    if (dir.dirName() == "medsquare")
      return dir.absolutePath();
  } while(dir.cdUp());
  return QString();
 }

/***********************************************************************************//**
 *
 */
MSQDicomExplorer::MSQDicomExplorer(QWidget* p) : QMainWindow(p)
{
  // initialize interface
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createInterface();

  // set current path
  initializing = true;
  this->currentPath = QDir::currentPath();

  //qDebug() << this->basePath();
  //QDir dir(this->currentPath);
  //qDebug() << currentPath;

  // create an instance of colormap helper class
  colormapFactory = new MSQColormapFactory();
  
  // create colormaps
  this->colormaps[0] = colormapFactory->createColormap(MSQ_LUT_GRAY, 256);
  this->colormaps[1] = colormapFactory->createColormap(MSQ_LUT_HUE, 256);
  this->colormaps[2] = colormapFactory->createColormap(MSQ_LUT_SATURATION, 256);
  this->colormaps[3] = colormapFactory->createColormap(MSQ_LUT_HOT, 256);
  this->colormaps[4] = 0;

  // set current colormap
  this->currentColormap = 0;

  // set current layer/colormap/opacity
  this->currentLayer = 0;
  this->layerColormap[0] = this->layerColormap[1] = 0;
  this->layerOpacity[0] = this->layerOpacity[1] = 0;

  // set current file name
  setCurrentFile("No source");
  setWindowTitle("DICOM Explorer");
  setUnifiedTitleAndToolBarOnMac(true);
}

/***********************************************************************************//**
 * Returns progress bar widget
 */
QProgressBar *MSQDicomExplorer::progressBar()
{
  return this->myProgressBar;
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::createActions()
{
  afileSource = new QAction(QIcon(":/images/toolbar_source.png"), tr("&Open Directory..."), this);
  //afileOpen->setShortcuts(QKeySequence::Open);
  afileSource->setStatusTip(tr("Scan a directory containing DICOM files"));
  afileSource->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
  afileSource->setIconVisibleInMenu(false);
  connect(afileSource, SIGNAL(triggered()), this, SLOT(fileOpenDir()));

 //this->saveAsValues = true;
  this->aoptionsSaveValues = new QAction(tr("Append Tag Values"), this);
  this->aoptionsSaveValues->setStatusTip(tr("Append tag values to filenames when exporting 2D Analyze files"));
  this->aoptionsSaveValues->setCheckable(true);
  this->aoptionsSaveValues->setChecked(true);

  exportMenu = new QMenu(tr("Export"), this);
  exportMenu->setEnabled(false);
  QAction *actionExportAs2DAnalyze = new QAction(tr("Multiple Analyze 2D..."), this);
  QObject::connect(actionExportAs2DAnalyze, SIGNAL(triggered()), this, SLOT(fileExportAs2D()));
  QAction *actionExportAs3DAnalyze = new QAction(tr("Single Analyze 3D..."), this);
  QObject::connect(actionExportAs3DAnalyze, SIGNAL(triggered()), this, SLOT(fileExportAs3D()));
  QAction *actionExportAs4DAnalyze = new QAction(tr("Single Analyze 4D..."), this);
  QObject::connect(actionExportAs4DAnalyze, SIGNAL(triggered()), this, SLOT(fileExportAs4D()));
  exportMenu->addAction(aoptionsSaveValues);
  exportMenu->addSeparator();
  exportMenu->addAction(actionExportAs2DAnalyze);
  exportMenu->addSeparator();
  exportMenu->addAction(actionExportAs3DAnalyze);
  exportMenu->addAction(actionExportAs4DAnalyze);

  afileExit = new QAction(tr("E&xit"), this);
  afileExit->setShortcuts(QKeySequence::Quit);
  afileExit->setStatusTip(tr("Exit DICOM Explorer"));
  connect(afileExit, SIGNAL(triggered()), this, SLOT(fileExit()));
 
  aeditCopyTo = new QAction(tr("Copy Selection To..."), this);
  aeditCopyTo->setStatusTip(tr("Copy selected DICOM files directory to folder"));
  connect(aeditCopyTo, SIGNAL(triggered()), this, SLOT(fileCopySelected()));
  aeditCopyTo->setEnabled(false);

  aeditDelete = new QAction(tr("Discard Unmarked"), this);
  aeditDelete->setStatusTip(tr("Discard unmarked DICOM files"));
  connect(aeditDelete, SIGNAL(triggered()), this, SLOT(fileFilter()));
  aeditDelete->setEnabled(false);

  aeditRestore = new QAction(tr("&Restore All"), this);
  aeditRestore->setStatusTip(tr("Restore original DICOM files"));
  aeditRestore->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  connect(aeditRestore, SIGNAL(triggered()), this, SLOT(fileRestore()));
  aeditRestore->setEnabled(false);

  this->viewHeader = false;
  this->aviewHeader = new QAction(QIcon(":/images/toolbar_tag.png"), tr("Show Tags"), this);
  //afileOpen->setShortcuts(QKeySequence::Open);
  this->aviewHeader->setStatusTip(tr("Show DICOM header"));
  this->aviewHeader->setIconVisibleInMenu(false);
  connect(this->aviewHeader, SIGNAL(triggered()), this, SLOT(viewShowHeader()));

  this->viewImage = true;
  this->aviewImage = new QAction(QIcon(":/images/toolbar_tag.png"), tr("Hide Image"), this);
  //afileOpen->setShortcuts(QKeySequence::Open);
  this->aviewImage->setStatusTip(tr("Hide DICOM image"));
  this->aviewImage->setIconVisibleInMenu(false);
  connect(this->aviewImage, SIGNAL(triggered()), this, SLOT(viewShowImage()));

  this->viewTools = false;
  this->aviewTools = new QAction(QIcon(":/images/toolbar_tools.png"), tr("Show Tools"), this);
  this->aviewTools->setStatusTip(tr("Show toolset"));
  this->aviewTools->setIconVisibleInMenu(false);
  connect(this->aviewTools, SIGNAL(triggered()), this, SLOT(viewShowTools()));

  //this->asortFiles = new QAction(QIcon(":/images/toolbar_sync.png"), tr("Sort Files"), this);
  //this->asortFiles->setStatusTip(tr("Sort DICOM files according to criteria"));
  //this->asortFiles->setEnabled(false);
  //connect(this->asortFiles, SIGNAL(triggered()), this, SLOT(fileSort()));

}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(afileSource);
  fileMenu->addSeparator();
  fileMenu->addMenu(exportMenu);
  //fileMenu->addAction(afileOpen);
  //fileMenu->addAction(afileImport);
  //fileMenu->addAction(aexportSlices);
  //fileMenu->addSeparator();
  //fileMenu->addAction(afileCompress);
  //fileMenu->addAction(afileSave);
  fileMenu->addSeparator();
  fileMenu->addAction(afileExit);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(aeditCopyTo);
  editMenu->addSeparator();
  editMenu->addAction(aeditDelete);
  editMenu->addAction(aeditRestore);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(aviewHeader);
  viewMenu->addAction(aviewImage);
  viewMenu->addSeparator();
  viewMenu->addAction(aviewTools);

  colormapMenu = menuBar()->addMenu(tr("&Colormap"));
  QActionGroup *colormapActionGroup = new QActionGroup(this);
  colormapActionGroup->setExclusive(true);
  QAction *grayColormap = new QAction(tr("Gray"), this);
  grayColormap->setCheckable(true);
  grayColormap->setChecked(true);
  colormapMenu->addAction(grayColormap);
  colormapActionGroup->addAction(grayColormap);
  QAction *hueColormap = new QAction(tr("Hue"), this);
  hueColormap->setCheckable(true);
  colormapMenu->addAction(hueColormap);
  colormapActionGroup->addAction(hueColormap);
  QAction *satColormap = new QAction(tr("Saturation"), this);
  satColormap->setCheckable(true);
  colormapMenu->addAction(satColormap);
  colormapActionGroup->addAction(satColormap);
  QAction *hotColormap = new QAction(tr("Hot"), this);
  hotColormap->setCheckable(true);
  colormapMenu->addAction(hotColormap);
  colormapActionGroup->addAction(hotColormap);
  colormapMenu->addSeparator();
  QAction *customColormap = new QAction(tr("Custom..."), this);
  colormapMenu->addAction(customColormap);
  connect(colormapMenu, SIGNAL(triggered(QAction*)), this, SLOT(selectColormap(QAction*)));
  //optionsMenu->addAction(aoptionsSaveValues);
  //toolsMenu = menuBar()->addMenu(tr("&Tools"));
  //toolsMenu->addAction(auseOrthogonalViewer);

  //menuBar()->addSeparator();

  //helpMenu = menuBar()->addMenu(tr("&Help"));
  //helpMenu->addAction(ahelpAbout);
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::createToolBars()
{
  fileToolBar = addToolBar(tr("File Toolbar"));
  fileToolBar->addAction(afileSource);
  fileToolBar->setIconSize(QSize(25, 25));
  fileToolBar->layout()->setMargin(0);
  fileToolBar->layout()->setSpacing(0);

  viewToolBar = addToolBar(tr("View Toolbar"));
  viewToolBar->addAction(aviewHeader);
  viewToolBar->addAction(aviewTools);
  viewToolBar->setIconSize(QSize(25, 25));
  viewToolBar->layout()->setMargin(0);
  viewToolBar->layout()->setSpacing(0);

  layerToolBar = addToolBar(tr("Layer Toolbar"));
  createLayerToolBar(layerToolBar);

  // Show text below icon
  fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  viewToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  //layerToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::createLayerToolBar(QToolBar *toolbar)
{
  QFont font11; 
  font11.setPointSize(11);

  // create layer combo box
  layerCombo = new QComboBox;
  layerCombo->addItem("Background");
  layerCombo->addItem("Foreground");
  layerCombo->setFont(font11);
  connect(layerCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(layerChanged(int)));

  QLabel *layerLabel = new QLabel("Layer");
  layerLabel->setAlignment(Qt::AlignCenter);
  layerLabel->setFont(font11);

  QVBoxLayout *layerLayout = new QVBoxLayout;
  layerLayout->addWidget(layerCombo);
  layerLayout->addWidget(layerLabel); 
  
  QWidget *layerWidget = new QWidget;
  layerWidget->setLayout(layerLayout);

  // create colormap combo box
  colormapCombo = new QComboBox;
  colormapCombo->setFont(font11);
  colormapCombo->addItem("Gray");
  colormapCombo->addItem("Hue");
  colormapCombo->addItem("Saturation");
  colormapCombo->addItem("Hot");
  connect(colormapCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(colormapChanged(int)));

  QLabel *colormapLabel = new QLabel("Colormap");
  colormapLabel->setAlignment(Qt::AlignCenter);
  colormapLabel->setFont(font11);

  QVBoxLayout *colormapLayout = new QVBoxLayout; 
  colormapLayout->addWidget(colormapCombo);
  colormapLayout->addWidget(colormapLabel);

  QWidget *colormapWidget = new QWidget;
  colormapWidget->setLayout(colormapLayout);

  // create opacity combo box
  opacityCombo = new QComboBox;
  opacityCombo->setFont(font11);
  opacityCombo->addItem("100%");
  opacityCombo->addItem("90%");
  opacityCombo->addItem("80%");
  opacityCombo->addItem("70%");
  opacityCombo->addItem("60%");
  opacityCombo->addItem("50%");
  opacityCombo->addItem("40%");
  opacityCombo->addItem("30%");
  opacityCombo->addItem("20%");
  opacityCombo->addItem("10%");
  connect(opacityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(opacityChanged(int)));

  QLabel *opacityLabel = new QLabel("Opacity");
  opacityLabel->setAlignment(Qt::AlignCenter);
  opacityLabel->setFont(font11);

  QVBoxLayout *opacityLayout = new QVBoxLayout;
  opacityLayout->addWidget(opacityCombo);
  opacityLayout->addWidget(opacityLabel); 

  QWidget *opacityWidget = new QWidget;
  opacityWidget->setLayout(opacityLayout);
 
  QWidget* spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  toolbar->addWidget(spacer);
  toolbar->addWidget(layerWidget);
  toolbar->addWidget(colormapWidget);
  toolbar->addWidget(opacityWidget);
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::createStatusBar()
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
void MSQDicomExplorer::layerChanged(int index)
{ 
  this->currentLayer = index;
  this->colormapCombo->setCurrentIndex(this->layerColormap[this->currentLayer]);
  this->opacityCombo->setCurrentIndex(this->layerOpacity[this->currentLayer]);
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::colormapChanged(int index)
{ 
  this->layerColormap[this->currentLayer] = index;
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::opacityChanged(int index)
{ 
  this->layerOpacity[this->currentLayer] = index;
}
/***********************************************************************************//**
 * 
 */
/*void MSQDicomExplorer::removeItem(QTreeWidgetItem *item)
{
  // my parent
  QTreeWidgetItem *parent = item->parent();
  
  // check number of children
  if (item->childCount() == 0) {

     printf("removing item %d\n",item->data(0, Qt::UserRole).toUInt());
     mFileList[item->data(0, Qt::UserRole).toUInt()]->enabled = false;

    // remove child
    if (parent != NULL) {
      parent->removeChild(item);
    } else {
      int index = this->dicomTree->indexOfTopLevelItem(item);
      (void)this->dicomTree->takeTopLevelItem(index);
    }
     
    // deallocate memory
    delete item;

    // if parent empty, need to remove it too
    //if (parent->childCount() == 0)
    //  removeItem(parent);

  } else {

    // more than one child
    while(item->childCount() > 0)
    {
      // remove child
      removeItem(item->child(0));

      // remove myself
      removeItem(item);
    }

  }

}*/

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::toggleItem(QTreeWidgetItem *item, bool enabled)
{
  initializing = true;

  if (item->childCount() == 0) {

    mFileList[item->data(0, Qt::UserRole).toUInt()]->enabled = enabled;
    item->setCheckState(0, enabled ? Qt::Checked : Qt::Unchecked);

  } else {

    item->setCheckState(0, enabled ? Qt::Checked : Qt::Unchecked);

    for(int i=0; i < item->childCount(); i++)
      toggleItem(item->child(i), enabled);

  }

  initializing = false;
}


/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::dicomSelectionChanged()
{
  QTreeWidgetItem *item = this->dicomTree->currentItem();
  
  // Allow only imports of entire series for now
  //this->importButton->setEnabled(
  //    item->childCount() > 0 && item->child(0)->childCount() == 0 ? true : false);

  // Allow only imports of entire series for now
  //this->exportButton->setEnabled(
  //    item->childCount() > 0);// && item->child(0)->childCount() == 0 ? true : false);

  // if not a single DICOM file, search down for the first it can find
  
  if (item != NULL) {

    while (item->childCount())
      item = item->child(0);

    //printf("entropy=%f\n",item->data(1, Qt::UserRole).toDouble());

    // set DICOM header viewer
    this->headerViewer->setInput(item->text(0));

    // set DICOM imagew viewer
    this->imageViewer->setInput(item->text(0));

    aeditRestore->setEnabled(true);
    //mExportButton->setEnabled(true);
    exportMenu->setEnabled(true);
    aeditCopyTo->setEnabled(true);

  } else {

    aeditRestore->setEnabled(false);
    //mExportButton->setEnabled(false);
    exportMenu->setEnabled(false);
    aeditCopyTo->setEnabled(false);

  }

  //this->tagTree->resizeColumnToContents(0);
  //this->tagTree->resizeColumnToContents(1);
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::dicomItemChanged(QTreeWidgetItem * item, int column )
{
  if (!initializing)
  {
    if (item->childCount() > 0)
      toggleItem(item, item->checkState(0));
    else {
      if (item->checkState(0) > 0) {
        mFileList[item->data(0, Qt::UserRole).toUInt()]->enabled = true;
        //item->setCheckState(0, Qt::Checked );
        //printf("true\n");
      }
      else {
        mFileList[item->data(0, Qt::UserRole).toUInt()]->enabled = false;
        //item->setCheckState(0, Qt::Unchecked );
        //printf("false\n");
      }
    }
  }
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::createInterface()
{
  // Create fonts
  QFont font11, font12, bold12, bold11;

  font11.setPointSize(11);
  font12.setPointSize(12);
  bold12.setPointSize(12);
  bold12.setBold(true);
  bold11.setPointSize(11);
  bold11.setBold(true);

  // create modal progress dialog
  mProgressDialog = new QProgressDialog(this);

  // create top layout
  //QHBoxLayout *topLayout = new QHBoxLayout;
  //topLayout->setContentsMargins(QMargins(4,4,4,4));

  //QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
  
  //this->totalLabel = new QLabel("Total files: 0, read: 0");
  //this->totalLabel->setMaximumHeight(40);
  //this->totalLabel->setContentsMargins(QMargins(10,0,0,0));
  //this->totalLabel->setFont(bold12);
  //this->totalLabel->setSizePolicy(sizePolicy);
  //topLayout->addWidget(this->totalLabel);
  //topLayout->addStretch();

  // create main splitter
  this->mainSplitter = new QSplitter(Qt::Horizontal);

  // create dicom file tree
  QStringList headerLabels;
  headerLabels << "Name" << "Value";
  this->dicomTree = new QTreeWidget;
  this->dicomTree->setColumnCount(4);
  this->dicomTree->setHeaderLabels(headerLabels);
  this->dicomTree->setAlternatingRowColors(true);
  // hide study and series Instance UID
  this->dicomTree->setColumnHidden(2, true);
  this->dicomTree->setColumnHidden(3, true);
  this->dicomTree->setSortingEnabled(false);
  //this->dicomTree->setColumnHidden(8, true);

  this->dicomTree->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->dicomTree->header()->setResizeMode(QHeaderView::ResizeToContents);
  this->dicomTree->setIconSize(QSize(20, 20));
  this->dicomTree->setSelectionMode(QAbstractItemView::ContiguousSelection);
  this->dicomTree->setSelectionBehavior(QTreeView::SelectRows);
  connect(this->dicomTree, SIGNAL(itemSelectionChanged()), this,
      SLOT(dicomSelectionChanged()));
  connect(this->dicomTree,SIGNAL(itemChanged(QTreeWidgetItem*, int)),
      SLOT(dicomItemChanged(QTreeWidgetItem*, int)));

  // create image sorter
  this->sortingControl = new MSQDicomImageSorter();
  connect(this->sortingControl, SIGNAL(sortButtonClicked()), this,
      SLOT(fileSort()));
  this->sortingControl->setEnabled(false);
  
    // create quality control
  this->qualityControl = new MSQDicomQualityControl();
  connect(this->qualityControl, SIGNAL(regionOfInterestChanged()), this,
      SLOT(dicomSelectionChanged()));
  this->qualityControl->setEnabled(false);

  // create bottom layout
  QHBoxLayout *bottomLayout = new QHBoxLayout;
  bottomLayout->setContentsMargins(QMargins(0,8,0,0));
  bottomLayout->setSpacing(10);
  //bottomLayout->addWidget(mRestoreButton);
  //bottomLayout->addWidget(mFilterButton);
  //bottomLayout->addWidget(mSortButton);
  //bottomLayout->addStretch();

  // create toolbox
  //QVBoxLayout *toolSetLayout = new QVBoxLayout;
  //toolSetLayout->setContentsMargins(QMargins(8,0,0,0));

  this->toolSet = new QTabWidget;
  toolSet->setFont(bold11);

  // create center layout
  QVBoxLayout *centerLayout = new QVBoxLayout;
  centerLayout->setContentsMargins(QMargins(0,0,0,0));
  centerLayout->addWidget(this->dicomTree, 10);
  centerLayout->addWidget(toolSet);
  //centerLayout->addWidget(toolSet);
 
  QWidget *leftWindow = new QWidget;
  leftWindow->setLayout(centerLayout);

  // add center layout to splitter
  this->mainSplitter->addWidget(leftWindow);

  // create side splitter
  this->leftSplitter = new QSplitter(Qt::Vertical);

  // create image viewer
  this->imageViewer = new MSQDicomImageViewer();
  leftSplitter->addWidget(this->imageViewer);

  // create header viewer
  this->headerViewer = new MSQDicomHeaderViewer();
  leftSplitter->addWidget(this->headerViewer);

  // define default splitter subdivision size
  QList<int> list_size;
  list_size.push_back(100);
  list_size.push_back(100);
  //leftSplitter->setStretchFactor(0, 1);
  leftSplitter->setSizes(list_size);

  // for maximizing image view initially
  this->leftSplitterSize = this->leftSplitter->sizes();
  QList<int> minimized;
  minimized << this->leftSplitterSize[1] << 0;
  this->leftSplitter->setSizes(minimized);
  this->leftSplitter->handle(1)->setEnabled(false);

  // add tag tree to splitter
  this->mainSplitter->addWidget(this->leftSplitter);

  // create quality control
  this->qualityControl->setDicomViewer(this->imageViewer);

  // main layout
  QVBoxLayout *mainLayout = new QVBoxLayout;
  //mainLayout->setSpacing(1);
  mainLayout->setContentsMargins(QMargins(0,0,0,0));
  //mainLayout->addLayout(topLayout);
  mainLayout->addWidget(this->mainSplitter);
  mainLayout->addLayout(bottomLayout);

  toolSet->addTab(this->sortingControl, "Sorting");
  toolSet->addTab(this->qualityControl, "Quality");
  toolSet->hide();

  QWidget *mainWindow = new QWidget();
  mainWindow->setLayout(mainLayout);
  this->setCentralWidget(mainWindow);

  this->adjustSize();
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::selectColormap(QAction *action)
{
  if (!strcmp(action->text().toLocal8Bit().constData(), "Gray"))
  {
    this->currentColormap = 0;
  }
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Hue"))
  {
    this->currentColormap = 1;
  }
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Saturation"))
  {
    this->currentColormap = 2;
  }
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Hot"))
  {
    this->currentColormap = 3;
  } 
  else if (!strcmp(action->text().toLocal8Bit().constData(), "Custom..."))
  {

    QString colormapPath = "./../colormap";
    QString colormapName = QFileDialog::getOpenFileName(this, tr("Open colormap"), colormapPath);
    QFileInfo fi(colormapName);
    if (!colormapName.isEmpty())
    {
      vtkmsqLookupTable *custom_cmap;

      if (fi.suffix() == "lut")
        custom_cmap = colormapFactory->loadColormap(colormapName.toStdString());
      else if (fi.suffix() == "plist")
        custom_cmap = colormapFactory->loadPListColormap(colormapName.toStdString());
      else return;

      if (custom_cmap == NULL)
        this->warningMessage(tr("Error reading file %1.").arg(colormapName),
            tr("Make sure file is of correct type and retry."));
      else {
        this->currentColormap = MAX_COLORMAPS-1;
        if (this->colormaps[MAX_COLORMAPS-1] != 0)
          this->colormaps[MAX_COLORMAPS-1]->Delete();
        this->colormaps[MAX_COLORMAPS-1] = custom_cmap;
      }
    }
    
  }

  this->imageViewer->setColormap(this->colormaps[this->currentColormap]);
}


/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::setCurrentFile(const QString &fileName)
{
  this->currentFileName = fileName;
  //setWindowTitle(QString("DICOM Explorer: %1[*]").arg(fileName));
}

/***********************************************************************************//**
 * Update progress bar
 */
void MSQDicomExplorer::updateProgressBar(vtkObject *caller, unsigned long eventId,
    void *clientData, void* callData)
{
  int progress = static_cast<int> ( *(static_cast<double*> (callData) ) * 100.0);
  myProgressBar->setValue(progress);

  // make sure we process our own events
  QApplication::processEvents();
}

/***********************************************************************************//**
 * Update progress
 */
void MSQDicomExplorer::updateProgress(gdcm::Subject *caller, const gdcm::Event &evt)
{
  const gdcm::ProgressEvent &pe = dynamic_cast<const gdcm::ProgressEvent&>(evt);
  mProgressDialog->setValue((int)(pe.GetProgress() * 100));
  //printf("%f\n",pe.GetProgress());
  //QApplication::processEvents();
}

/***********************************************************************************//**
 * Update the status bar
 */
void MSQDicomExplorer::updateStatusBar(QString message, bool showProgressBar, int timeout)
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
 *
 */
void MSQDicomExplorer::warningMessage(const QString &text, const QString &info)
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
void MSQDicomExplorer::helpAbout()
{
  QMessageBox::about(this, tr("About MSQDicomExplorer"),
      tr("<b>MSQDicomExplorer</b> is a DICOM image exploration and conversion tool, part of the MedSquare suite."));
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileOpenDir()
{
  //openFileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
  QString dirName = QFileDialog::getExistingDirectory(this, tr("Select directory"),
      this->currentPath);
  //QString dirName = QFileDialog::getExistingDirectory(this, tr("Select directory"),
  //    tr("Dicom Explorer (*.crt))", this->currentPath);

  //QString dirName = "";

  if (!dirName.isEmpty())
  {
    // save current path
    this->currentPath = dirName;
    setCurrentFile(dirName);

    this->readDirectory(dirName);
    
    //pathLabel->setText(QString("Source: %1").arg(dirName));

    // make sure our dialog stays up
    //this->show();
    //this->raise();
    //this->activateWindow();

    // fetch data
    //this->setRootPath(dirName);
  }
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileCopySelectedRecursive(QTreeWidgetItem *item, bool selected, const QString& dirName)
{
  if (item->childCount() == 0) {
    if (selected || item->isSelected()) {
        QFileInfo fi(QDir(dirName), QFileInfo(item->text(0)).fileName());
        //printf("%s\n", fi.absoluteFilePath().toLocal8Bit().data());
        QFile::copy(item->text(0), fi.absoluteFilePath());
    }
  } else {
    if (item->isSelected())
      selected = true;    
      for(int i=0; i<item->childCount(); i++) {
        fileCopySelectedRecursive(item->child(i), selected, dirName);
      }
  }
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileCopySelected()
{
  //openFileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
  QString dirName = QFileDialog::getExistingDirectory(this, tr("Select directory"),
      this->currentPath);

  if (!dirName.isEmpty())
  {
    fileCopySelectedRecursive(
      this->dicomTree->invisibleRootItem(), 
      this->dicomTree->invisibleRootItem()->isSelected(), 
      dirName);
  }

}

/***********************************************************************************//**
 * 
 */
double MSQDicomExplorer::GetSliceSpacingFromDataset(const gdcm::DataSet& ds)
{
  // check whether slice spacing exists otherwise use slice thickness
  gdcm::Attribute<0x0018, 0x0088> sliceSpacingTag;
  gdcm::Attribute<0x0018, 0x0050> sliceThicknessTag;

  double slicespacing, slicethickness;

  if (ds.FindDataElement(gdcm::Tag(0x0018, 0x0088)))
  {
    // get slice spacing
    sliceSpacingTag.Set(ds);
    slicespacing = sliceSpacingTag.GetValue();
    // get slice thickness
    if (ds.FindDataElement(gdcm::Tag(0x0018, 0x0050)))
    {
      sliceThicknessTag.Set(ds);
      slicethickness = sliceThicknessTag.GetValue();
      // sanity check
      if (((slicespacing - slicethickness) / slicethickness) > 1.0)
        slicespacing = slicethickness;
    } else
      return slicespacing;
  }
  else
  {
    if (ds.FindDataElement(gdcm::Tag(0x0018, 0x0050)))
    {
      sliceThicknessTag.Set(ds);
      slicespacing = sliceThicknessTag.GetValue();
    }
    else
      slicespacing = 1;
  }
  return slicespacing;
}

/***********************************************************************************//**
 * 
 */
int MSQDicomExplorer::GetDominantOrientation(const double *dircos)
{
  double orientMatrix[3][3];

  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      orientMatrix[i][j] = dircos[i * 3 + j];
    }
  }

  vtkMath::Cross(orientMatrix[0], orientMatrix[1], orientMatrix[2]);

  double axialVector[] = { 0.0, 0.0, 1.0 };
  double coronalVector[] = { 0.0, 1.0, 0.0 };
  double sagittalVector[] = { 1.0, 0.0, 0.0 };

  double axialDot = fabs(vtkMath::Dot(orientMatrix[2], axialVector));
  double coronalDot = fabs(vtkMath::Dot(orientMatrix[2], coronalVector));
  double sagittalDot = fabs(vtkMath::Dot(orientMatrix[2], sagittalVector));

  QString orientLabel;

  if (axialDot > coronalDot)
  {
    if (axialDot > sagittalDot)
      return vtkMedicalImageProperties::AXIAL;
    else
      return vtkMedicalImageProperties::SAGITTAL;
  }
  else
  {
    if (coronalDot > sagittalDot)
      return vtkMedicalImageProperties::CORONAL;
    else
      return vtkMedicalImageProperties::SAGITTAL;
  }

  return vtkMedicalImageProperties::AXIAL;
}

/***********************************************************************************//**
 * Export DICOM
 */
bool MSQDicomExplorer::exportToAnalyze(const QStringList& fileNames, const QString& fileNameAnalyze, int components)
{
  // instantiate DICOM readers
  vtkSmartPointer<vtkmsqGDCMMoisacImageReader> imageReader = vtkSmartPointer<
      vtkmsqGDCMMoisacImageReader>::New();

  // assume uniform thickness, given in first slice
  gdcm::ImageReader reader;
  reader.SetFileName(fileNames.at(0).toLocal8Bit().constData());
  if (!reader.Read())
  {
    printf("Could not open %s for reading!", fileNames.at(0).toLocal8Bit().constData());
    //vtkErrorMacro( "ImageReader failed: " << filename);
    return 0;
  }
  gdcm::File &file = reader.GetFile();
  gdcm::DataSet &ds = file.GetDataSet();
  const double sliceSpacing = this->GetSliceSpacingFromDataset(ds);


  vtkStringArray *vtkFileNames = vtkStringArray::New();

  // convert file names
  foreach(QString file, fileNames)
  {
    vtkFileNames->InsertNextValue(file.toLocal8Bit().constData());
  }

  // for multiframe images, do not pass the array, otherwise
  // GDCM will not read.
  if (vtkFileNames->GetNumberOfValues() > 1)
    imageReader->SetFileNames(vtkFileNames);
  else
    imageReader->SetFileName(vtkFileNames->GetValue(0));
  imageReader->Update();

  const vtkFloatingPointType *spacing = imageReader->GetOutput()->GetSpacing();

  vtkSmartPointer<vtkImageChangeInformation> newInfo = vtkSmartPointer<
      vtkImageChangeInformation>::New();
  newInfo->SetInput(imageReader->GetOutput());
  newInfo->SetOutputSpacing(spacing[0], spacing[1], sliceSpacing);
  newInfo->Update();

  vtkmsqImageInterleaving *inter = vtkmsqImageInterleaving::New();
  inter->SetInput(newInfo->GetOutput());
  inter->SetNumberOfFrames(components);
  inter->Update();

  //vtkImageData *newImage = vtkImageData::New();
 
  //int dims[3];
  //imageReader->GetOutput()->GetDimensions(dims);
  //dims[2] = dims[2] / components;
  //newImage->SetExtent(0, dims[0]-1, 0, dims[1]-1, 0, dims[2]-1);
  //newImage->SetNumberOfScalarComponents(components);
  //newImage->ShallowCopy(newInfo->GetOutput());

  //vtkImageData *newImage = vtkImageData::New();

  //newImage->ShallowCopy(newInfo->GetOutput());

  //int dims[3];
  //newImage->GetDimensions(dims);
  //dims[2] = dims[2] / components;
  //newImage->SetNumberOfScalarComponents(components);
  //newImage->SetDimensions(dims);

  // update properties
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  // update orientation
  newProperties->SetOrientationType(
      this->GetDominantOrientation(newProperties->GetDirectionCosine()));

  // instantiate Analyze Writer
  vtkSmartPointer<vtkmsqAnalyzeWriter> imageWriter =
      vtkSmartPointer<vtkmsqAnalyzeWriter>::New();

  // can we actually write the file ?
  if (imageWriter->CanWriteFile(fileNameAnalyze.toLocal8Bit().constData()) == 0)
  {
    /* This solution for the non-canonical file name fails on Mac OS.
    // If CanWriteFile() returned 0, that means the file name isn't canonical.
    // Let's try appending .hdr to it.
    QString temporary_file_name;
    temporary_file_name = fileName;
    temporary_file_name += ".hdr";
    if (imageWriter->CanWriteFile(temporary_file_name.toLocal8Bit().constData()) == 0)
      {
  cout << "Unwriteable file!" << endl;
  return false;
      }
    else
      {
  fileName = temporary_file_name;
  cout << "Appending .hdr to file name." << endl;
      }
    */
    return false;
  }

  // write out Analyze image
  imageWriter->SetFileName(fileNameAnalyze.toLocal8Bit().constData());
  //imageWriter->SetInput(newImage);
  imageWriter->SetInput(inter->GetOutput());
  imageWriter->SetMedicalImageProperties(newProperties);
  imageWriter->SetCompression(0);
  imageWriter->Write();

  vtkFileNames->Delete();
  //newImage->Delete();
  newProperties->Delete();

  return true;
}

/***********************************************************************************//**
 * Export DICOM
 */
bool MSQDicomExplorer::exportToAnalyze(const QString& fileName, const QString& fileNameAnalyze)
{
  // instantiate DICOM readers
  vtkSmartPointer<vtkmsqGDCMMoisacImageReader> imageReader = vtkSmartPointer<
      vtkmsqGDCMMoisacImageReader>::New();

  gdcm::ImageReader reader;
  reader.SetFileName(fileName.toLocal8Bit().constData());
  if (!reader.Read())
  {
    printf("Could not open %s for reading!", fileName.toLocal8Bit().constData());
    //vtkErrorMacro( "ImageReader failed: " << filename);
    return 0;
  }
  //gdcm::Image &image = reader.GetImage();
  gdcm::File &file = reader.GetFile();
  gdcm::DataSet &ds = file.GetDataSet();

  //vtkStringArray *vtkFileNames = vtkStringArray::New();

  // convert file names
  //foreach(QString file, fileNames)
  //{
    //vtkFileNames->InsertNextValue(file.toLocal8Bit().constData());
  //}

  // for multiframe images, do not pass the array, otherwise
  // GDCM will not read.
  //if (vtkFileNames->GetNumberOfValues() > 1)
  //  imageReader->SetFileNames(vtkFileNames);
  //else
  imageReader->SetFileName(fileName.toLocal8Bit().constData());
  imageReader->SetFileLowerLeft(1);
  imageReader->Update();

  const double sliceSpacing = this->GetSliceSpacingFromDataset(ds);
  const vtkFloatingPointType *spacing = imageReader->GetOutput()->GetSpacing();

  vtkSmartPointer<vtkImageChangeInformation> newInfo = vtkSmartPointer<
      vtkImageChangeInformation>::New();
  newInfo->SetInput(imageReader->GetOutput());
  newInfo->SetOutputSpacing(spacing[0], spacing[1], sliceSpacing);
  newInfo->Update();

  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(newInfo->GetOutput());

  // update propertiesprintf
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  // update orientation
  newProperties->SetOrientationType(
      this->GetDominantOrientation(newProperties->GetDirectionCosine()));

  // instantiate Analyze Writer
  vtkSmartPointer<vtkmsqAnalyzeWriter> imageWriter =
      vtkSmartPointer<vtkmsqAnalyzeWriter>::New();

  // can we actually write the file ?
  if (imageWriter->CanWriteFile(fileNameAnalyze.toLocal8Bit().constData()) == 0)
  {
    cout << "Unwriteable file!" << endl;
    return false;
  }

  // write out Analyze image
  imageWriter->SetFileName(fileNameAnalyze.toLocal8Bit().constData());
  imageWriter->SetInput(newImage);
  imageWriter->SetMedicalImageProperties(newProperties);
  imageWriter->SetCompression(0);
  imageWriter->Write();

  //vtkFileNames->Delete();
  newImage->Delete();
  newProperties->Delete();

  return true;
}

/***********************************************************************************//**
 * 
 */
/*void MSQDicomExplorer::fileCheckQualityRecursive(QTreeWidgetItem *item, double topperc)
{
  if (item->childCount() > 0) {

    if (item->child(0)->childCount() > 0)
    {
      for(int i=0; i<item->childCount(); i++)
        fileCheckQualityRecursive(item->child(i), topperc);
    } else {

      std::vector<entropy_pair> vec;

      // fetch entropy values
      for(int i=0; i<item->childCount(); i++)
        vec.push_back(std::make_pair(item->child(i)->data(1, Qt::UserRole).toDouble(), i));

      // sort
      std::sort(vec.begin(), vec.end(), entropy_compare);

      // uncheck
      for(int i=0; i<item->childCount(); i++)
        if (i >= item->childCount()*topperc)
          item->child(vec[i].second)->setCheckState(0, Qt::Unchecked);
        else
          item->child(vec[i].second)->setCheckState(0, Qt::Checked);

      //for(int i=item->childCount()*topperc; i<item->childCount(); i++)
      //  item->child(vec[i].second)->setCheckState(0, Qt::Unchecked);
    }
  }
}*/

/***********************************************************************************//**
 * 
 */
//void MSQDicomExplorer::fileCheckQuality()
//{
  //fileCheckQualityRecursive(this->dicomTree->invisibleRootItem(), mQuality->text().toDouble() / 100.0);
//}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileExportToAnalyze(QString preffix, QTreeWidgetItem *item, long count)
{
  QVector<gdcm::Tag> tags = this->sortingControl->tags();
  QVector<int> orders = this->sortingControl->orders();
  QStringList descriptions = this->sortingControl->descriptions();
  QStringList aliases = this->sortingControl->aliases();
  QVector<bool> groups = this->sortingControl->groups();

  QString fileName = preffix + "/";
  QStringList key = item->text(2).split(QRegExp(","), QString::SkipEmptyParts);
  QStringList indx = item->text(3).split(QRegExp(","), QString::SkipEmptyParts);

  for(int i = 0; i < tags.size(); i++)
  {
    if (!groups.at(i))
      continue;
    if (this->aoptionsSaveValues->isChecked())
      fileName.append(QString("%1%2_").arg(aliases.at(i)).arg(key.at(i)));
    else
      fileName.append(QString("%1%2_").arg(aliases.at(i)).arg(indx.at(i)));
  }

  fileName.append(QString("%1.hdr").arg(count));

  this->exportToAnalyze(item->text(0).toLocal8Bit().data(), fileName);
  //printf("saving %s into %s\n", item->text(0).toLocal8Bit().data(), fileName.toLocal8Bit().data());
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileExport2DRecursive(QString preffix, QTreeWidgetItem *item, bool selected, long *count)
{
  if (item->childCount() == 0) {
    if (selected || item->isSelected()) {
      fileExportToAnalyze(preffix, item, *count);
      //printf("%ld: %s\n", *count, item->text(0).toLocal8Bit().data());
      *count = *count + 1;
    }
  } else {
    if (item->isSelected())
      selected = true;
    for(int i=0; i<item->childCount(); i++) {
        this->fileExport2DRecursive(preffix, item->child(i), selected, count);
        //printf("count=%ld\n",count);
    }
  }
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileExport3DRecursive(QStringList& fileNames, QTreeWidgetItem *item, bool selected, long *count)
{
  if (item->childCount() == 0) {
    if (selected || item->isSelected()) {
      fileNames.append(item->text(0));
      //printf("%ld: %s\n", *count, item->text(0).toLocal8Bit().data());
      //*count = *count + 1;
    }
  } else {
    if (item->isSelected())
      selected = true;
    for(int i=0; i<item->childCount(); i++) {
        this->fileExport3DRecursive(fileNames, item->child(i), selected, count);
        //printf("count=%ld\n",count);
    }
  }
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileExport4DRecursive(QStringList& fileNames, QTreeWidgetItem *item, bool selected, 
  long *count, int *comp)
{
  int total = 0;

  if (item->childCount() == 0) {
    if (selected || item->isSelected()) {
      fileNames.append(item->text(0));
      //printf("%ld: %s\n", *count, item->text(0).toLocal8Bit().data());
      *count = *count + 1;
    }
  } else {
    if (item->isSelected())
      selected = true;
    for(int i=0; i<item->childCount(); i++) {
        if (item->child(i)->childCount() != 0)
          total++;
        this->fileExport4DRecursive(fileNames, item->child(i), selected, count, comp);
        //printf("count=%ld\n",*count);
    }
    // if total == 0, only simple leaves
    if (total == 0 && selected)
      *comp = *comp + 1;
  }
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileExportAs3D()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export as 3D Analyze file"),
      currentFileName, tr("Analyze (*.hdr *.img)"), &currentFilter);

  this->fileCount = 0;
  QStringList selectedNames;

  if (!fileName.isEmpty())
  {
    fileExport3DRecursive(
      selectedNames,
      this->dicomTree->invisibleRootItem(), 
      this->dicomTree->invisibleRootItem()->isSelected(), 
      &this->fileCount);

    // printf("files=%ld\n", this->fileCount);

    // do appropriate export
    this->exportToAnalyze(selectedNames, fileName);

    //printf("List 3D created\n");
  }

}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileExportAs4D()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export as 4D Analyze file"),
      currentFileName, tr("Analyze (*.hdr *.img)"), &currentFilter);

  this->fileCount = 0;
  int components = 0;

  QStringList selectedNames;

  //printf("Initiating scan...\n");

  if (!fileName.isEmpty())
  {
    fileExport4DRecursive(
      selectedNames,
      this->dicomTree->invisibleRootItem(), 
      this->dicomTree->invisibleRootItem()->isSelected(), 
      &this->fileCount, &components);

    //printf("components=%d\n",components);

    if (components == 0)
      components = 1;

    //printf("files=%ld, components=%d, slices per volume=%f\n", this->fileCount, components, (float)this->fileCount/components);
    // do appropriate export
    this->exportToAnalyze(selectedNames, fileName, components);

    //printf("List 4D created\n");
  }

}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileExportAs2D()
{
  QString dirName = QFileDialog::getExistingDirectory(this, tr("Export as 2D Analyze files"),
    currentFileName);

  //    currentFileName, tr("Analyze (*.hdr *.img)"), &currentFilter);
  //QString fileName = QFileDialog::getSaveFileName(this, tr("Export as 2D Analyze files"),
  //    currentFileName, tr("Analyze (*.hdr *.img)"), &currentFilter);

  this->fileCount = 0;

  // In case a file was chosen try saving it
  if (!dirName.isEmpty())
  {
    fileExport2DRecursive(
      dirName,
      this->dicomTree->invisibleRootItem(), 
      this->dicomTree->invisibleRootItem()->isSelected(), 
      &this->fileCount);
  }
  
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::viewShowTools()
{
  if (this->viewTools) {
     // Hide tools
    this->aviewTools->setText(tr("Show Tools"));
    this->aviewTools->setStatusTip(tr("Show toolset"));
    this->toolSet->hide();
  } else {
    // Show tools
    this->aviewTools->setText(tr("Hide Tools"));
    this->aviewTools->setStatusTip(tr("Hide toolset"));
    this->toolSet->show();
  }
  this->viewTools = !this->viewTools;
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::viewShowHeader()
{
  if (this->viewHeader) {
    // Hide tags
    this->aviewHeader->setText(tr("Show Tags"));
    this->aviewHeader->setStatusTip(tr("Show DICOM tags"));
    if (this->viewImage) {
      this->leftSplitterSize = this->leftSplitter->sizes();
      QList<int> minimized;
      minimized << this->leftSplitterSize[0] << 0;//this->leftSplitterSize[1];
      this->leftSplitter->setSizes(minimized);
      this->leftSplitter->handle(1)->setEnabled(false);
    } else {
      // Image is hidden, enlarge file tree
      this->mainSplitterSize = this->mainSplitter->sizes();
      QList<int> maximized;
      maximized << this->mainSplitterSize[0] << 0;
      this->mainSplitter->setSizes(maximized);
      this->mainSplitter->handle(1)->setEnabled(false);
      this->leftSplitter->setSizes(this->leftSplitterSize);
    }
  } else {
    // Show tags
    this->aviewHeader->setText(tr("Hide Tags"));
    this->aviewHeader->setStatusTip(tr("Hide DICOM tags"));
    if (!this->viewImage) {
      this->leftSplitterSize = this->leftSplitter->sizes();
      QList<int> minimized;
      minimized << 0 << this->leftSplitterSize[1];//this->leftSplitterSize[1];
      this->leftSplitter->setSizes(minimized);
      this->mainSplitter->setSizes(this->mainSplitterSize);
      this->mainSplitter->handle(1)->setEnabled(true);
    } else {
      this->leftSplitter->setSizes(leftSplitterSize);
      this->leftSplitter->handle(1)->setEnabled(true);
    }
  }
  this->viewHeader = !this->viewHeader;
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::viewShowImage()
{
  if (this->viewImage) {
    // Hide image
    this->aviewImage->setText(tr("Show Image"));
    this->aviewImage->setStatusTip(tr("Show DICOM image"));
    if (this->viewHeader) {
      this->leftSplitterSize = this->leftSplitter->sizes();
      QList<int> minimized;
      minimized << 0 << this->leftSplitterSize[1];//this->leftSplitterSize[1];
      this->leftSplitter->setSizes(minimized);
      this->leftSplitter->handle(1)->setEnabled(false);
    } else {
      // Header is hidden, enlarge file tree
      this->mainSplitterSize = this->mainSplitter->sizes();
      QList<int> maximized;
      maximized << this->mainSplitterSize[0] << 0;
      this->mainSplitter->setSizes(maximized);
      this->mainSplitter->handle(1)->setEnabled(false);
      this->leftSplitter->setSizes(this->leftSplitterSize);
    }
  } else {
    // Show image
    this->aviewImage->setText(tr("Hide Image"));
    this->aviewImage->setStatusTip(tr("Hide DICOM image"));
    if (!this->viewHeader) {
      this->leftSplitterSize = this->leftSplitter->sizes();
      QList<int> minimized;
      minimized << this->leftSplitterSize[0] << 0;//this->leftSplitterSize[1];
      this->leftSplitter->setSizes(minimized);
      this->mainSplitter->setSizes(this->mainSplitterSize);
      this->mainSplitter->handle(1)->setEnabled(true);
    } else {
      this->leftSplitter->setSizes(leftSplitterSize);
      this->leftSplitter->handle(1)->setEnabled(true);
    }
  }
  this->viewImage = !this->viewImage;
}

/***********************************************************************************//**
 * Exit MSQDicomExplorer
 */
void MSQDicomExplorer::fileExit()
{
  qApp->exit();
}

/***********************************************************************************//**
 *
 */
int MSQDicomExplorer::countFiles(const QString &path)
{
  int suma = 0;

  QDir dir(path);
  dir.setFilter(QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot);
  if(!dir.exists()) {
    return 1;
  }
  
  QFileInfoList sList = dir.entryInfoList(QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot);
  foreach(QFileInfo ruta, sList)
  {
    if(ruta.isDir()) {
      suma += MSQDicomExplorer::countFiles(ruta.path() + "/" + ruta.completeBaseName()+ "/");
    }
    suma++;
  }

  return suma;
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileRestore()
{
  fileSort(true);
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileFilter()
{
  totalSorted = 0;

  // add tags
  QVector<gdcm::Tag> tags = this->sortingControl->tags();
  QVector<int> orders = this->sortingControl->orders();
  QStringList descriptions = this->sortingControl->descriptions();
  QStringList aliases = this->sortingControl->aliases();
  QVector<bool> groups = this->sortingControl->groups();

  // clear tree
  this->dicomTree->clear();

  int count = 0;
  unsigned int index = 0;
  mProgressDialog->setMinimum(0);
  mProgressDialog->setMaximum(mFileList.size());
  mProgressDialog->setValue(0);
  mProgressDialog->setWindowModality(Qt::WindowModal);
  mProgressDialog->setLabelText("Discarding unmarked files... Please wait");
  if (!mProgressDialog->isVisible())
    mProgressDialog->show();
  QApplication::processEvents();

  initializing = true;

  std::vector< gdcm::SmartPointer<MSQFileWithName> >::iterator it2;
  for(it2 = mFileList.begin(), index = 0; it2 != mFileList.end(); ++it2, ++index )
    {
      gdcm::SmartPointer<MSQFileWithName> const & f = *it2;

      if (f->enabled)
        addToDicomTree(f->filename, index, f->enabled, tags, descriptions, aliases, groups);

      if (++count % 100 == 0)
        mProgressDialog->setValue(count);
      //if (count % 50 == 0)
      //QApplication::processEvents();
    }

    mProgressDialog->hide();

    if (this->dicomTree->topLevelItemCount())
      this->dicomTree->setCurrentItem(this->dicomTree->topLevelItem(0)->child(0));
  
    initializing = false;

  // display totals
   setWindowTitle(QString("DICOM Explorer (%1 files inspected, %2 images read)[*]").arg(this->totalFiles).arg(this->totalSorted));
  //this->totalLabel->setText(QString("Total files: %1, read: %2").arg(this->totalFiles).arg(this->totalSorted));

    //sortFiles( mFilenames, tags.toStdVector(), orders.toStdVector());
}

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::fileSort(bool restore)
{
  totalSorted = 0;

  // add tags
  QVector<gdcm::Tag> tags = this->sortingControl->tags();
  QVector<int> orders = this->sortingControl->orders();
  QStringList descriptions = this->sortingControl->descriptions();
  QStringList aliases = this->sortingControl->aliases();
  QVector<bool> groups = this->sortingControl->groups();

  this->dicomTree->clear();

  //printf("mFileList=%d\n",mFileList.size());
  //if( mFilenames.empty() )
  //  {
  //    mFilenames.clear();
  //    return;// true;
  //  }

  //std::vector< gdcm::SmartPointer<MSQFileWithName> > filelist;
  //std::vector< gdcm::SmartPointer<gdcm::FileWithName> > filelist;
  //filelist.resize( mFilenames.size() );

  int count = 0;
  unsigned int index = 0;
  mProgressDialog->setMinimum(0);
  mProgressDialog->setMaximum(mFileList.size());
  mProgressDialog->setWindowModality(Qt::WindowModal);
  mProgressDialog->setLabelText("Sorting DICOM files... Please wait");
  if (!mProgressDialog->isVisible())
    mProgressDialog->show();
  QApplication::processEvents();

  std::vector<gdcm::Tag> const& std_tags = tags.toStdVector();
  std::vector<int> const& std_orders = orders.toStdVector();

  //printf("before sorting\n");

  SortFunctor sf(std_tags, std_orders);
  
  gdcm::SmartPointer<MSQFileWithName> const & f1 = *mFileList.begin();
  //printf("printing %s\n", f1->filename.c_str());

  if (tags.size() > 0)
    std::stable_sort( mFileList.begin(), mFileList.end(), sf );
  
  gdcm::SmartPointer<MSQFileWithName> const & f2 = *mFileList.begin();
  //printf("printing %s\n", f2->filename.c_str());

  count = 0;
  //mFilenames.clear(); // cleanup any previous call

  mProgressDialog->setMinimum(0);
  mProgressDialog->setMaximum(mFileList.size());
  //mProgressDialog->setWindowModality(Qt::WindowModal);
  mProgressDialog->setLabelText("Classifying and grouping DICOM files...");
  //mProgressDialog->setAutoClose(true);
  //mProgressDialog->show();
  //QApplication::processEvents();

  initializing = true;

  std::vector< gdcm::SmartPointer<MSQFileWithName> >::iterator it2;
  for(it2 = mFileList.begin(), index = 0; it2 != mFileList.end(); ++it2, ++index )
    {
      gdcm::SmartPointer<MSQFileWithName> const & f = *it2;
      //mFilenames.push_back( f->filename );

      //printf("adding %s\n", f->filename.c_str());
      //index = std::distance(mFileList.begin(), it2);
      if (restore)
        f->enabled = true;

      if (f->enabled)
        addToDicomTree(f->filename, index, f->enabled, tags, descriptions, aliases, groups);
      //printf("index=%ld",std::distance(filelist.begin(), it2));

      if (++count % 100 == 0)
        mProgressDialog->setValue(count);
      //if (count % 50 == 0)
      //QApplication::processEvents();
    }

    mProgressDialog->hide();
  
    initializing = false;

    if (this->dicomTree->topLevelItemCount())
      this->dicomTree->setCurrentItem(this->dicomTree->topLevelItem(0)->child(0));
    //this->dicomTree->setCurrentItem(this->dicomTree->topLevelItem(0)->child(0));
    //sortFiles( mFilenames, tags.toStdVector(), orders.toStdVector());

  // display totals
  setWindowTitle(QString("DICOM Explorer (%1 files inspected, %2 images read)[*]").arg(this->totalFiles).arg(this->totalSorted));

  //this->totalLabel->setText(QString("Total files: %1, read: %2").arg(this->totalFiles).arg(this->totalSorted));

  //}

  //gdcm::Directory::FilenamesType files = sorter.GetFilenames();
  //gdcm::Directory::FilenamesType::const_iterator file = mFilenames.begin();

  //for(int count = 0; file != mFilenames.end(); ++file, ++count)
 // {
 //     addToDicomTree(*file, tags, descriptions, groups);
 //     if (count % 50 == 0)
 //       QApplication::processEvents();
  //}
}

/***********************************************************************************//**
 * 
 */
/*bool MSQDicomExplorer::sortFiles(std::vector<std::string> const & filenames, 
    std::vector<gdcm::Tag> const& tags, std::vector<int> const& order)
{
  // BUG: I cannot clear Filenames since input filenames could also be the output of ourself...
  // Filenames.clear();
  if( filenames.empty() )
    {
    mFilenames.clear();
    return true;
    }

  std::vector< gdcm::SmartPointer<gdcm::FileWithName> > filelist;
  filelist.resize( filenames.size() );

  std::vector< gdcm::SmartPointer<gdcm::FileWithName> >::iterator it2 = filelist.begin();
  for( gdcm::Directory::FilenamesType::const_iterator it = filenames.begin();
    it != filenames.end() && it2 != filelist.end(); ++it, ++it2)
    {
      printf("%s\n", it->c_str());
      //if (!strcmp(it->c_str(), "<removed>"))
        //continue;
      gdcm::Reader reader;
      reader.SetFileName( it->c_str() );
      gdcm::SmartPointer<gdcm::FileWithName> &f = *it2;
      if( reader.Read() )
        {
        f = new gdcm::FileWithName( reader.GetFile() );
        f->filename = *it;
        }
      else
        {
        gdcmErrorMacro( "File could not be read: " << it->c_str() );
        return false;
        }
    }
  SortFunctor sf(tags, order);//, mProgressDialog);
  //sf = tag;
  //sf = order;
  std::stable_sort( filelist.begin(), filelist.end(), sf);

  mFilenames.clear(); // cleanup any previous call
  for(it2 = filelist.begin(); it2 != filelist.end(); ++it2 )
    {
    gdcm::SmartPointer<gdcm::FileWithName> const & f = *it2;
    mFilenames.push_back( f->filename );



    }

  return true;
}*/

/***********************************************************************************//**
 * 
 */
void MSQDicomExplorer::addToDicomTree(std::string fileName, unsigned int index, bool enabled,
  const QVector<gdcm::Tag> &tags, const QStringList &descriptions, const QStringList &aliases, const QVector<bool> &groups)
{
  // Message to see files added to tree
  //std::string msg = "Added file: ";
  //msg = msg + fileName;
  //cout << msg << std::endl;
  //progressLabel->setText(msg.c_str());
  double window = 256, center = 128;
  gdcm::Tag twindowcenter(0x0028, 0x1050);
  gdcm::Tag twindowwidth(0x0028, 0x1051);

  QFont font11;
  font11.setPointSize(11);

  QFont font;
  font.setPointSize(12);

  QFont bold;
  bold.setPointSize(12);
  bold.setBold(true);

  //std::set<std::string> acceptedFilenames;
  //acceptedFilenames.insert("CT");
  //acceptedFilenames.insert("MR");
  //acceptedFilenames.insert("NM");
  //acceptedFilenames.insert("PT");
  //acceptedFilenames.insert("ST");

  //gdcm::Reader reader;
  //reader.SetFileName( it->c_str() );
  gdcm::ImageReader reader;

  const gdcm::File &file = reader.GetFile();
  const gdcm::Image &gimage = reader.GetImage();
  reader.SetFileName(fileName.c_str());
  //printf("%s\n",fileName.c_str());
  if (!reader.Read()) {
    //this->warningMessage(QString("Could not read file %1").arg(QString::fromStdString(fileName)), "Pleack check and try again."); 
    // if it falls here, it is not a DICOM image.
    return;
  }

  //const gdcm::Image &image = reader.GetImage();
  const gdcm::DataSet &ds = file.GetDataSet();

  QTreeWidgetItem *topItem = dicomTree->invisibleRootItem();
  QTreeWidgetItem *temp;
  QString key;
  QString indices;

  // then need to dive in
  for(int i = 0; i < tags.size(); i++)
  {
    if (!groups.at(i))
      continue;

    std::string str = GetStringValueFromTag(tags.at(i), file);
    QString value = QString::fromStdString(str).replace(QChar('\\'), QString(" "), Qt::CaseInsensitive).simplified();
    //QString value = QString::fromStdString(str).replace(QRegExp("[`~!@#$%^&*()+=|:;<>«»,?/{}\'\"\\\[\\\]\\\\]"), 
    //  QString("_"));//, Qt::CaseInsensitive);

    bool found = false;
    for(int j = 0; j < topItem->childCount() && !found; j++)
      if (topItem->child(j)->text(1) == value) {
        topItem = topItem->child(j);
        key.append(topItem->text(1).replace(QRegExp("[ `~!@#$%^&*()+=|:;<>«»,?/{}\'\"\\\[\\\]\\\\]"), "_"));
        key.append(",");
        indices.append(QString("%1,").arg(j));
        found = true;
      }

    if (!found) {

      temp = new QTreeWidgetItem(topItem);

      //topItem->setText(0, QString("%1: %2").arg(descriptions.at(i)).arg(QString::fromStdString(str)));
      QString desc = descriptions.at(i);
      QString alias = aliases.at(i);
      //topItem->setText(0, desc.remove(QChar(' '), Qt::CaseInsensitive));
      temp->setText(0, desc.replace(QRegExp("\\([^\\(]*\\)"), ""));
      temp->setText(1, value);
      temp->setText(2, QString("%1").arg(i));
      key.append(temp->text(1).replace(QRegExp("[ `~!@#$%^&*()+=|:;<>«»,?/{}\'\"\\\[\\\]\\\\]"), "_"));
      key.append(",");
      indices.append(QString("%1,").arg(topItem->indexOfChild(temp)));
      temp->setFont(0, bold);
      temp->setFont(1, bold);

      temp->setFlags(temp->flags() | Qt::ItemIsUserCheckable);
      temp->setCheckState(0, Qt::Checked);

      topItem = temp;

    }       
  }

  //}

  //cout << fileName << std::endl;

  //printf("here with %s\n", fileName.c_str());
  QTreeWidgetItem *item = new QTreeWidgetItem(topItem);
  //item->setText(0, QString(fileName.c_str()));

  item->setText(0, QString::fromStdString(fileName));
  item->setFont(0, font11);
  item->setText(2, key.simplified());
  item->setText(3, indices.simplified());
  item->setData(0, Qt::UserRole, QVariant(index));
  //item->setData(1, Qt::UserRole, QVariant(entropy));
  item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
  item->setCheckState(0, enabled ? Qt::Checked : Qt::Unchecked);

  totalSorted++;
  
}

/***********************************************************************************//**
 *
 */
void MSQDicomExplorer::readDirectory(const QString& dirName)
{
  QFont font11;
  font11.setPointSize(11);
  int numFiles = 0;

  this->totalFiles = 0;

  //mProgressDialog->setMinimum(0);
  //mProgressDialog->setMaximum(countFiles(dirName));
  //mProgressDialog->setWindowModality(Qt::WindowModal);
  //mProgressDialog->setLabel(new QLabel("Reading DICOM files... Please wait"));
  //mProgressDialog->setAutoClose(true);
  //mProgressDialog->setAutoReset(true);
  //mProgressDialog->show();
  //numFiles = countFiles(dirName));
  //mProgressDialog->hide();

   // set up types for GDCM subject/observer mechanism
  typedef gdcm::MemberCommand<MSQDicomExplorer> gdcmCommandType;

  // define progress event command
  gdcm::SmartPointer<gdcmCommandType> progressEventCommand = gdcmCommandType::New();
  progressEventCommand->SetCallbackFunction(this, &MSQDicomExplorer::updateProgress);

   // read directory contents
  gdcm::Directory dir;
  const char *dname = dirName.toLocal8Bit().constData();
  unsigned int nfiles = dir.Load( dname, true );

  mProgressDialog->setMinimum(0);
  mProgressDialog->setMaximum(100);
  mProgressDialog->setWindowModality(Qt::WindowModal);
  mProgressDialog->setLabelText("Scanning directory for DICOM files... Please wait");
  //mProgressDialog->setAutoClose(true);
  //mProgressDialog->setAutoReset(true);
  mProgressDialog->show();
  //const gdcm::Directory::FilenamesType& files = dir.GetFilenames();
  //gdcm::Directory::FilenamesType::const_iterator file = files.begin();

  // scan only DICOM files
  gdcm::Scanner s;

  // add tags
  QVector<gdcm::Tag> tags = this->sortingControl->tags();
  QVector<int> orders = this->sortingControl->orders();
  QStringList descriptions = this->sortingControl->descriptions();

  for(int i = 0; i < tags.size(); i++)
      s.AddTag( tags.at(i) );

  // set up observer for progress event in GDCM scanner
  s.AddObserver(gdcm::ProgressEvent(), progressEventCommand);

  int count = 0;

  // scan it
  if ( s.Scan( dir.GetFilenames() ) )
  {
    // clear containers
    mFilenames.clear();
    mFileList.clear();

    // get files
    //mFilenames = s.GetFilenames();
    mFilenames = s.GetKeys();
    totalFiles = mFilenames.size();

    if (mFilenames.size() == 0) {
      
      mProgressDialog->setValue(100);
      mProgressDialog->hide();

      this->warningMessage("No DICOM files were found!", "Please check your source directory and try again.");

    } else {

      // resize container
      mFileList.resize( mFilenames.size() );

      std::vector< gdcm::SmartPointer<MSQFileWithName> >::iterator it2 = mFileList.begin();

      mProgressDialog->setMinimum(0);
      mProgressDialog->setMaximum(mFileList.size());
      mProgressDialog->setWindowModality(Qt::WindowModal);
      mProgressDialog->setLabelText("Reading DICOM files... Please wait");
      if (!mProgressDialog->isVisible())
        mProgressDialog->show();
      QApplication::processEvents();

      // copy files to final container
      for( gdcm::Directory::FilenamesType::const_iterator it = mFilenames.begin();
        it != mFilenames.end() && it2 != mFileList.end(); ++it, ++it2)
      {
        gdcm::Reader reader;
        reader.SetFileName( it->c_str() );

        if (++count % 100 == 0)
          mProgressDialog->setValue(count);

        if (mProgressDialog->wasCanceled())
        {
          reset();
          return;
        }

        gdcm::SmartPointer<MSQFileWithName> &f = *it2;
        if( reader.Read() )
        {
          f = new MSQFileWithName( reader.GetFile() );
          f->filename = *it;
        }
        else
        {
          gdcmErrorMacro( "File could not be read: " << it->c_str() );
          continue;// false;
        }
      }

      // now sort files with given criteria
      fileSort();

      //fileCheckQuality();

      //mSortButton->setEnabled(true);
      aeditRestore->setEnabled(true);
      aeditDelete->setEnabled(true);
      this->sortingControl->setEnabled(true);
      this->qualityControl->setEnabled(true);
      this->qualityControl->setDicomTree(dicomTree->invisibleRootItem());
      this->imageViewer->enableToolBar(true);

    }

  } else {

    //mSortButton->setEnabled(false);
    //asortFiles->setEnabled(false);
    aeditRestore->setEnabled(false);
    aeditDelete->setEnabled(false);
    this->sortingControl->setEnabled(false);
    this->qualityControl->setEnabled(false);
    this->imageViewer->enableToolBar(false);

  }

  // display totals
  setWindowTitle(QString("DICOM Explorer (%1 files inspected, %2 images read)[*]").arg(this->totalFiles).arg(this->totalSorted));

}

/***********************************************************************************//**
 *
 */
 void MSQDicomExplorer::reset()
 {
  this->totalFiles = 0;
  this->totalSorted = 0;

  // clear containers
  mFilenames.clear();
  mFileList.clear();

  this->dicomTree->clear();

  //mSortButton->setEnabled(false);
  //asortFiles->setEnabled(false);
  aeditRestore->setEnabled(false);
  aeditDelete->setEnabled(false);
  this->sortingControl->setEnabled(false);
  this->qualityControl->setEnabled(false);
  this->imageViewer->enableToolBar(false);

  // display totals
  //this->totalLabel->setText(QString("Total files: %1, read: %2").arg(this->totalFiles).arg(this->totalSorted));
  setWindowTitle(QString("DICOM Explorer (%1 files inspected, %2 images read)[*]").arg(this->totalFiles).arg(this->totalSorted));
 }

/***********************************************************************************//**
 *
 */
MSQDicomExplorer::~MSQDicomExplorer()
{
  //while(!this->imageList.isEmpty()){
	//this->imageList.removeAt(0);
 // }
}

//END OF FILE
