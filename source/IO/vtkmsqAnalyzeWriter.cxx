/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqAnalyzeWriter.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqAnalyzeWriter.h"

#include "vtkCommand.h"
#include "vtkErrorCode.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDataSetAttributes.h"

#include <vtksys/SystemTools.hxx>
#include <vtkstd/string>
#include <vtkzlib/zlib.h>
#include <sys/stat.h>

/** \cond 0 */
vtkCxxRevisionMacro(vtkmsqAnalyzeWriter, "$Revision: 1.17 $");
vtkStandardNewMacro(vtkmsqAnalyzeWriter);
/** \endcond */

/***********************************************************************************//**
 * 
 */
static vtkstd::string GetAnalyzeExtension(const vtkstd::string& filename)
{
  vtkstd::string fileExt(vtksys::SystemTools::GetFilenameLastExtension(filename));

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
      && fileExt != ".img" && fileExt != ".hdr" && fileExt != ".HDR")
  {
    return ("");
  }

  return (fileExt);
}

/***********************************************************************************//**
 * 
 */
static vtkstd::string GetAnalyzeRootName(const vtkstd::string& filename)
{
  const vtkstd::string fileExt = GetAnalyzeExtension(filename);

  // Create a base filename
  // i.e Image.HDR --> Image
  if (fileExt.length() > 0 && filename.length() > fileExt.length())
  {
    const vtkstd::string::size_type it = filename.find_last_of(fileExt);
    vtkstd::string baseName(filename, 0, it - (fileExt.length() - 1));
    return (baseName);
  }

  //Default to return same as input when the extension is nothing.
  return (filename);
}

/***********************************************************************************//**
 * 
 */
static vtkstd::string GetAnalyzeHeaderFileName(const vtkstd::string & filename)
{
  vtkstd::string HeaderFileName(filename);
  const vtkstd::string fileExt = GetAnalyzeExtension(filename);

  // Accomodate either all caps or all lower-case filenames.
  if ((fileExt == ".IMG") || (fileExt == ".IMG.gz"))
  {
    HeaderFileName = GetAnalyzeRootName(filename);
    HeaderFileName += ".HDR";
  }
  else if ((fileExt == ".img") || (fileExt == ".img.gz"))
  {
    HeaderFileName = GetAnalyzeRootName(filename);
    HeaderFileName += ".hdr";
  }
  return (HeaderFileName);
}

/***********************************************************************************//**
 * 
 */
static vtkstd::string GetAnalyzeImageFileName(const vtkstd::string& filename)
{
  vtkstd::string ImageFileName(filename);
  const vtkstd::string fileExt = GetAnalyzeExtension(filename);

  // Default to uncompressed .IMG if .HDR is given as file name.
  if (fileExt == ".HDR")
  {
    ImageFileName = GetAnalyzeRootName(filename);
    ImageFileName += ".IMG";
  }
  else if (fileExt == ".hdr")
  {
    ImageFileName = GetAnalyzeRootName(filename);
    ImageFileName += ".img";
  }
  return (ImageFileName);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqAnalyzeWriter::InitializeHeader(struct analyze_dsr *hdr)
{
  hdr->hk.sizeof_hdr = static_cast<int>(sizeof(struct analyze_dsr));
  hdr->hk.db_name[0] = '\0';
  hdr->hk.extents = 16384;
  hdr->hk.session_error = 0;
  hdr->hk.regular = 'r';
  hdr->hk.hkey_un0 = '\0';

  /* HeaderObj_dimension information*/
  hdr->dime.dim[0] = 4; //Usually 4 x,y,z,time
  hdr->dime.dim[1] = 1; //size_x;//number of columns
  hdr->dime.dim[2] = 1; //size_y;//number of rows
  hdr->dime.dim[3] = 1; //size_z;//number of slices
  hdr->dime.dim[4] = 1; //size_t;//number of volumes
  hdr->dime.dim[5] = 1;
  hdr->dime.dim[6] = 1;
  hdr->dime.dim[7] = 1;

  /* labels voxel spatial unit */
  hdr->dime.vox_units[0] = '\0';
  /* labels voxel calibration unit */
  hdr->dime.cal_units[0] = '\0';

  hdr->dime.unused1 = 0;
  hdr->dime.dim_un0 = 0;

  // Set the voxel dimension fields:
  // A value of 0.0 for these fields implies that the value is unknown.
  // Change these values to what is appropriate for your data
  // or pass additional commahdr->dime.dim[0] line arguments
  hdr->dime.pixdim[0] = 0.0f; //Unused field
  hdr->dime.pixdim[1] = 1.0f; //x_dimension
  hdr->dime.pixdim[2] = 1.0f; //y_dimension
  hdr->dime.pixdim[3] = 1.0f; //z_dimension
  hdr->dime.pixdim[4] = 1.0f; //t_dimension
  hdr->dime.pixdim[5] = 1.0f;
  hdr->dime.pixdim[6] = 1.0f;
  hdr->dime.pixdim[7] = 1.0f;
  // Assume zero offset in .img file, byte at which pixel data starts in
  // the HeaderObj file
  // byte offset in the HeaderObj file which voxels start
  hdr->dime.vox_offset = 0.0f;

  hdr->dime.roi_scale = 0.0f;
  hdr->dime.funused1 = 0.0f;
  hdr->dime.funused2 = 0.0f;
  hdr->dime.cal_max = 0.0f; // specify range of calibration values
  hdr->dime.cal_min = 0.0f; // specify range of calibration values
  hdr->dime.compressed = 0; // specify that the data file with extension
  // .img is not compressed
  hdr->dime.verified = 0;
  hdr->dime.glmax = 0; // max value for all of the data set
  hdr->dime.glmin = 0; // min value for all of the data set

  /*data_history*/
  hdr->hist.descrip[0] = '\0';
  hdr->hist.aux_file[0] = '\0';
  /*Acceptable values are*/
  /*0-transverse unflipped*/
  /*1-coronal unflipped*/
  /*2-sagittal unfipped*/
  /*3-transverse flipped*/
  /*4-coronal flipped*/
  /*5-sagittal flipped*/
  hdr->hist.orient = MSQ_ANALYZE_ORIENTATION_RPI_TRANSVERSE;

  hdr->hist.originator[0] = '\0';
  hdr->hist.generated[0] = '\0';
  hdr->hist.scannum[0] = '\0';
  hdr->hist.patient_id[0] = '\0';
  hdr->hist.exp_date[0] = '\0';
  hdr->hist.exp_time[0] = '\0';
  hdr->hist.hist_un0[0] = '\0';
  hdr->hist.views = 0;
  hdr->hist.vols_added = 0;
  hdr->hist.start_field = 0;
  hdr->hist.field_skip = 0;
  hdr->hist.omax = 0;
  hdr->hist.omin = 0;
  hdr->hist.smax = 0;
  hdr->hist.smin = 0;
}

/***********************************************************************************//**
 * 
 */
vtkmsqAnalyzeWriter::vtkmsqAnalyzeWriter()
{
  // zero out entire header
  memset((void *) &this->header, 0, sizeof(struct analyze_dsr));

  // initialize header info
  this->InitializeHeader(&header);

  // No compression
  this->Compression = 0;

  // Reset propperties
  this->MedicalImageProperties = NULL;
}

/***********************************************************************************//**
 * 
 */
int vtkmsqAnalyzeWriter::CanWriteFile(const char * FileNameToWrite)
{
  vtkstd::string filename(FileNameToWrite);
  // Data file name given?
  vtkstd::string::size_type imgPos = filename.rfind(".img");
  if ((imgPos != vtkstd::string::npos) && (imgPos == filename.length() - 4))
  {
    return 1;
  }

  // Header file given?
  vtkstd::string::size_type hdrPos = filename.rfind(".hdr");
  if ((hdrPos != vtkstd::string::npos) && (hdrPos == filename.length() - 4))
  {
    return 1;
  }

  // Compressed image given?
  vtkstd::string::size_type imggzPos = filename.rfind(".img.gz");
  if ((imggzPos != vtkstd::string::npos) && (imggzPos == filename.length() - 7))
  {
    return 1;
  }

  return 0;
}

/***********************************************************************************//**
 * 
 */
int vtkmsqAnalyzeWriter::WriteHeader(const char *fileName)
{
  // Get header filename
  vtkstd::string headerFilename = GetAnalyzeHeaderFileName(fileName);

  // Try opening the header file for writing
  FILE *fp;
  if (!(fp = fopen(headerFilename.c_str(), "wb")))
    return 0;

  // write header
  fwrite(&this->header, 1, sizeof(struct analyze_dsr), fp);

  // close file
  fclose(fp);

  return 1;
}

/***********************************************************************************//**
 * This function writes image  in one data of data.
 * templated to handle different data types.
 */
template<class OT>
void vtkmsqAnalyzeWriterUpdateCompressed(vtkmsqAnalyzeWriter *self, vtkImageData *data, OT *inPtr,
    gzFile zfp)
{
  int outExtent[6];
  data->GetExtent(outExtent);

  unsigned int numberColumns = outExtent[1] - outExtent[0] + 1;
  unsigned int numberRows = outExtent[3] - outExtent[2] + 1;
  unsigned int numberSlices = outExtent[5] - outExtent[4] + 1;
  unsigned int numberComponents = data->GetNumberOfScalarComponents();

  // buffer for single slice of type OT
  unsigned long sliceSize = numberColumns * numberRows;
  unsigned long sliceSizeComp = sliceSize * numberComponents;
  unsigned long imageSliceSizeInBytes = sliceSize * sizeof(OT);
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
      bufferIncr = 0;
      localIncr = comp;

      // copy slice contents into vtk image output
      while (bufferIncr < sliceSize)
      {
        sliceBuffer[bufferIncr] = inPtr[sliceIncr + localIncr];
        localIncr += numberComponents;
        bufferIncr++;
      }

      // let's read a slice at a time
      gzwrite(zfp, sliceBuffer, imageSliceSizeInBytes);

      // update progress
      if (!(count % target))
      {
        self->UpdateProgress(count / (25.0 * target));
      }
      count++;

      // advance one slice
      sliceIncr += sliceSizeComp;
    }
  }

  delete[] sliceBuffer;

}

/***********************************************************************************//**
 * This function writes image  in one data of data.
 * templated to handle different data types.
 */
template<class OT>
void vtkmsqAnalyzeWriterUpdate(vtkmsqAnalyzeWriter *self, vtkImageData *data, OT *inPtr,
    FILE *fp)
{
  int outExtent[6];
  data->GetExtent(outExtent);

  unsigned int numberColumns = outExtent[1] - outExtent[0] + 1;
  unsigned int numberRows = outExtent[3] - outExtent[2] + 1;
  unsigned int numberSlices = outExtent[5] - outExtent[4] + 1;
  unsigned int numberComponents = data->GetNumberOfScalarComponents();

  //printf("cols=%d, rows=%d, slices=%d, comps=%d\n", numberColumns, numberRows, numberSlices, numberComponents);

  // buffer for single slice of type OT
  unsigned long sliceSize = numberColumns * numberRows;
  unsigned long sliceSizeComp = sliceSize * numberComponents;
  unsigned long imageSliceSizeInBytes = sliceSize * sizeof(OT);
  OT *sliceBuffer = new OT[sliceSize];

  // increments
  unsigned long sliceIncr = 0;
  unsigned long bufferIncr = 0;
  unsigned long localIncr = 0;

  // progress target
  unsigned long target = (unsigned long) ((numberSlices * numberComponents) / 25.0) + 1;
  unsigned long count = 0;

  // finally write image
  for (unsigned int comp = 0; comp < numberComponents && !self->AbortExecute; comp++)
  {
    // reset slice increment
    //sliceIncr = sliceSize * numberSlices * comp;
    sliceIncr = 0;

    for (unsigned int slice = 0; slice < numberSlices; slice++)
    {
      bufferIncr = 0;
      localIncr = comp;
      
      // copy slice contents into vtk image output
      while (bufferIncr < sliceSize)
      {
        sliceBuffer[bufferIncr] = inPtr[sliceIncr + localIncr];
        localIncr += numberComponents;
        bufferIncr++;
      }

      // let's write a slice at a time
      fwrite(sliceBuffer, 1, imageSliceSizeInBytes, fp);

      // update progress
      if (!(count % target))
      {
        self->UpdateProgress(count / (25.0 * target));
      }
      count++;

      // advance one slice
      sliceIncr += sliceSizeComp;
    }

  }

  delete[] sliceBuffer;

}
/***********************************************************************************//**
 * 
 */
int vtkmsqAnalyzeWriter::WriteImage(const char *fileName)
{
  // Get header filename
  vtkstd::string imageFilename = GetAnalyzeImageFileName(fileName);

  gzFile zfp;
  FILE *fp;

  void *ptr;

  // Determine whether to use compression or not
  if (this->Compression)
  {
    imageFilename += ".gz";
    zfp = gzopen(imageFilename.c_str(), "wb");

    // if error close file and exit
    if (!zfp)
    {
      gzclose(zfp);
      return 0;
    }

  } else {

    fp = fopen(imageFilename.c_str(), "wb");

    // if error close file and exit
    if (!fp)
    {
      fclose(fp);
      return 0;
    }

  }

  // Call the correct templated function for the output
  ptr = this->GetInput()->GetScalarPointer();

  if (this->Compression) {

    switch (this->GetInput()->GetScalarType())
    {
      vtkTemplateMacro(
          vtkmsqAnalyzeWriterUpdateCompressed(this, this->GetInput(), (VTK_TT *)(ptr), zfp));
      default:
        vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
    }
    
    // close file
    gzclose(zfp);

  } else {

    switch (this->GetInput()->GetScalarType())
    {
      vtkTemplateMacro(
          vtkmsqAnalyzeWriterUpdate(this, this->GetInput(), (VTK_TT *)(ptr), fp));
      default:
        vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
    }

    // close file
    fclose(fp);
  }

  return 1;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqAnalyzeWriter::Write()
{
  this->SetErrorCode(vtkErrorCode::NoError);

  this->GetInput()->UpdateInformation();

  // Error checking
  if (this->GetInput() == NULL)
  {
    vtkErrorMacro(<<"Write:Please specify an input!");
    return;
  }

  if (this->MedicalImageProperties == NULL)
  {
    vtkErrorMacro(<<"Write:Please specify a medical image property object!");
    return;
  }

  // Retrive dimensions and spacing
  int *wholeExtent = this->GetInput()->GetWholeExtent();
  double *spacing = this->GetInput()->GetSpacing();

  int dimSize[3];
  dimSize[0] = wholeExtent[1] - wholeExtent[0] + 1;
  dimSize[1] = wholeExtent[3] - wholeExtent[2] + 1;
  dimSize[2] = wholeExtent[5] - wholeExtent[4] + 1;

  // Get number of components
  int numberOfComponents = this->GetInput()->GetNumberOfScalarComponents();

  // Fill out dimension parameters
  this->header.dime.dim[0] = 4; // Analyze 7.5 specs
  this->header.dime.dim[1] = dimSize[0];
  this->header.dime.dim[2] = dimSize[1];
  this->header.dime.dim[3] = dimSize[2];
  this->header.dime.dim[4] = numberOfComponents;

  // Fill out spacing info
  this->header.dime.pixdim[1] = (float) spacing[0];
  this->header.dime.pixdim[2] = (float) spacing[1];
  this->header.dime.pixdim[3] = (float) spacing[2];

  // Fill out orientation information
  this->header.hist.orient = this->MedicalImageProperties->GetOrientationType(0);

  // Fill out data type
  int scalarType = this->GetInput()->GetScalarType();
  int elementType = ANALYZE_DT_SIGNED_SHORT;

  switch (scalarType)
  {
    case VTK_CHAR:
      elementType = ANALYZE_DT_BINARY;
      break;
    case VTK_SIGNED_CHAR:
      elementType = ANALYZE_DT_BINARY;
      break;
    case VTK_UNSIGNED_CHAR:
      elementType = ANALYZE_DT_UNSIGNED_CHAR;
      break;
    case VTK_SHORT:
      elementType = ANALYZE_DT_SIGNED_SHORT;
      break;
    case VTK_UNSIGNED_SHORT:
      elementType = SPMANALYZE_DT_UNSIGNED_SHORT;
      break;
    case VTK_INT:
      elementType = ANALYZE_DT_SIGNED_INT;
      break;
    case VTK_UNSIGNED_INT:
      elementType = SPMANALYZE_DT_UNSIGNED_INT;
      break;
    case VTK_LONG:
      elementType = ANALYZE_DT_SIGNED_INT;
      break;
    case VTK_UNSIGNED_LONG:
      elementType = SPMANALYZE_DT_UNSIGNED_INT;
      break;
    case VTK_FLOAT:
      elementType = ANALYZE_DT_FLOAT;
      break;
    case VTK_DOUBLE:
      elementType = ANALYZE_DT_DOUBLE;
      break;
    default:
      vtkErrorMacro("Unknown scalar type.");
      return;
  }

  //printf("Element type=%d\n", elementType);

  // Handle the ANALYZE_DT_RGB case
  if (elementType == ANALYZE_DT_UNSIGNED_CHAR && numberOfComponents == 3)
    elementType = ANALYZE_DT_RGB;

  //assign the correct data type
  this->header.dime.datatype = elementType;

  // Set file dimensionality
  this->SetFileDimensionality(3);

  this->InvokeEvent(vtkCommand::StartEvent);

  this->UpdateProgress(0.0);

  // Write header file .hdr
  if (!this->WriteHeader(this->FileName))
  {
    this->SetErrorCode(vtkErrorCode::CannotOpenFileError);
    return;
  }

  // Write image file .img or .img.gz 
  if (!this->WriteImage(this->FileName))
  {
    this->SetErrorCode(vtkErrorCode::CannotOpenFileError);
    return;
  }

  this->UpdateProgress(1.0);

  this->InvokeEvent(vtkCommand::EndEvent);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqAnalyzeWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

