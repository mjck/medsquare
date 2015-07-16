#ifndef MSQ_DICOMIMPORT_DIALOG_H
#define MSQ_DICOMIMPORT_DIALOG_H

#include <QtGui/QDialog>
#include <QtGui>
#include <QAbstractScrollArea>
#include <QAbstractItemView>
#include <QtGui/QTreeWidget>

#include "vtkImageData.h"
#include "vtkMedicalImageProperties.h"

#include "gdcmSubject.h"
#include "gdcmCommand.h"
#include "gdcmEvent.h"
#include "gdcmSmartPointer.h"
#include "gdcmProgressEvent.h"
#include "gdcmScanner.h"
#include "gdcmDataElement.h"
#include "gdcmAttribute.h"

#include "MedSquare.h"
#include "MSQImageIO.h"

class MSQImportDICOMDialog: public QDialog
{
Q_OBJECT

public:
  MSQImportDICOMDialog(QWidget *parent = 0);
  ~MSQImportDICOMDialog();

  void setRootPath(const QString &dirName);

  QStringList selectedFiles();
  QString seriesName();
  QString seriesNumber();
  double sliceSpacing();
  QString details();

signals:
  void filesSelected(const QStringList& selected, const QString& seriesName,
      const double sliceSpacing);

private slots:
  void dirChoose();
  void enableShowTags();
  void dicomSelectionChanged();
  void import();
  void exportSeries();

private:
  QLabel *pathLabel;
  QLabel *countLabel;
  QTreeWidget *dicomTree;
  QTreeWidget *tagTree;
  QPushButton *chooseButton;
  QPushButton *importButton;
  QPushButton *exportButton;
  QPushButton *showTagsButton;
  QLabel *progressLabel;
  QProgressBar *progressBar;
  QString currentPath;
  QCheckBox *checkSortByAcq;

  int fileCount;

  // list of sorted DICOM files
  std::vector<gdcm::Directory::FilenamesType> SortedFiles;

  static bool sortByAcquisition(gdcm::DataSet const & ds1, gdcm::DataSet const & ds2);
  void processAcquisition(gdcm::Scanner const & s,
    gdcm::Directory::FilenamesType const & subset, const char *acquid);

  void updateProgress(gdcm::Subject *caller, const gdcm::Event &evt);
  std::string GetStringValueFromTag(const gdcm::Tag& t, const gdcm::DataSet& ds);
  void processIntoVolumes(gdcm::Scanner const & s);
  void processAStudy(gdcm::Scanner const & s, const char * studyuid);
  void processASeries(gdcm::Scanner const & s,
      gdcm::Directory::FilenamesType const & subset, const char * seriesuid);
  void processAnOrientation(gdcm::Scanner const & s,
      gdcm::Directory::FilenamesType const & subset, const char * orientid);
  void processAFrameOfRef(gdcm::Scanner const & s,
      gdcm::Directory::FilenamesType const & subset, const char * frameuid);
  void processAIOP(gdcm::Scanner const & s, gdcm::Directory::FilenamesType const & subset,
      const char *iopval);

  int GetDominantOrientation(const double *dircos);
  int exportToAnalyze(const QStringList& fileNames, const double sliceSpacing, const QString& fileName);

  void addToDicomTree(std::string fileName);
  void dicomPopulateTagTree(QString fileName);

  void beginFetch();
  int fetchData(const QString &dirName);
  void endFetch();

  void buildDialog();
};

#endif
