//
// .NAME MSqImageIO - implement image reading and writing
// .SECTION Description
// MSqImageIO is a class that hides the details about reading and writing
// images into MedSquare.
//

#ifndef MSQ_IMAGE_IO_H
#define MSQ_IMAGE_IO_H

#include <QObject>

#include "MedSquare.h"

#include "vtkObject.h"
#include "vtkImageData.h"
#include "vtkMedicalImageProperties.h"
#include "vtkSmartPointer.h"

#include "MSQOpenRawDialog.h"
#include "vtkmsqRawHeader.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/stl/string>

class MSQImageIO: public QObject
{
Q_OBJECT

public:

  MSQImageIO(MedSquare *medSquare);

  int loadBruker2DSEQImage(const QString &fileName);
  int loadPhilipsRECPARImage(const QString &fileName);
  int loadNiftiImage(const QString &fileName);
  int loadDICOMImage(const QStringList &fileNames, const double sliceSpacing = 1.0);
  int loadAnalyzeImage(const QString &fileName);
  int loadRawImage(const QString &fileName);
  int loadMetaImage(const QString &fileName);

  bool saveAnalyzeImage(QString &fileName, vtkImageData *newImage,
      vtkMedicalImageProperties *newProperties, bool saveCompressed);

public slots:

  // callback to update progress bar
  void updateProgressBar(vtkObject *caller, unsigned long eventId, void *clientData,
      void* callData);
protected:
  int GetDominantOrientation(const double *dircos);

private:
  MedSquare *medSquare;
};

#endif
