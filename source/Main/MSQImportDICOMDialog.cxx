/*=========================================================================

 Program:   MedSquare
 Module:    MSQImportDICOMDialog.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQImportDICOMDialog.h"

#include "vtkmsqAnalyzeReader.h"
#include "vtkmsqAnalyzeWriter.h"
#include "vtkmsqPhilipsRECReader.h"
#include "vtkmsqBruker2DSEQReader.h"
#include "vtkmsqNiftiReader.h"
#include "vtkmsqRawReader.h"
#include "vtkmsqGDCMImageReader.h"
#include "vtkmsqGDCMMoisacImageReader.h"

#include "vtkMath.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkImageChangeInformation.h"
#include "vtkMetaImageReader.h"
#include "vtkMedicalImageProperties.h"

#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkImageAccumulate.h"
#include "vtkImageExtractComponents.h"

#include "gdcmSystem.h"
#include "gdcmDataElement.h"
#include "gdcmAttribute.h"
#include "gdcmDirectory.h"
#include "gdcmIPPSorter.h"
#include "gdcmImageReader.h"
#include "gdcmByteValue.h"
#include "gdcmSwapper.h"
#include "gdcmDirectionCosines.h"
#include "gdcmPrinter.h"
#include "gdcmCSAHeader.h"

#include <iostream>
#include <sstream>
#include <cmath>

/***********************************************************************************//**
 * 
 */
MSQImportDICOMDialog::MSQImportDICOMDialog(QWidget *parent) :
    QDialog(parent)
{
  this->buildDialog();
  this->fileCount = 0;

  // set current path
  this->currentPath = QDir::currentPath();
}

/***********************************************************************************//**
 * 
 */
MSQImportDICOMDialog::~MSQImportDICOMDialog()
{
}

/***********************************************************************************//**
 * 
 */
inline std::string MSQImportDICOMDialog::GetStringValueFromTag(const gdcm::Tag& t,
    const gdcm::DataSet& ds)
{
  if (ds.FindDataElement(t))
  {
    const gdcm::DataElement& de = ds.GetDataElement(t);
    const gdcm::ByteValue *bv = de.GetByteValue();

    if (bv) // Can be Type 2
    {
      return std::string(bv->GetPointer(), bv->GetLength());
    }
  }
  return std::string();
}

/***********************************************************************************//**
 * 
 */
inline double GetSliceSpacingFromDataset(const gdcm::DataSet& ds)
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
inline QString GetDominantOrientationString(const double *dircos)
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
  double orientDot;
  if (axialDot > coronalDot)
  {
    if (axialDot > sagittalDot)
    {
      orientLabel = "Axial";
      orientDot = axialDot;
    }
    else
    {
      orientLabel = "Sagittal";
      orientDot = sagittalDot;
    }
  }
  else
  {
    if (coronalDot > sagittalDot)
    {
      orientLabel = "Coronal";
      orientDot = coronalDot;
    }
    else
    {
      orientLabel = "Sagittal";
      orientDot = sagittalDot;
    }
  }

  if (1 - fabs(orientDot) > 1E-6)
    orientLabel.append(" oblique");
  return orientLabel;
}

/***********************************************************************************//**
 * 
 */
inline bool IsSiemensMosaicDataset(const gdcm::DataSet& ds)
{
  gdcm::CSAHeader csa;
  const gdcm::PrivateTag &t1 = csa.GetCSAImageHeaderInfoTag();
  if (!csa.LoadFromDataElement(ds.GetDataElement(t1)))
  {
    return false;
  }

  return (csa.FindCSAElementByName("NumberOfImagesInMosaic"));
}

/***********************************************************************************//**
 * 
 */
inline gdcm::Directory::FilenamesType GetAllFilenamesFromTagToValue(
    gdcm::Scanner const & s, gdcm::Directory::FilenamesType const &filesubset,
    gdcm::Tag const &t, const char *valueref)
{
  gdcm::Directory::FilenamesType theReturn;
  if (valueref)
  {
    size_t len = strlen(valueref);
    gdcm::Directory::FilenamesType::const_iterator file = filesubset.begin();
    for (; file != filesubset.end(); ++file)
    {
      const char *filename = file->c_str();
      const char * value = s.GetValue(filename, t);
      if (value && strncmp(value, valueref, len) == 0)
      {
        theReturn.push_back(filename);
      }
    }
  }
  return theReturn;
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::processAIOP(gdcm::Scanner const & s,
    gdcm::Directory::FilenamesType const & subset, const char *iopval)
{
  gdcm::IPPSorter ipp;
  ipp.SetComputeZSpacing(true);
  ipp.SetZSpacingTolerance(1e-3); // ??
  bool b = ipp.Sort(subset);
  if (!b)
  {
    // If you reach here this means you need one more parameter to discriminiat this
    // series. Eg. T1 / T2 intertwinted. Multiple Echo (0018,0081)
    std::cerr << "Failed to sort: " << subset.begin()->c_str() << std::endl;
    SortedFiles.push_back(subset);
    return;
  }

  SortedFiles.push_back(ipp.GetFilenames());

  QApplication::processEvents();
}

/***********************************************************************************//**
 * 
 */
bool MSQImportDICOMDialog::sortByAcquisition(gdcm::DataSet const & ds1, gdcm::DataSet const & ds2)
{
  gdcm::Attribute<0x0020,0x0012> at1;
  at1.Set( ds1 );
  gdcm::Attribute<0x0020,0x0012> at2;
  at2.Set( ds2 );
  return at1 < at2;
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::processAcquisition(gdcm::Scanner const & s,
    gdcm::Directory::FilenamesType const & subset, const char *acquid)
{
  gdcm::Sorter sorter;
  sorter.SetSortFunction(sortByAcquisition);
  sorter.StableSort( subset );
  SortedFiles.push_back(sorter.GetFilenames());
  QApplication::processEvents();
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::processAFrameOfRef(gdcm::Scanner const & s,
    gdcm::Directory::FilenamesType const & subset, const char * frameuid)
{
  const gdcm::Tag t2(0x0020, 0x0012); // Acquisition number
  const gdcm::Tag t3(0x0020, 0x0052); // Frame of Reference UID
  const gdcm::Tag t4(0x0020, 0x0037); // Image Orientation (Patient)

  // In this subset of files (belonging to same series), let's find those
  // belonging to the same Frame ref UID:
  gdcm::Directory::FilenamesType files = GetAllFilenamesFromTagToValue(s, subset, t3,
      frameuid);

  std::set<std::string> iopset;

  for (gdcm::Directory::FilenamesType::const_iterator file = files.begin();
      file != files.end(); ++file)
  {
    const char * value = s.GetValue(file->c_str(), t4);
    assert( value);
    iopset.insert(value);
  }
  size_t n = iopset.size();

  std::cout << "Sorted file: " << subset.begin()->c_str() << std::endl;

  if (n == 0)
  {
    std::cerr << "No frame of reference!" << endl;
    assert( files.empty());
    return;
  }

  if (n == 1)
  {
    this->processAIOP(s, files, iopset.begin()->c_str());
  }
  else
  {
    std::cout << "Multiple IOPs!" << endl;

    // If we reach here this means there is actually 'n' different IOP
    for (std::set<std::string>::const_iterator it = iopset.begin(); it != iopset.end();
        ++it)
    {
      const char *iopvalue = it->c_str();
      gdcm::Directory::FilenamesType iopfiles = GetAllFilenamesFromTagToValue(s, files,
          t4, iopvalue);
      this->processAIOP(s, iopfiles, iopvalue);

    }
  }
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::processAnOrientation(gdcm::Scanner const & s,
    gdcm::Directory::FilenamesType const & subset, const char * orientid)
{
  const gdcm::Tag t4(0x0020, 0x0037); // Image Orientation (Patient)
  const gdcm::Tag t3(0x0020, 0x0012); // Acquisition number

  // In this subset of files (belonging to same series), let's find those
  // belonging to the same Frame ref UID:
  gdcm::Directory::FilenamesType files = GetAllFilenamesFromTagToValue(s, subset, t3,
      orientid);

  this->processAIOP(s, files, "dummy");
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::processASeries(gdcm::Scanner const & s,
    gdcm::Directory::FilenamesType const & subset, const char * seriesuid)
{
  const gdcm::Tag t2(0x0020, 0x000e); // Series Instance UID
  const gdcm::Tag t3(0x0020, 0x0052); // Frame of Reference UID

  // let's find all files belonging to this series:
  gdcm::Directory::FilenamesType seriesfiles = GetAllFilenamesFromTagToValue(s, subset,
      t2, seriesuid);

  if (this->checkSortByAcq->checkState() == Qt::Checked) {
    
    this->processAcquisition(s, seriesfiles, "dummy");
  
  } else {

    gdcm::Scanner::ValuesType vt3 = s.GetValues(t3);

    // Message to see files added to tree
    //std::string msg = "Processing series: ";
    //msg = msg + std::string(seriesuid);
    //cout << msg << std::endl;
    //progressLabel->setText(msg.c_str());
    this->fileCount++;
    countLabel->setText(QString("Series read: %1").arg(this->fileCount));

    if (vt3.size() == 0)
    {
      this->processAIOP(s, seriesfiles, "dummy");
    }
    else
      for (gdcm::Scanner::ValuesType::const_iterator it = vt3.begin(); it != vt3.end();
          ++it)
      {
        this->processAFrameOfRef(s, seriesfiles, it->c_str());

        QApplication::processEvents();
      }

  }
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::processAStudy(gdcm::Scanner const & s, const char * studyuid)
{
  const gdcm::Tag t1(0x0020, 0x000d); // Study Instance UID
  const gdcm::Tag t2(0x0020, 0x000e); // Series Instance UID

  // let's find all files belonging to this study
  gdcm::Directory::FilenamesType studyfiles = GetAllFilenamesFromTagToValue(s,
      s.GetFilenames(), t1, studyuid);

  gdcm::Scanner::ValuesType vt2 = s.GetValues(t2);

  progressBar->setValue(0);

  for (gdcm::Scanner::ValuesType::const_iterator it = vt2.begin(); it != vt2.end(); ++it)
  {
    this->processASeries(s, studyfiles, it->c_str());

    QApplication::processEvents();
  }
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::processIntoVolumes(gdcm::Scanner const & s)
{
  const gdcm::Tag t1(0x0020, 0x000d); // Study Instance UID

  gdcm::Scanner::ValuesType vt1 = s.GetValues(t1);
  for (gdcm::Scanner::ValuesType::const_iterator it = vt1.begin(); it != vt1.end(); ++it)
  {
    this->processAStudy(s, it->c_str());
  }
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::addToDicomTree(std::string fileName)
{
  // Message to see files added to tree
  //std::string msg = "Added file: ";
  //msg = msg + fileName;
  //cout << msg << std::endl;
  //progressLabel->setText(msg.c_str());

  QFont font11;
  font11.setPointSize(11);

  QFont font;
  font.setPointSize(12);

  QFont bold;
  bold.setPointSize(12);
  bold.setBold(true);

  QTreeWidgetItem *patientItem, *seriesItem;

  std::set<std::string> acceptedFilenames;
  acceptedFilenames.insert("CT");
  acceptedFilenames.insert("MR");
  acceptedFilenames.insert("NM");
  acceptedFilenames.insert("PT");
  acceptedFilenames.insert("ST");

  gdcm::ImageReader reader;

  const gdcm::File &file = reader.GetFile();
  reader.SetFileName(fileName.c_str());
  if (!reader.Read())
    return;

  const gdcm::Image &image = reader.GetImage();
  const gdcm::DataSet &ds = file.GetDataSet();

  // get dimensions
  const unsigned int *dims = image.GetDimensions();

  // get spacing
  const double *spc = image.GetSpacing();

  // Get modality, and if not one from accepted modalities bail out
  std::string modality = GetStringValueFromTag(gdcm::Tag(0x0008, 0x0060), ds);
  if (acceptedFilenames.find(modality) == acceptedFilenames.end())
    return;

  std::string patient = GetStringValueFromTag(gdcm::Tag(0x0010, 0x0010), ds); // Get patient name
  std::string description = GetStringValueFromTag(gdcm::Tag(0x0008, 0x103e), ds); // Get series description
  std::string studyid = GetStringValueFromTag(gdcm::Tag(0x0020, 0x0010), ds); // Get study ID
  std::string studyinstanceuid = GetStringValueFromTag(gdcm::Tag(0x0020, 0x00d), ds); // Get study instance UID
  std::string seriesnumber = GetStringValueFromTag(gdcm::Tag(0x0020, 0x0011), ds); // Get series number
  std::string seriesinstanceuid = GetStringValueFromTag(gdcm::Tag(0x0020, 0x000e), ds); // Get series instance UID

  double slicespacing = GetSliceSpacingFromDataset(ds);

  // get direction cosines
  QString orientLabel = GetDominantOrientationString(image.GetDirectionCosines());

  // get additional details
  QString details;
  if (image.GetNumberOfDimensions() == 3)
    details.append("Multiframe");
  if (IsSiemensMosaicDataset(ds))
  {
    if (details.isEmpty()) // this is ugly, needs to be done nicely
      details.append("Mosaic");
    else
      details.append(", Moisac");
  }

  // handle null name studyid
  if (studyid.length() == 0)
    studyid = "0";

  QList<QTreeWidgetItem*> itemList = dicomTree->findItems(
      QString::fromStdString(studyinstanceuid), Qt::MatchFixedString, 7);
  if (itemList.empty())
  {
    patientItem = new QTreeWidgetItem(dicomTree);
    patientItem->setText(0, QString::fromStdString(patient));
    patientItem->setFont(0, bold);
    patientItem->setIcon(0, QIcon(":/images/user.png"));

    patientItem->setText(7, QString::fromStdString(studyinstanceuid));
  }
  else
  {
    patientItem = itemList.first();
  }

  // handle null name series
  if (seriesnumber.length() == 0)
    seriesnumber = "0";

  QList<QTreeWidgetItem*> seriesList = dicomTree->findItems(
      QString::fromStdString(seriesinstanceuid), Qt::MatchExactly | Qt::MatchRecursive,
      8);
  seriesItem = NULL;
  for (QList<QTreeWidgetItem*>::iterator sit = seriesList.begin();
      sit != seriesList.end(); sit++)
  {
    if ((*sit)->parent() == patientItem)
    {
      seriesItem = *sit;
      break;
    }
  }

  if (seriesItem == NULL)
  {
    seriesItem = new QTreeWidgetItem(patientItem);
    seriesItem->setText(0,
        description.length() ? QString::fromStdString(description) : QString("Unnamed"));
    seriesItem->setFont(0, bold);

    // append series number
    seriesItem->setText(1, QString::fromStdString(seriesnumber));
    seriesItem->setFont(1, font);

    // handle multiframe DICOM
    if (image.GetNumberOfDimensions() == 3)
      seriesItem->setText(2, QString("%1x%2x%3").arg(dims[0]).arg(dims[1]).arg(dims[2]));
    else
      seriesItem->setText(2, QString("%1x%2").arg(dims[0]).arg(dims[1]));
    seriesItem->setFont(2, font);

    // sets spacing
    if (spc)
      seriesItem->setText(3,
          QString("%1x%2").arg(spc[0], 0, 'g', 2).arg(spc[1], 0, 'g', 2));
    else
      seriesItem->setText(3, QString("1x1"));
    seriesItem->setFont(3, font);

    seriesItem->setText(4, QString("%1").arg(slicespacing, 0, 'g', 3));
    seriesItem->setFont(4, font);
    seriesItem->setText(5, orientLabel);
    seriesItem->setFont(5, font);

    seriesItem->setText(6, details);
    seriesItem->setFont(6, font);

    seriesItem->setText(8, QString::fromStdString(seriesinstanceuid));

  }
  else
  {

    // multiple orientations inside single series
    if (!seriesItem->text(5).contains(orientLabel))
      seriesItem->setText(5, QString("%1/%2").arg(seriesItem->text(5)).arg(orientLabel));
  }

  // Add file name to the tree
  QTreeWidgetItem *item = new QTreeWidgetItem(seriesItem);
  item->setText(0, QString::fromUtf8(fileName.c_str()));
  item->setText(1, QString::fromStdString(seriesnumber).trimmed());

  // handle multiframe DICOM
  if (image.GetNumberOfDimensions() == 3)
    item->setText(2, QString("%1x%2x%3").arg(dims[0]).arg(dims[1]).arg(dims[2]));
  else
    item->setText(2, QString("%1x%2").arg(dims[0]).arg(dims[1]));

  // slice spacing
  if (spc)
    item->setText(3, QString("%1x%2").arg(spc[0], 0, 'g', 2).arg(spc[1], 0, 'g', 2));
  else
    item->setText(3, QString("1x1"));

  item->setText(4, QString("%1").arg(slicespacing, 0, 'g', 3));
  item->setText(5, orientLabel);
  item->setText(6, details);

  for (int f = 0; f < 8; f++)
    item->setFont(f, font11);
}

/***********************************************************************************//**
 * Read in DICOM format image, return false in case it cannot be loaded.
 */
void MSQImportDICOMDialog::setRootPath(const QString &dirName)
{
  // clear dicom and tag trees
  this->dicomTree->clear();
  this->tagTree->clear();

  // begin fetch dicom data
  this->beginFetch();

  // fetch data
  this->fetchData(dirName);

  // end fetch dicom data
  this->endFetch();

  // expand top level items
  this->dicomTree->expandToDepth(0);
  this->dicomTree->setCurrentItem(this->dicomTree->topLevelItem(0)->child(0));
}

/***********************************************************************************//**
 * Signal begin reading
 */
void MSQImportDICOMDialog::beginFetch()
{
  this->progressBar->show();
  this->progressLabel->show();
  this->chooseButton->setEnabled(false);
  this->importButton->setEnabled(false);
  this->exportButton->setEnabled(false);
}

/***********************************************************************************//**
 * Signal end reading
 */
void MSQImportDICOMDialog::endFetch()
{
  this->chooseButton->setEnabled(true);
  this->importButton->setEnabled(true);
  this->exportButton->setEnabled(true);
  this->progressBar->hide();
  this->progressLabel->hide();
}

/***********************************************************************************//**
 * Update progress
 */
void MSQImportDICOMDialog::updateProgress(gdcm::Subject *caller, const gdcm::Event &evt)
{
  QApplication::processEvents();
}

/***********************************************************************************//**
 * Fetch DICOM files
 */
int MSQImportDICOMDialog::fetchData(const QString &dirName)
{
  const gdcm::Tag t1(0x0020, 0x000d); // Study Instance UID
  const gdcm::Tag t2(0x0020, 0x000e); // Series Instance UID
  const gdcm::Tag t4(0x0020, 0x0037); // Image Orientation (Patient)

  gdcm::Directory d;
  d.Load(dirName.toLocal8Bit().constData(), true); // recursive !

  // set up types for GDCM subject/observer mechanism
  typedef gdcm::MemberCommand<MSQImportDICOMDialog> gdcmCommandType;

  // define progress event command
  gdcm::SmartPointer<gdcmCommandType> progressEventCommand = gdcmCommandType::New();
  progressEventCommand->SetCallbackFunction(this, &MSQImportDICOMDialog::updateProgress);

  gdcm::Scanner s;

  // set up observer for progress event in GDCM scanner
  s.AddObserver(gdcm::ProgressEvent(), progressEventCommand);

  countLabel->setText("");
  this->fileCount = 0;
  progressLabel->setText("Reading files...");

  s.AddTag(t1);
  s.AddTag(t2);
  s.AddTag(t4);

  bool b = s.Scan(d.GetFilenames());
  if (!b)
  {
    std::cerr << "Scanner failed" << std::endl;
    return 1;
  }

  progressLabel->setText("Sorting...");

  // process volumes
  SortedFiles.clear();
  this->processIntoVolumes(s);

  if (SortedFiles.size() == 0)
  {
    QTreeWidgetItem *patientItem = new QTreeWidgetItem(this->dicomTree);
    patientItem->setText(0, "No DICOM dataset found!");

    return 0;
  }

  progressLabel->setText("Grouping...");

  // create tree widget from sorted files
  for (std::vector<gdcm::Directory::FilenamesType>::const_iterator it =
      SortedFiles.begin(); it != SortedFiles.end(); ++it)
  {
    for (gdcm::Directory::FilenamesType::const_iterator file = it->begin();
        file != it->end(); ++file)
    {
      addToDicomTree(*file);

      QApplication::processEvents();
    }
  }

  for (int i = 0; i < dicomTree->topLevelItemCount(); i++)
  {

    // get patient
    QTreeWidgetItem *patientItem = dicomTree->topLevelItem(i);
    int numSeries = patientItem->childCount();
    QString patientName = patientItem->text(0);

    // add number of series to patient name
    patientItem->setText(0, QString("%1 (%2 series)").arg(patientName).arg(numSeries));

    // for each children, set number of images
    for (int j = 0; j < patientItem->childCount(); j++)
    {

      QTreeWidgetItem *seriesItem = patientItem->child(j);
      int numFiles = seriesItem->childCount();

      if (seriesItem->text(2).count("x") == 1)
        seriesItem->setText(2, QString("%1x%2").arg(seriesItem->text(2)).arg(numFiles));

      QApplication::processEvents();

    }

    // Sort by series number
    patientItem->sortChildren(1, Qt::AscendingOrder);
  }

  return 0;

}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::dicomPopulateTagTree(QString fileName)
{
  QFont font;
  font.setPointSize(11);

  gdcm::ImageReader reader;
  reader.SetFileName(fileName.toLocal8Bit().constData());
  if (!reader.Read())
  {
    this->tagTree->clear();

    QTreeWidgetItem *item = new QTreeWidgetItem(this->tagTree);
    item->setText(0, QString("No tags to show!"));
    item->setFont(0, font);
    return;
  }

  std::ostringstream oss(std::ostringstream::out);

  // let's use GDCM's printer to retrieve tags appropriately
  gdcm::Printer printer;
  printer.SetFile(reader.GetFile());
  printer.Print(oss);

  // get tags as string list
  QStringList tagList = QString::fromStdString(oss.str()).split("\n");

  // clear tag tree
  this->tagTree->clear();

  QTreeWidgetItem *parentItem = this->tagTree->invisibleRootItem();

  // let's populate the tag tree
  QStringListIterator itemIterator(tagList);
  while (itemIterator.hasNext())
  {

    // get item
    const QString& item = itemIterator.next().trimmed();

    // handle meta information
    if (item == "# Dicom-Meta-Information-Header")
    {
      parentItem = new QTreeWidgetItem(this->tagTree);
      parentItem->setText(0, "Metadata");
      parentItem->setFont(0, font);

    }
    else if (item == "# Dicom-Data-Set")
    {
      parentItem = new QTreeWidgetItem(this->tagTree);
      parentItem->setText(0, "Dataset");
      parentItem->setFont(0, font);

    }
    else if (item.startsWith("("))
    {
      QString tag = item.left(11);
      QString name, value;

      // Only add to list if there is a description for the tag
      // otherwise is not worth the trouble.
      int index = item.lastIndexOf(QChar('#'));

      if (index > 0)
      {
        name = item.mid(index).mid(2);
        name = name.mid(name.indexOf(' ') + 1).trimmed();
        value = item.mid(15, index - 15).trimmed();

        QTreeWidgetItem *newItem = new QTreeWidgetItem(parentItem);
        newItem->setText(0, tag);
        newItem->setText(1, name);
        newItem->setText(2, value);
        newItem->setFont(0, font);
        newItem->setFont(1, font);
        newItem->setFont(2, font);
      }
    }
  }

  this->tagTree->expandToDepth(0);

}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::dicomSelectionChanged()
{
  QTreeWidgetItem *item = dicomTree->currentItem();

  // Allow only imports of entire series for now
  this->importButton->setEnabled(
      item->childCount() > 0 && item->child(0)->childCount() == 0 ? true : false);

  // Allow only imports of entire series for now
  this->exportButton->setEnabled(
      item->childCount() > 0);// && item->child(0)->childCount() == 0 ? true : false);

  // if not a single DICOM file, search down for the first it can find
  while (item->childCount())
    item = item->child(0);

  // get tags
  this->dicomPopulateTagTree(item->text(0));
  //resize columns in tags  : If this part of code is modificated it must be modificated in the "enableShowTags" too
  this->tagTree->resizeColumnToContents(0);
  this->tagTree->resizeColumnToContents(1);
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::enableShowTags()
{
  if (this->showTagsButton->text() == "Show Tags")
  {
    this->showTagsButton->setText("Hide Tags");

    //resize columns in tags
    // : If this part of code is modificated it must be modificated in the "dicomSelectionChanged" too
    this->tagTree->resizeColumnToContents(0);
    this->tagTree->resizeColumnToContents(1);

    this->tagTree->show();

  }
  else
  {
    this->showTagsButton->setText("Show Tags");
    this->tagTree->hide();
  }
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::dirChoose()
{
  QString dirName = QFileDialog::getExistingDirectory(this, tr("Select directory"),
      pathLabel->text());

  if (!dirName.isEmpty())
  {

    // set label
    pathLabel->setText(QString("Source: %1").arg(dirName));

    // make sure our dialog stays up
    this->show();
    this->raise();
    this->activateWindow();

    // fetch data
    this->setRootPath(dirName);
  }
}

/***********************************************************************************//**
 * Build widget components
 */
void MSQImportDICOMDialog::buildDialog()
{
  // Create fonts
  QFont font11, bold12, bold11;

  font11.setPointSize(11);
  bold12.setPointSize(12);
  bold12.setBold(true);
  bold11.setPointSize(11);
  bold11.setBold(true);

  // create labels
  QLabel *infoLabel =
      new QLabel(
          tr(
              "Please select a source directory to search for DICOM datasets, then select a series to import.\n"));
  infoLabel->setFont(font11);

  pathLabel = new QLabel(QString("Source: %1").arg(QDir::currentPath()));
  pathLabel->setFont(bold12);

  countLabel = new QLabel("");
  countLabel->setFont(font11);
  countLabel->setAlignment(Qt::AlignRight);

  // splitter
  QSplitter *splitter = new QSplitter(Qt::Vertical);

  // create dicom file tree
  QStringList headerLabels;
  headerLabels << "Dataset" << "Series" << "Dimensions" << "Spacing" << "Thickness"
      << "Orientation" << "Details" << "Study Instance UID" << "Series Instance UID";

  this->dicomTree = new QTreeWidget;
  this->dicomTree->setColumnCount(7);
  this->dicomTree->setHeaderLabels(headerLabels);
  this->dicomTree->setAlternatingRowColors(true);
  //hide the Study Instance UID:
  this->dicomTree->setColumnHidden(7, true);
  this->dicomTree->setColumnHidden(8, true);

  this->dicomTree->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->dicomTree->header()->setResizeMode(QHeaderView::ResizeToContents);

  this->dicomTree->setIconSize(QSize(20, 20));
  connect(this->dicomTree, SIGNAL(itemSelectionChanged()), this,
      SLOT(dicomSelectionChanged()));

  // add dicom tree to splitter
  splitter->addWidget(this->dicomTree);

  // create tag list
  QStringList tagLabels;
  tagLabels << "Tag" << "Name" << "Value";
  tagTree = new QTreeWidget;
  tagTree->setColumnCount(3);
  tagTree->setHeaderLabels(tagLabels);
  tagTree->setAlternatingRowColors(true);

  tagTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  tagTree->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

  tagTree->header()->setResizeMode(QHeaderView::ResizeToContents);
  tagTree->hide();

  // add tag tree to splitter
  splitter->addWidget(tagTree);

  // button box
  importButton = new QPushButton(tr("&Import"));
  importButton->setAutoDefault(true);
  importButton->setEnabled(false);
  connect(importButton, SIGNAL(clicked()), this, SLOT(import()));

  QPushButton *cancelButton = new QPushButton(tr("&Cancel"));

  chooseButton = new QPushButton("Source...");
  connect(chooseButton, SIGNAL(clicked()), this, SLOT(dirChoose()));

  showTagsButton = new QPushButton(tr("Show Tags"));
  connect(showTagsButton, SIGNAL(clicked()), this, SLOT(enableShowTags()));

  exportButton = new QPushButton(tr("Export"));
  exportButton->setEnabled(false);
  connect(exportButton, SIGNAL(clicked()), this, SLOT(exportSeries()));

  checkSortByAcq = new QCheckBox(tr("Sort series by acquisition number")); 
  checkSortByAcq->setFont(font11);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->addButton(importButton, QDialogButtonBox::AcceptRole);
  buttonBox->addButton(chooseButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(showTagsButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(exportButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  // create progress bar
  progressBar = new QProgressBar;
  progressBar->setMaximum(0);
  progressBar->setMinimum(0);
  progressBar->hide();

  progressLabel = new QLabel;
  progressLabel->setFont(font11);
  progressLabel->setText("Reading files...");
  progressLabel->hide();

  QHBoxLayout *progressLayout = new QHBoxLayout;
  progressLayout->addWidget(checkSortByAcq);
  progressLayout->addWidget(progressLabel);
  progressLayout->addWidget(progressBar);

  // main layout
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(infoLabel);
  mainLayout->addWidget(pathLabel);
  mainLayout->addWidget(countLabel);
  mainLayout->addWidget(splitter);
  mainLayout->addLayout(progressLayout);
  mainLayout->addWidget(buttonBox);

  // set layout
  this->setLayout(mainLayout);
  this->setMinimumSize(800, 480);
  this->setWindowTitle(tr("Import DICOM"));
}

/***********************************************************************************//**
 * Return selected files
 */
QStringList MSQImportDICOMDialog::selectedFiles()
{
  QStringList fileNames;

  // Determine selected series
  foreach( QTreeWidgetItem *item, this->dicomTree->selectedItems() )
  {
    for (int i = 0; i < item->childCount(); ++i)
    {
      fileNames.append(item->child(i)->text(0));
    }
  }

  return fileNames;
}

/***********************************************************************************//**
 * Returns selected series number
 */
QString MSQImportDICOMDialog::seriesNumber()
{
  QTreeWidgetItem *item = this->dicomTree->selectedItems().first();
  return item->text(1);
}

/***********************************************************************************//**
 * Returns selected series name
 */
QString MSQImportDICOMDialog::seriesName()
{
  QTreeWidgetItem *item = this->dicomTree->selectedItems().first();
  return item->text(0);
}

/***********************************************************************************//**
 * Returns selected series spacing
 */
double MSQImportDICOMDialog::sliceSpacing()
{
  QTreeWidgetItem *item = this->dicomTree->selectedItems().first();
  return item->text(4).toDouble();
}

/***********************************************************************************//**
 * 
 */
QString MSQImportDICOMDialog::details()
{
  QTreeWidgetItem *item = this->dicomTree->selectedItems().first();
  return item->text(6);
}

/***********************************************************************************//**
 * 
 */
int MSQImportDICOMDialog::GetDominantOrientation(const double *dircos)
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
int MSQImportDICOMDialog::exportToAnalyze(const QStringList& fileNames, const double sliceSpacing, const QString& fileName)
{
  // instantiate DICOM readers
  vtkSmartPointer<vtkmsqGDCMMoisacImageReader> imageReader = vtkSmartPointer<
      vtkmsqGDCMMoisacImageReader>::New();

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

  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(newInfo->GetOutput());

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
  if (imageWriter->CanWriteFile(fileName.toLocal8Bit().constData()) == 0)
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
  imageWriter->SetFileName(fileName.toLocal8Bit().constData());
  imageWriter->SetInput(newImage);
  imageWriter->SetMedicalImageProperties(newProperties);
  imageWriter->SetCompression(0);
  imageWriter->Write();

  vtkFileNames->Delete();
  newImage->Delete();
  newProperties->Delete();

  return 1;
}
/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::import()
{
  this->importButton->setDisabled(true);
  QString series = seriesName();
  QStringList selected = selectedFiles();
  double spacing = sliceSpacing();

  emit
  filesSelected(selected, series, spacing);

  QDialog::accept();
  this->importButton->setDisabled(false);
}

/***********************************************************************************//**
 * 
 */
void MSQImportDICOMDialog::exportSeries()
{
  this->importButton->setDisabled(true);

  // Get selected item
  QTreeWidgetItem *item = this->dicomTree->selectedItems().first();

  if (item->childCount() > 0 && item->child(0)->childCount() == 0) {
    
    // exporting a single series

    QStringList selected = selectedFiles();
    double spacing = sliceSpacing();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image as Analyze"),
        QFileInfo(selected[0]).path(), tr("Analyze (*.hdr *.img)"));
    if (!fileName.isEmpty())
    { 
      // do appropriate export
      this->exportToAnalyze(selected, spacing, fileName);
    }

  } else {

    // exporting multiple series

    int total = item->childCount();
    double spacing;
    QString series;

    for(int i=0; i<total; i++)
    {
      QStringList selected;
      QString fileName;

      selected.clear();

      // retrieve filenames for this series
      for(int j=0; j<item->child(i)->childCount(); j++) 
      {
        selected.append(item->child(i)->child(j)->text(0));
        //printf("%s\n",item->child(i)->child(j)->text(0).toLocal8Bit().data());
      }
    
      spacing = (item->child(i)->text(4)).toDouble(0);
      series = QString("S%1.hdr").arg(i+1);
      fileName = QFileInfo(selected[0]).absolutePath();
      fileName = QFileInfo(fileName).absolutePath() + "/" + series;

      // do appropriate export
      this->exportToAnalyze(selected, spacing, fileName);
    }

  }
  
  this->importButton->setDisabled(false);
}
