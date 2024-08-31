/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqNiftiReader.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqNiftiReader.h"

#include "vtkByteSwap.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vtkzlib/zlib.h>
#include <iostream>
#include <nifti1_io.h>
#include <vtksys/SystemTools.hxx>

// System endianess
// May have to go somewhere else later
#define MSQ_BIG_ENDIAN    0
#define MSQ_LITTLE_ENDIAN 1

/** \cond 0 */
//vtkCxxRevisionMacro(vtkmsqNiftiReader, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkmsqNiftiReader);
/** \endcond */

/***********************************************************************************//**
 * 
 */
static std::string GetNiftiExtension(const std::string& filename)
{
  std::string fileExt(vtksys::SystemTools::GetFilenameLastExtension(filename));

  // If the last extension is .gz, then need to pull off 2 extensions.
  //.gz is the only valid compression extension.
  if (fileExt == ".gz")
  {
    fileExt = vtksys::SystemTools::GetFilenameLastExtension(
        vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
    fileExt += ".gz";
  }

  // Check that a valid extension was found
  // Will check for either all caps or all lower-case.
  // Mixed caps/lower-case will always (with the
  // exception of the lower-case gz on the end which is
  // always assumed to be lower-case) fail on an OS with
  // a case sensitive file system.
  if (fileExt != ".IMG.gz" && fileExt != ".IMG" && fileExt != ".img.gz"
      && fileExt != ".img" && fileExt != ".NII.gz" && fileExt != ".NII"
      && fileExt != ".nii.gz" && fileExt != ".nii" && fileExt != ".hdr"
      && fileExt != ".HDR")
  {
    return ("");
  }

  return (fileExt);
}

/***********************************************************************************//**
 * 
 */
static std::string GetNiftiRootName(const std::string& filename)
{
  const std::string fileExt = GetNiftiExtension(filename);

  // Create a base filename
  // i.e Image.HDR --> Image
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
static std::string GetNiftiHeaderFileName(const std::string & filename)
{
  std::string HeaderFileName(filename);
  const std::string fileExt = GetNiftiExtension(filename);

  //    Accomodate either all caps or all lower-case filenames.
  if ((fileExt == ".IMG") || (fileExt == ".IMG.gz"))

  {
    HeaderFileName = GetNiftiRootName(filename);
    HeaderFileName += ".HDR";
  }
  else if ((fileExt == ".img") || (fileExt == ".img.gz"))
  {
    HeaderFileName = GetNiftiRootName(filename);
    HeaderFileName += ".hdr";
  }
  return (HeaderFileName);
}

/***********************************************************************************//**
 * 
 */
vtkmsqNiftiReader::vtkmsqNiftiReader()
{
  // find out byte endianess from header file
  this->AutoByteSwapping = 1;

  // Handle old Analyze 7.5 files
  this->LegacyAnalyze75Mode = 0;

  // Reset properties
  this->MedicalImageProperties = vtkmsqMedicalImageProperties::New();
}

/***********************************************************************************//**
 * 
 */
vtkmsqNiftiReader::~vtkmsqNiftiReader()
{
  this->MedicalImageProperties->Delete();
}

/***********************************************************************************//**
 * 
 */
int vtkmsqNiftiReader::CanReadFile(const char* fname)
{
  // is_nifti_file returns
  //       > 0 for a nifti file
  //      == 0 for an analyze file,
  //       < 0 for an error,
  // if the return test is >= 0, nifti will read analyze files
  const int image_FTYPE = is_nifti_file(fname);
  if (image_FTYPE > 0)
  {
    return 1;
  }
  else if (image_FTYPE == 0 && (this->GetLegacyAnalyze75Mode()))
  {
    return 3;
  }
  /* image_FTYPE < 0 */
  return 0;
}

/***********************************************************************************//**
 * 
 */
int vtkmsqNiftiReader::readNiftiData(vtkInformation* request,
    vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{

  this->nii_header = nifti_image_read(this->GetFileName(), false);
  if (this->nii_header == NULL)
  {
    vtkErrorMacro("Unable to open file " << this->GetFileName());
    return 0;
  }

  if (this->nii_header->byteorder == 1)
  {
    SetDataByteOrder(VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN);
  }
  else
  {
    SetDataByteOrder(VTK_FILE_BYTE_ORDER_BIG_ENDIAN);
  }

  // set image information
  this->SetFileDimensionality(this->nii_header->dim[0]);

  // set up image dimensions, spacing, components, etc
  this->SetDataOrigin(0, 0, 0);

  this->SetDataExtent(0, this->nii_header->dim[1] - 1, 0, this->nii_header->dim[2] - 1, 0,
      this->nii_header->dim[3] - 1);

  this->SetDataSpacing(this->nii_header->pixdim[1], this->nii_header->pixdim[2],
      this->nii_header->pixdim[3]);

  if (this->nii_header->dim[4] < 1)
  {
    this->nii_header->dim[4] = 1;
  }
  this->SetNumberOfScalarComponents(this->nii_header->dim[4]);

  //set up VTK data type
  switch (this->nii_header->datatype)
  {
    case DT_BINARY:
      this->SetDataScalarTypeToChar();
      break;
    case DT_INT8:
      this->SetDataScalarTypeToSignedChar();
      break;
    case DT_UINT8:
      this->SetDataScalarTypeToUnsignedChar();
      break;
    case DT_INT16:
      this->SetDataScalarTypeToShort();
      break;
    case DT_UINT16:
      this->SetDataScalarTypeToUnsignedShort();
      break;
    case DT_INT32:
      this->SetDataScalarTypeToInt();
      break;
    case DT_UINT32:
      this->SetDataScalarTypeToUnsignedInt();
      break;
    case DT_FLOAT32:
      this->SetDataScalarTypeToFloat();
      break;
    case DT_FLOAT64:
      this->SetDataScalarTypeToDouble();
      break;
    case DT_RGBA32:
      this->SetDataScalarTypeToUnsignedChar();
      this->SetNumberOfScalarComponents(3);
      break;
    default:
      this->SetDataScalarTypeToShort();
  };

  // ************************************************
  // ADD ANALYZE ORIENTANTION CODE TO PROPERTIES
  // ************************************************
  this->MedicalImageProperties->SetOrientationType(vtkMedicalImageProperties::AXIAL);

  nifti_image_free(this->nii_header);

  // call father to finish up
  return this->Superclass::RequestInformation(request, inputVector, outputVector);
}

/***********************************************************************************//**
 * This method returns the largest data that can be generated.
 */
int vtkmsqNiftiReader::RequestInformation(vtkInformation* request,
    vtkInformationVector** inputVector, vtkInformationVector* outputVector)

{
  std::string headerfilename = GetNiftiHeaderFileName(this->FileName);

  this->nii_header = nifti_image_read(headerfilename.c_str(), false);
  if (this->nii_header != NULL)
  {
    nifti_image_free(this->nii_header);
    return readNiftiData(request, inputVector, outputVector);
  }
  return -1;
}

/***********************************************************************************//**
 * This function reads in one data of data.
 * templated to handle different data types.
 */
template<class OT>
void vtkmsqNiftiReaderUpdate(vtkmsqNiftiReader *self, vtkImageData *data, OT *outPtr,
    gzFile zfp)
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

  // progress target
  unsigned long target = (unsigned long) ((numberSlices * numberComponents) / 25.0) + 1;
  unsigned long count = 0;

  // finally read image
  for (unsigned int comp = 0; comp < numberComponents && !self->AbortExecute; comp++)
  {
    // reset slice increment
    sliceIncr = 0;

    for (unsigned int slice = 0; slice < numberSlices; slice++)
    {
      // let's read a slice at a time
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
    }
  }

  delete[] sliceBuffer;

}

/***********************************************************************************//**
 * This function reads a data from a file.  The datas extent/axes
 * are assumed to be the same as the file extent/order.
 */
int vtkmsqNiftiReader::RequestData(
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

  if (!this->FileName && !this->FilePattern)
  {
    vtkErrorMacro("Either a valid FileName or FilePattern must be specified.");
    return 0;
  }

  char * t_imagefilename = nifti_findimgname(this->FileName, 1);
  std::string imagefilename(t_imagefilename);
  delete t_imagefilename;

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

  int is_nii = is_nifti_file(this->FileName);

  if (is_nii == 1)
  {
    char * t_hdrfilename = nifti_findhdrname(this->FileName);
    this->nii_header = nifti_image_read(t_hdrfilename, false);
    gzseek(zfp, this->nii_header->iname_offset, SEEK_SET);

    nifti_image_free(this->nii_header);
    delete t_hdrfilename;
  }

  int *ext = data->GetExtent();

  data->GetPointData()->GetScalars()->SetName("NiftiImage");

  vtkDebugMacro(
      "Reading extent: " << ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]);

  // Call the correct templated function for the output
  ptr = data->GetScalarPointer();
  switch (this->GetDataScalarType())
  {
    vtkTemplateMacro(vtkmsqNiftiReaderUpdate(this, data, (VTK_TT *)(ptr), zfp));
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
void vtkmsqNiftiReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

