/*=========================================================================

 Program:   MedSquare
 Module:    MSQImageIO.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQImageIO.h"

#include <algorithm>
#include <iostream>

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

#include "gdcmSystem.h"
#include "gdcmDirectory.h"
#include "gdcmIPPSorter.h"

/***********************************************************************************//**
 * 
 */
MSQImageIO::MSQImageIO(MedSquare *medSquare) :
    medSquare(medSquare)
{
}

/***********************************************************************************//**
 * 
 */
void MSQImageIO::updateProgressBar(vtkObject *caller, unsigned long eventId,
    void *clientData, void* callData)
{
  int progress = static_cast<int> ( *( static_cast<double*> ( callData ) ) * 100.0 );
  medSquare->progressBar()->setValue(progress);

  // make sure we process our own events
  QApplication::processEvents();
}

/***********************************************************************************//**
 * 
 */
int MSQImageIO::GetDominantOrientation(const double *dircos)
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
 * Read in Bruker 2DSEQ format image
 * Return 0 if cannot read image
 * Return 1 if file image was read
 */
int MSQImageIO::loadBruker2DSEQImage(const QString &fileName)
{
  // instantiate reader
  vtkSmartPointer<vtkmsqBruker2DSEQReader> imageReader = vtkSmartPointer<
      vtkmsqBruker2DSEQReader>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageReader, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *,
              unsigned long, void *, void *)));

  // can we actually read the file ?
  if (imageReader->CanReadFile(fileName.toLocal8Bit().constData()) == 0)
  {
    return 0;
  }

  // update status bar information
  medSquare->updateStatusBar(tr("Reading Bruker image..."), true);

  // read in Bruker image
  imageReader->SetFileName(fileName.toLocal8Bit().constData());
  imageReader->UpdateWholeExtent();

  // update image
  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(imageReader->GetOutput());

  // update properties
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  int success = medSquare->updateImageAndProperties(newImage, newProperties);

  return success;
}

/***********************************************************************************//**
 * Read in Philips REC/PAR format image
 * Return 0 if cannot read image
 * Return 1 if file image was read
 */
int MSQImageIO::loadPhilipsRECPARImage(const QString &fileName)
{
  // instantiate reader
  vtkSmartPointer<vtkmsqPhilipsRECReader> imageReader = vtkSmartPointer<
      vtkmsqPhilipsRECReader>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageReader, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

  // can we actually read the file ?
  if (imageReader->CanReadFile(fileName.toLocal8Bit().constData()) == 0)
  {
    return 0;
  }

  // update status bar information
  medSquare->updateStatusBar(tr("Reading REC/PAR image..."), true);

  // read in Analyze image
  imageReader->SetFileName(fileName.toLocal8Bit().constData());
  imageReader->UpdateWholeExtent();

  // update image
  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(imageReader->GetOutput());

  // update properties
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  int success = medSquare->updateImageAndProperties(newImage, newProperties);

  return success;
}

/***********************************************************************************//**
 * Read in NIFTI format image
 * Return 0 if cannot read image
 * Return 1 if file image was read
 */
int MSQImageIO::loadNiftiImage(const QString &fileName)
{
  // instantiate reader
  vtkSmartPointer<vtkmsqNiftiReader> imageReader =
      vtkSmartPointer<vtkmsqNiftiReader>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageReader, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

  // can we actually read the file ?
  if (imageReader->CanReadFile(fileName.toLocal8Bit().constData()) == 0)
  {
    return 0;
  }

  // update status bar information
  medSquare->updateStatusBar(tr("Reading NIfTI image..."), true);

  // read in Analyze image
  imageReader->SetFileName(fileName.toLocal8Bit().constData());
  imageReader->LegacyAnalyze75ModeOn();
  imageReader->UpdateWholeExtent();

  // update image
  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(imageReader->GetOutput());

  // update properties
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  int success = medSquare->updateImageAndProperties(newImage, newProperties);

  return success;
}

/***********************************************************************************//**
 * Read in DICOM format image
 * Return 0 if cannot read image
 * Return 1 if file image was read
 */
int MSQImageIO::loadDICOMImage(const QStringList& fileNames, const double sliceSpacing)
{
  // instantiate readers
  vtkSmartPointer<vtkmsqGDCMMoisacImageReader> imageReader = vtkSmartPointer<
      vtkmsqGDCMMoisacImageReader>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageReader, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

  // show status message
  medSquare->updateStatusBar(tr("Reading DICOM image..."), true);

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

  //const vtkFloatingPointType *spacing = imageReader->GetOutput()->GetSpacing();
  const double *spacing = imageReader->GetOutput()->GetSpacing();

  vtkSmartPointer<vtkImageChangeInformation> newInfo = vtkSmartPointer<
      vtkImageChangeInformation>::New();
  newInfo->SetInputConnection(imageReader->GetOutputPort());
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

  int success = medSquare->updateImageAndProperties(newImage, newProperties);

  return success;
}

/***********************************************************************************//**
 * Read in Analyze format image
 * Return 0 if cannot read image
 * Return 1 if file image was read
 */
int MSQImageIO::loadMetaImage(const QString &fileName)
{
  // instantiate reader
  vtkSmartPointer<vtkMetaImageReader> imageReader =
      vtkSmartPointer<vtkMetaImageReader>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageReader, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

  // can we actually read the file ?
  if (imageReader->CanReadFile(fileName.toLocal8Bit().constData()) == 0)
  {
    return 0;
  }

  // update status bar information
  medSquare->updateStatusBar(tr("Reading MetaImage image..."), true);

  // read in Analyze image
  imageReader->SetFileName(fileName.toLocal8Bit().constData());
  imageReader->UpdateWholeExtent();

  // update image
  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(imageReader->GetOutput());

  // update properties
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->SetOrientationType(vtkmsqMedicalImageProperties::AXIAL);

  int success = medSquare->updateImageAndProperties(newImage, newProperties);

  return success;
}

/***********************************************************************************//**
 * Read in Analyze format image
 * Return 0 if cannot read image
 * Return 1 if file image was read
 */
int MSQImageIO::loadAnalyzeImage(const QString &fileName)
{
  // instantiate reader
  vtkSmartPointer<vtkmsqAnalyzeReader> imageReader =
      vtkSmartPointer<vtkmsqAnalyzeReader>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageReader, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

  // can we actually read the file ?
  if (imageReader->CanReadFile(fileName.toLocal8Bit().constData()) == 0)
  {
    return 0;
  }

  // update status bar information
  medSquare->updateStatusBar(tr("Reading Analyze image..."), true);

  // read in Analyze image
  imageReader->SetFileName(fileName.toLocal8Bit().constData());
  imageReader->UpdateWholeExtent();

  // update image
  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(imageReader->GetOutput());

  // update properties
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  int success = medSquare->updateImageAndProperties(newImage, newProperties);

  return success;
}

/***********************************************************************************//**
 * Read in raw format image
 * Return 0 if cannot read image
 * Return 1 if file image was read
 * Return 2 if second dialog is cancelled
 */
int MSQImageIO::loadRawImage(const QString &fileName)
{
  // instantiate header
  vtkSmartPointer<vtkmsqRawHeader> header;

  // instantiate reader
  vtkSmartPointer<vtkmsqRawReader> imageReader = vtkSmartPointer<vtkmsqRawReader>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageReader, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

  // load header information
  header = MSQOpenRawDialog::loadHeader(fileName);

  if (header == NULL)
  {
    return 2;
  }

  // can we actually read the file ?
  if (imageReader->CanReadFile(fileName.toLocal8Bit().constData()) == 0)
  {
    return 0;
  }

  // update status bar information
  medSquare->updateStatusBar(tr("Importing raw image..."), true);

  // read in raw image
  imageReader->SetFileName(fileName.toLocal8Bit().constData());
  imageReader->SetDataExtent(0, header->GetDimensions()[0] - 1, 0,
      header->GetDimensions()[1] - 1, 0, header->GetDimensions()[2] - 1);
  imageReader->SetDataSpacing(header->GetSpacing());
  imageReader->SetDataOrigin(header->GetOrigin());
  imageReader->SetNumberOfScalarComponents(header->GetVolume());
  imageReader->SetFileNameSliceOffset(header->GetOffset());
  if (header->GetLittleEndian())
    imageReader->SetDataByteOrderToLittleEndian();
  else
    imageReader->SetDataByteOrderToBigEndian();

  // Set image orientation
  switch (header->GetOrientation())
  {
    case RAWAXIAL:
      imageReader->SetOrientation(vtkMedicalImageProperties::AXIAL);
      break;
    case RAWCORONAL:
      imageReader->SetOrientation(vtkMedicalImageProperties::CORONAL);
      break;
    case RAWSAGITTAL:
      imageReader->SetOrientation(vtkMedicalImageProperties::SAGITTAL);
      break;
  }

  // Set iamge data type
  switch (header->GetType())
  {
    case FLOAT:
      imageReader->SetDataScalarTypeToFloat();
      break;
    case DOUBLE:
      imageReader->SetDataScalarTypeToDouble();
      break;
    case INT:
      imageReader->SetDataScalarTypeToInt();
      break;
    case UNSIGNEDINT:
      imageReader->SetDataScalarTypeToUnsignedInt();
      break;
    case SHORT:
      imageReader->SetDataScalarTypeToShort();
      break;
    case UNSIGNEDSHORT:
      imageReader->SetDataScalarTypeToUnsignedShort();
      break;
    case CHAR:
      imageReader->SetDataScalarTypeToChar();
      break;
    case UNSIGNEDCHAR:
      imageReader->SetDataScalarTypeToUnsignedChar();
      break;
    case SIGNEDCHAR:
      imageReader->SetDataScalarTypeToSignedChar();
      break;
  }

  imageReader->UpdateWholeExtent();

  // update image
  vtkImageData *newImage = vtkImageData::New();
  newImage->ShallowCopy(imageReader->GetOutput());

  // update properties
  vtkmsqMedicalImageProperties *newProperties = vtkmsqMedicalImageProperties::New();
  newProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  int success = medSquare->updateImageAndProperties(newImage, newProperties);

  return success;
}

/***********************************************************************************//**
 * Save Analyze format image, return false in case the image cannot be loaded.
 */
bool MSQImageIO::saveAnalyzeImage(QString &fileName, vtkImageData *newImage,
    vtkMedicalImageProperties *newProperties, bool saveCompressed)
{
  // instantiate reader
  vtkSmartPointer<vtkmsqAnalyzeWriter> imageWriter =
      vtkSmartPointer<vtkmsqAnalyzeWriter>::New();

  // instantiate connection between vtk and qt events
  vtkSmartPointer<vtkEventQtSlotConnect> connection = vtkSmartPointer<
      vtkEventQtSlotConnect>::New();

  // connect progress events to qt progress bar updates
  connection->Connect(imageWriter, vtkCommand::ProgressEvent, this,
      SLOT(updateProgressBar(vtkObject *, unsigned long, void *, void *)));

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

  // update status bar information
  medSquare->updateStatusBar(tr("Writing Analyze image..."), true);

  // write out Analyze image
  imageWriter->SetFileName(fileName.toLocal8Bit().constData());
  imageWriter->SetInputData(newImage);
  imageWriter->SetMedicalImageProperties(newProperties);
  imageWriter->SetCompression(int(saveCompressed));
  imageWriter->Write();

  return true;
}
