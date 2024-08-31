/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqPhilipsRECReader.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqPhilipsRECReader.h"

#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkByteSwap.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vtksys/SystemTools.hxx>
//#include <vtkstd/string>
#include <string>
#include <vtkzlib/zlib.h>

/** \cond 0 */
//vtkCxxRevisionMacro(vtkmsqPhilipsRECReader, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkmsqPhilipsRECReader);
/** \endcond */

/***********************************************************************************//**
 * 
 */
static std::string GetRECPARExtension(const std::string& filename)
{
  std::string fileExt(vtksys::SystemTools::GetFilenameLastExtension(filename));

  // If the last extension is .gz, then need to pull off 2 extensions.
  //.gz is the only valid compression extension.
  if (fileExt == std::string(".gz"))
  {
    fileExt = vtksys::SystemTools::GetFilenameLastExtension(
        vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
    fileExt += ".gz";
  }

  // Check that a valid extension was found
  // Will check for either all caps or all lower-case.
  // By default the Philips Pride Workstation outputs
  // the filenames as all caps, but a user may change the
  // filenames to lowercase.  This will allow one or the
  // other.  Mixed caps/lower-case will always (with the 
  // exception of the lower-case gz on the end which is 
  // always assumed to be lower-case) fail on an OS with
  // a case sensitive file system.
  if (fileExt != ".REC.gz" && fileExt != ".REC" && fileExt != ".PAR"
      && fileExt != ".rec.gz" && fileExt != ".rec" && fileExt != ".par")
  {
    return ("");
  }

  return (fileExt);
}

/***********************************************************************************//**
 * 
 */
static std::string GetRECPARRootName(const std::string& filename)
{
  const std::string fileExt = GetRECPARExtension(filename);

  // Create a base filename
  // i.e Image.PAR --> Image
  if (fileExt.length() > 0 && filename.length() > fileExt.length())
  {
    const std::string::size_type it = filename.find_last_of(fileExt);
    std::string baseName(filename, 0, it - (fileExt.length() - 1));
    return (baseName);
  }
  //Default to return same as input when the extension is nothing.
  return (filename);
}

/***********************************************************************************//**
 * 
 */
static std::string GetRECPARHeaderFileName(const std::string & filename)
{
  std::string ImageFileName(filename);
  const std::string fileExt = GetRECPARExtension(filename);
  // Accomodate either all caps or all lower-case filenames.
  if ((fileExt == ".REC") || (fileExt == ".REC.gz"))
  {
    ImageFileName = GetRECPARRootName(filename);
    ImageFileName += ".PAR";
  }
  else if ((fileExt == ".rec") || (fileExt == ".rec.gz"))
  {
    ImageFileName = GetRECPARRootName(filename);
    ImageFileName += ".par";
  }
  return (ImageFileName);
}

/***********************************************************************************//**
 * 
 */
static std::string GetRECPARImageFileName(const std::string& filename)
{
  std::string ImageFileName(filename);
  const std::string fileExt = GetRECPARExtension(filename);
  // Default to uncompressed .REC if .PAR is given as file name.
  if (fileExt == ".PAR")
  {
    ImageFileName = GetRECPARRootName(filename);
    ImageFileName += ".REC";
  }
  else if (fileExt == ".par")
  {
    ImageFileName = GetRECPARRootName(filename);
    ImageFileName += ".rec";
  }
  return (ImageFileName);
}

/***********************************************************************************//**
 * 
 */
vtkmsqPhilipsRECReader::vtkmsqPhilipsRECReader()
{
  this->SliceIndex = new SliceIndexType();
  this->MedicalImageProperties = vtkmsqMedicalImageProperties::New();
}

/***********************************************************************************//**
 * 
 */
vtkmsqPhilipsRECReader::~vtkmsqPhilipsRECReader()
{
  this->MedicalImageProperties->Delete();
  delete this->SliceIndex;
}

/***********************************************************************************//**
 * 
 */
int vtkmsqPhilipsRECReader::CanReadFile(const char* fname)
{
  std::string filename(fname);

  // we check that the correct extension is given by the user
  std::string filenameext = GetRECPARExtension(filename);
  if (filenameext != std::string(".PAR") && filenameext != std::string(".REC")
      && filenameext != std::string(".REC.gz") && filenameext != std::string(".par")
      && filenameext != std::string(".rec")
      && filenameext != std::string(".rec.gz"))
  {
    return 0;
  }

  const std::string HeaderFileName = GetRECPARHeaderFileName(filename);

  // Try to read the par file.
  struct msqpar_parameter par;

  // Zero out par_parameter.
  memset(&par, 0, sizeof(struct msqpar_parameter));

  vtkSmartPointer<vtkmsqPhilipsPAR> philipsPAR = vtkSmartPointer<vtkmsqPhilipsPAR>::New();

  (void) philipsPAR->ReadPAR(HeaderFileName, &par);

  // Check to see if there were any problems reading
  // the par file.
  if (par.problemreading)
  {
    return 0;
  }

  return 1;
}

/***********************************************************************************//**
 * 
 */
int vtkmsqPhilipsRECReader::GetSliceIndex(int index)
{
  if ((index < 0) || (index > (int) (this->SliceIndex->size() - 1)))
  {
    return -1;
  }
  return (*this->SliceIndex)[index];
}

/***********************************************************************************//**
 * This generates the correct offset to the desired image type and
 * scanning sequence (randomly ordered in the REC).
 */
int vtkmsqPhilipsRECReader::GetImageTypeOffset(int imageType, int scanSequence,
    int volumeIndex, int slice, int numSlices, struct msqpar_parameter parParam,
    std::vector<std::pair<int, int> > sliceImageTypesIndex,
    std::vector<std::pair<int, int> > sliceScanSequenceIndex)
{
  int index = volumeIndex * parParam.num_slice_repetitions * numSlices
      + slice * parParam.num_slice_repetitions;
  int i;
  for (i = 0; i < parParam.num_slice_repetitions; i++)
  {
    if ((sliceImageTypesIndex[index + i].second == imageType)
        && (sliceScanSequenceIndex[index + i].second == scanSequence))
    {
      break;
    }
  }
  return i;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqPhilipsRECReader::SetupSliceIndex(
    vtkmsqPhilipsRECReader::SliceIndexType *indexMatrix, int sortBlock,
    struct msqpar_parameter parParam,
    std::vector<std::pair<int, int> > imageTypesScanSequenceIndex,
    std::vector<std::pair<int, int> > sliceImageTypesIndex,
    std::vector<std::pair<int, int> > sliceScanSequenceIndex)
{
  int index = 0;
  int actualSlices = parParam.slice;
  int remainingVolumes = parParam.image_blocks / parParam.num_slice_repetitions;

  if (indexMatrix->size()
      != (vtkmsqPhilipsRECReader::SliceIndexType::size_type) parParam.dim[2])
  {
    vtkErrorMacro(
        "indexMatrix->size(): " << indexMatrix->size() << " != parParam.dim[2]: " << parParam.dim[2]);
    return;
  }

  if (parParam.dim[2] != (parParam.slice * parParam.image_blocks))
  {
    vtkErrorMacro(
        "parParam.dim[2]: " << parParam.dim[2] << " != (parParam.slice*parParam.image_blocks): " << parParam.slice * parParam.image_blocks);
    return;
  }

  if (imageTypesScanSequenceIndex.size()
      != (vtkmsqPhilipsRECReader::SliceIndexType::size_type) parParam.num_slice_repetitions)
  {
    vtkErrorMacro(
        "imageTypesScanSequenceIndex.size(): " << imageTypesScanSequenceIndex.size() << " != parParam.num_slice_repetitions " << parParam.num_slice_repetitions);
    return;
  }

  // Different index depending on the desired slice sort and the REC
  // slice order.
  if ((sortBlock && parParam.slicessorted) || (!sortBlock && !parParam.slicessorted))
  {
    // No sorting nessecary for these cases.
    for (int i = 0; i < parParam.dim[2]; i++)
    {
      (*indexMatrix)[i] = i;
    }
  }
  // This case is the real problematic one.
  else if (sortBlock && !parParam.slicessorted && (parParam.num_slice_repetitions > 1))
  {
    // Ok, need to figure out where all of the images are located
    // using sliceImageTypesIndex and sliceScanSequenceIndex.
    for (int i = 0; i < parParam.num_slice_repetitions; i++)
    {
      for (int j = 0; j < remainingVolumes; j++)
      {
        for (int k = 0; k < actualSlices; k++)
        {
          (*indexMatrix)[index] = j * parParam.num_slice_repetitions * actualSlices
              + k * parParam.num_slice_repetitions
              + this->GetImageTypeOffset(imageTypesScanSequenceIndex[i].first,
                  imageTypesScanSequenceIndex[i].second, j, k, actualSlices, parParam,
                  sliceImageTypesIndex, sliceScanSequenceIndex);
          index++;
        }
      }
    }
  }
  else
  {
    // Unsort image block or sort by image block.
    for (int i = 0; i < parParam.image_blocks; i++)
    {
      for (int j = 0; j < actualSlices; j++)
      {
        (*indexMatrix)[index] = j * parParam.image_blocks + i;
        index++;
      }
    }
  }
}

/***********************************************************************************//**
 * This method returns the largest data that can be generated.
 */
int vtkmsqPhilipsRECReader::RequestInformation(vtkInformation* request,
    vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  const std::string HeaderFileName = GetRECPARHeaderFileName(this->GetFileName());
  struct msqpar_parameter par;

  // Zero out par_parameter.
  memset(&par, 0, sizeof(struct msqpar_parameter));

  // Read PAR file.
  vtkSmartPointer<vtkmsqPhilipsPAR> philipsPAR = vtkSmartPointer<vtkmsqPhilipsPAR>::New();

  (void) philipsPAR->ReadPAR(HeaderFileName, &par);

  if (par.problemreading)
  {
    vtkErrorMacro("Problem reading PAR file");
    return 0;
  }

  // Setup the slice index matrix.
  this->SliceIndex->clear();
  this->SliceIndex->resize(par.dim[2]);

  std::vector<std::pair<int, int> > sliceImageTypesIndexes =
      philipsPAR->GetRECSliceIndexImageTypes(HeaderFileName);
  std::vector<std::pair<int, int> > sliceScanSequencesIndexes =
      philipsPAR->GetRECSliceIndexScanningSequence(HeaderFileName);
  std::vector<std::pair<int, int> > imageTypesScanSequencesIndexes =
      philipsPAR->GetImageTypesScanningSequence(HeaderFileName);

  this->SetupSliceIndex(this->SliceIndex, 1, par, imageTypesScanSequencesIndexes,
      sliceImageTypesIndexes, sliceScanSequencesIndexes);

  // As far as I know all Philips REC files are littleEndian.
  this->SetDataByteOrderToLittleEndian();

  // Set dimensions.
  unsigned int numberOfDimensions = 3;
  // In reality PAR/REC files can have 4 or more dimensions
  // but it is very difficult to sort out all of the
  // possibilities.  The reader will sort the images
  // by block and the different types of images
  // stored in the blocks may be determined using the 
  // MetaDataDictionary.
  this->SetFileDimensionality(numberOfDimensions);

  // As far as I know, Philips REC files are only
  // 8-bit or 16-bit signed integers.
  switch (par.bit)
  {
    case 8:
      this->SetDataScalarTypeToUnsignedChar();
      break;
    case 16:
      this->SetDataScalarTypeToShort();
      break;
    default:
      vtkErrorMacro(
          "Unknown data type. par.bit must be 8 or 16. " << "par.bit is " << par.bit);
      return 0;
  }

  // set up the dimensionality stuff
  this->SetDataOrigin(0, 0, 0);
  this->SetDataExtent(0, par.dim[0] - 1, 0, par.dim[1] - 1, 0, par.slice - 1);
  this->SetDataSpacing(par.vox[0], par.vox[1], par.vox[2]);
  this->SetNumberOfScalarComponents(par.image_blocks);

  switch (par.sliceorient)
  {
    case PAR_SLICE_ORIENTATION_TRANSVERSAL:
      // Transverse - the REC data appears to be stored as right-left, 
      // anterior-posterior, and inferior-superior.
      // Verified using a marker on right side of brain.
      this->MedicalImageProperties->SetOrientationType(vtkMedicalImageProperties::AXIAL);
      break;
    case PAR_SLICE_ORIENTATION_SAGITTAL:
      // Sagittal - the REC data appears to be stored as anterior-posterior, 
      // superior-inferior, and right-left.
      // Verified using marker on right side of brain.
      this->MedicalImageProperties->SetOrientationType(
          vtkMedicalImageProperties::SAGITTAL);
      break;
    case PAR_SLICE_ORIENTATION_CORONAL:
      // Coronal - the REC data appears to be stored as right-left, 
      // superior-inferior, and anterior-posterior.
      // Verified using marker on right side of brain.
      // fall thru
    default:
      this->MedicalImageProperties->SetOrientationType(
          vtkMedicalImageProperties::CORONAL);
  }

  // diffusion dataset, populate gradient directions
  if (par.diffusion == 1)
    {
      std::vector<std::vector<float> > gradientValues;
      std::vector<float> bValues;
      
      if (philipsPAR->GetDiffusionGradientOrientationAndBValues(HeaderFileName, &gradientValues, &bValues) == false)
	printf("Could not read diffusion sequence parameters.\n"); 
      
      // Set diffusion gradient and b values
      this->MedicalImageProperties->SetNumberOfGradientValues(gradientValues.size());
      this->MedicalImageProperties->SetNumberOfBValues(par.max_num_diff_vals);

      for(unsigned int i=0;i<gradientValues.size();i++)
	this->MedicalImageProperties->AddDiffusionGradient(gradientValues[i], bValues[i]);
    }

  // call father to finish up
  return this->Superclass::RequestInformation(request, inputVector, outputVector);
}

/***********************************************************************************//**
 * This function reads in one data of data.
 * templated to handle different data types.
 */
template<class OT>
void vtkmsqPhilipsRECReaderUpdate(vtkmsqPhilipsRECReader *self, vtkImageData *data,
    OT *outPtr, gzFile zfp)
{
  int outExtent[6];
  data->GetExtent(outExtent);

  unsigned int numberColumns = outExtent[1] - outExtent[0] + 1;
  unsigned int numberRows = outExtent[3] - outExtent[2] + 1;
  unsigned int numberSlices = outExtent[5] - outExtent[4] + 1;
  unsigned int numberComponents = data->GetNumberOfScalarComponents();

  // buffer for single slice of type OT
  unsigned int sliceSize = numberColumns * numberRows;
  unsigned int imageSliceSizeInBytes = sliceSize * sizeof(OT);
  OT *sliceBuffer = new OT[sliceSize];

  // increments
  unsigned long sliceIncr = 0;
  unsigned long bufferIncr = 0;
  unsigned long localIncr = 0;

  unsigned long sliceIndex = 0;

  // progress target
  unsigned long target = (unsigned long) ((numberSlices * numberComponents) / 25.0) + 1;
  unsigned long count = 0;

  // finally read image
  for (unsigned int comp = 0; comp < numberComponents && !self->AbortExecute; comp++)
  {
    // reset slice increment
    sliceIncr = 0;

    // read image a slice at a time (sorted).
    for (unsigned int slice = 0; slice < numberSlices; slice++)
    {
      gzread(zfp, sliceBuffer, imageSliceSizeInBytes);

      // swap bytes if necessary
      if (self->GetSwapBytes())
        vtkByteSwap::SwapVoidRange((void *) sliceBuffer, sliceSize, sizeof(OT));

      bufferIncr = 0;
      localIncr = 0;

      // copy slice contents into vtk image output
      while (bufferIncr < (unsigned) sliceSize)
      {
        outPtr[sliceIncr + localIncr + comp] = sliceBuffer[bufferIncr];
        localIncr = localIncr + numberComponents;
        bufferIncr++;
      }

      // update progress
      if (!(count % target))
      {
        self->UpdateProgress(count / (25.0 * target));
      }
      count++;

      // advance one slice
      sliceIncr += sliceSize * numberComponents;
      sliceIndex++;
    }
  }

  delete[] sliceBuffer;

}

/***********************************************************************************//**
 * 
 */
int vtkmsqPhilipsRECReader::RequestData(
  vtkInformation* request, 
  vtkInformationVector** vtkNotUsed(inputVector), 
  vtkInformationVector* outputVector)
{
  int outputPort = request->Get(vtkDemandDrivenPipeline::FROM_OUTPUT_PORT());
  if (outputPort > 1) {
    return 1;
  }
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  int extent[6];
  outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), extent);

  // get data object, allocate memory
  vtkImageData *data = static_cast<vtkImageData *>(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  this->AllocateOutputData(data, outInfo, extent);

  gzFile zfp;
  void *ptr;

  if (this->FileName && !this->FilePattern)
  {
    vtkErrorMacro(<< "Either a FileName or FilePattern must be specified.");
    return 0;
  }

  // open image for reading
  std::string imagefilename = GetRECPARImageFileName(this->FileName);

  // NOTE: gzFile operations act just like FILE * operations when the files
  // are not in gzip format.
  // This greatly simplifies the following code, and gzFile types are used
  // everywhere.
  if (!(zfp = gzopen(imagefilename.c_str(), "rb")))
  {
    imagefilename += ".gz";
    if (!(zfp = gzopen(imagefilename.c_str(), "rb")))
      return 0;
  }

  int *ext = data->GetExtent();
  //vtkDebugMacro(
  //    "Reading extent: " << ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]);

  data->GetPointData()->GetScalars()->SetName("PhilipsRECImage");

  // Call the correct templated function for the output
  ptr = data->GetScalarPointer();
  switch (this->GetDataScalarType())
  {
    vtkTemplateMacro(vtkmsqPhilipsRECReaderUpdate(this, data, (VTK_TT *)(ptr), zfp));
    default:
      vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
  }

  // close file
  gzclose(zfp);

  return 1;
}


/***********************************************************************************//**
 * 
 */
// void vtkmsqPhilipsRECReader::ExecuteData(vtkDataObject *output, vtkInformation *outInfo)
// {
//   vtkImageData *data = this->AllocateOutputData(output, outInfo);

//   gzFile zfp;
//   void *ptr;

//   if (this->FileName && !this->FilePattern)
//   {
//     vtkErrorMacro(<< "Either a FileName or FilePattern must be specified.");
//     return;
//   }

//   // open image for reading
//   std::string imagefilename = GetRECPARImageFileName(this->FileName);

//   // NOTE: gzFile operations act just like FILE * operations when the files
//   // are not in gzip format.
//   // This greatly simplifies the following code, and gzFile types are used
//   // everywhere.
//   if (!(zfp = gzopen(imagefilename.c_str(), "rb")))
//   {
//     imagefilename += ".gz";
//     if (!(zfp = gzopen(imagefilename.c_str(), "rb")))
//       return;
//   }

//   int *ext = data->GetExtent();
//   vtkDebugMacro(
//       "Reading extent: " << ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]);

//   data->GetPointData()->GetScalars()->SetName("PhilipsRECImage");

//   // Call the correct templated function for the output
//   ptr = data->GetScalarPointer();
//   switch (this->GetDataScalarType())
//   {
//     vtkTemplateMacro(vtkmsqPhilipsRECReaderUpdate(this, data, (VTK_TT *)(ptr), zfp));
//     default:
//       vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
//   }

//   // close file
//   gzclose(zfp);
// }

/***********************************************************************************//**
 * 
 */
void vtkmsqPhilipsRECReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

