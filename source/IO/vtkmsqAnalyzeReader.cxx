/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqAnalyzeReader.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqAnalyzeReader.h"

#include "vtkByteSwap.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vtksys/SystemTools.hxx>
#include <vtkzlib/zlib.h>

// System endianess
// May have to go somewhere else later
#define MSQ_BIG_ENDIAN    0
#define MSQ_LITTLE_ENDIAN 1

/** \cond 0 */
//vtkCxxRevisionMacro(vtkmsqAnalyzeReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkmsqAnalyzeReader);
/** \endcond */

/***********************************************************************************//**
 * 
 */
static std::string GetAnalyzeExtension(const std::string& filename)
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
      && fileExt != ".img" && fileExt != ".hdr" && fileExt != ".HDR")
  {
    return ("");
  }

  return (fileExt);
}

/***********************************************************************************//**
 * 
 */
static std::string GetAnalyzeRootName(const std::string& filename)
{
  const std::string fileExt = GetAnalyzeExtension(filename);

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
static std::string GetAnalyzeHeaderFileName(const std::string & filename)
{
  std::string HeaderFileName(filename);
  const std::string fileExt = GetAnalyzeExtension(filename);

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
static std::string GetAnalyzeImageFileName(const std::string& filename)
{
  std::string ImageFileName(filename);
  const std::string fileExt = GetAnalyzeExtension(filename);

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
static int TestByteOrder()
{
  short int word = 0x0001;
  char *byte = (char *) &word;
  return (byte[0] ? MSQ_LITTLE_ENDIAN : MSQ_BIG_ENDIAN);
}

/***********************************************************************************//**
 * 
 */
vtkmsqAnalyzeReader::vtkmsqAnalyzeReader()
{
  // zero out entire header
  memset((void *) &this->header, 0, sizeof(struct analyze_dsr));

  // initialize header info
  this->InitializeHeader(&header);

  // find out byte endianess from header file
  this->AutoByteSwapping = 1;

  // Reset properties
  this->MedicalImageProperties = vtkmsqMedicalImageProperties::New();
}

/***********************************************************************************//**
 * 
 */
vtkmsqAnalyzeReader::~vtkmsqAnalyzeReader()
{
  this->MedicalImageProperties->Delete();
}

/***********************************************************************************//**
 * Determine Analyze byte ordering
 */
int vtkmsqAnalyzeReader::GetAnalyzeEndianess(struct analyze_dsr *temphdr)
{
  // checking hk.extents only is NOT a good idea. Many programs do not set
  // hk.extents correctly. Doing an additional check on hk.sizeof_hdr
  // increases chance of correct result. --Juerg Tschirrin Univeristy of Iowa
  // All properly constructed analyze images should have the extents field
  // set.  It is part of the file format standard.  While most headers of
  // analyze images are 348 bytes long, The Analyze file format allows the
  // header to have other lengths.
  // This code will fail in the unlikely event that the extents field is
  // not set (invalid anlyze file anyway) and the header is not the normal
  // size.  Other pieces of code have used a heuristic on the image
  // dimensions.  If the Image dimensions is greater
  // than 16000 then the image is almost certainly byte-swapped-- Hans

  if (TestByteOrder() == MSQ_BIG_ENDIAN)
  {
    if ((temphdr->hk.extents == 16384) || (temphdr->hk.sizeof_hdr == 348))
      return VTK_FILE_BYTE_ORDER_BIG_ENDIAN;
    else
      return VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN;
  }
  else
  {
    if ((temphdr->hk.extents == 16384) || (temphdr->hk.sizeof_hdr == 348))
      return VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN;
    else
      return VTK_FILE_BYTE_ORDER_BIG_ENDIAN;
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqAnalyzeReader::SwapHeaderBytesIfNecessary(struct analyze_dsr *imageheader)
{
  if (TestByteOrder() == MSQ_BIG_ENDIAN)
  {
    if (this->GetSwapBytes())
    {
      vtkByteSwap::Swap4BE(&imageheader->hk.sizeof_hdr);
      vtkByteSwap::Swap4BE(&imageheader->hk.extents);

      vtkByteSwap::Swap2BE(&imageheader->hk.session_error);
      vtkByteSwap::Swap2BERange(&imageheader->dime.dim[0], 8);
      vtkByteSwap::Swap2BE(&imageheader->dime.unused1);
      vtkByteSwap::Swap2BE(&imageheader->dime.datatype);
      vtkByteSwap::Swap2BE(&imageheader->dime.bitpix);
      vtkByteSwap::Swap2BE(&imageheader->dime.dim_un0);

      vtkByteSwap::Swap4BERange(&imageheader->dime.pixdim[0], 8);
      vtkByteSwap::Swap4BE(&imageheader->dime.vox_offset);
      vtkByteSwap::Swap4BE(&imageheader->dime.roi_scale);
      vtkByteSwap::Swap4BE(&imageheader->dime.funused1);
      vtkByteSwap::Swap4BE(&imageheader->dime.funused2);
      vtkByteSwap::Swap4BE(&imageheader->dime.cal_max);
      vtkByteSwap::Swap4BE(&imageheader->dime.cal_min);

      vtkByteSwap::Swap4BE(&imageheader->dime.compressed);
      vtkByteSwap::Swap4BE(&imageheader->dime.verified);
      vtkByteSwap::Swap4BE(&imageheader->dime.glmax);
      vtkByteSwap::Swap4BE(&imageheader->dime.glmin);

      vtkByteSwap::Swap4BE(&imageheader->hist.views);
      vtkByteSwap::Swap4BE(&imageheader->hist.vols_added);
      vtkByteSwap::Swap4BE(&imageheader->hist.start_field);
      vtkByteSwap::Swap4BE(&imageheader->hist.field_skip);
      vtkByteSwap::Swap4BE(&imageheader->hist.omax);
      vtkByteSwap::Swap4BE(&imageheader->hist.omin);
      vtkByteSwap::Swap4BE(&imageheader->hist.smax);
      vtkByteSwap::Swap4BE(&imageheader->hist.smin);
    }
  }
  else
  {
    if (this->GetSwapBytes())
    {
      vtkByteSwap::Swap4LE(&imageheader->hk.sizeof_hdr);
      vtkByteSwap::Swap4LE(&imageheader->hk.extents);

      vtkByteSwap::Swap2LE(&imageheader->hk.session_error);
      vtkByteSwap::Swap2LERange(&imageheader->dime.dim[0], 8);
      vtkByteSwap::Swap2LE(&imageheader->dime.unused1);
      vtkByteSwap::Swap2LE(&imageheader->dime.datatype);
      vtkByteSwap::Swap2LE(&imageheader->dime.bitpix);
      vtkByteSwap::Swap2LE(&imageheader->dime.dim_un0);

      vtkByteSwap::Swap4LERange(&imageheader->dime.pixdim[0], 8);
      vtkByteSwap::Swap4LE(&imageheader->dime.vox_offset);
      vtkByteSwap::Swap4LE(&imageheader->dime.roi_scale);
      vtkByteSwap::Swap4LE(&imageheader->dime.funused1);
      vtkByteSwap::Swap4LE(&imageheader->dime.funused2);
      vtkByteSwap::Swap4LE(&imageheader->dime.cal_max);
      vtkByteSwap::Swap4LE(&imageheader->dime.cal_min);

      vtkByteSwap::Swap4LE(&imageheader->dime.compressed);
      vtkByteSwap::Swap4LE(&imageheader->dime.verified);
      vtkByteSwap::Swap4LE(&imageheader->dime.glmax);
      vtkByteSwap::Swap4LE(&imageheader->dime.glmin);

      vtkByteSwap::Swap4LE(&imageheader->hist.views);
      vtkByteSwap::Swap4LE(&imageheader->hist.vols_added);
      vtkByteSwap::Swap4LE(&imageheader->hist.start_field);
      vtkByteSwap::Swap4LE(&imageheader->hist.field_skip);
      vtkByteSwap::Swap4LE(&imageheader->hist.omax);
      vtkByteSwap::Swap4LE(&imageheader->hist.omin);
      vtkByteSwap::Swap4LE(&imageheader->hist.smax);
      vtkByteSwap::Swap4LE(&imageheader->hist.smin);
    }
  }
}

/***********************************************************************************//**
 * 
 */
void vtkmsqAnalyzeReader::InitializeHeader(struct analyze_dsr *hdr)
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
  // Acceptable data values are DT_NONE, DT_UNKOWN, DT_BINARY,
  // DT_UNSIGNED_CHAR
  // DT_SIGNED_SHORT, DT_SIGNED_INT, DT_FLOAT, DT_COMPLEX, DT_DOUBLE,
  // DT_RGB, DT_ALL

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
int vtkmsqAnalyzeReader::CanReadFile(const char* fname)
{
  std::string filename(fname);
  std::string ext = GetAnalyzeExtension(filename);

  // if not default extension bail out
  if (ext != ".HDR" && ext != ".hdr" && ext != ".IMG" && ext != ".img" && ext != ".IMG.gz"
      && ext != ".img.gz")
  {
    return 0;
  }

  // Get header file name
  std::string headerfname = GetAnalyzeHeaderFileName(filename);

  // check if we can read header
  FILE *fp = fopen(headerfname.c_str(), "rb");
  if (!fp)
  {
    return 0;
  }

  // read header
  fread(&this->header, 1, sizeof(struct analyze_dsr), fp);

  // close file
  fclose(fp);

  // if the machine and file endianess are different
  // perform the byte swapping on it
  if (this->GetAutoByteSwapping())
    SetDataByteOrder(GetAnalyzeEndianess(&this->header));
  SwapHeaderBytesIfNecessary(&this->header);

  // just to be sure it is an Analyze file
  if (this->header.hk.sizeof_hdr != 348)
  {
    return 0;
  }

  return 3;
}

/***********************************************************************************//**
 * This method returns the largest data that can be generated.
 */
int vtkmsqAnalyzeReader::RequestInformation(vtkInformation* request,
    vtkInformationVector** inputVector, vtkInformationVector* outputVector)

{
  FILE *fp;

  // Get .hdr file name
  std::string headerfilename = GetAnalyzeHeaderFileName(this->GetFileName());

  if (!(fp = fopen(headerfilename.c_str(), "rb")))
  {
    vtkErrorMacro("Unable to open file " << headerfilename.c_str());
    return 0;
  }

  // read header
  fread(&this->header, 1, sizeof(struct analyze_dsr), fp);

  // close file
  fclose(fp);

  // if the machine and file endianess are different
  // perform the byte swapping on it
  if (this->GetAutoByteSwapping())
    SetDataByteOrder(GetAnalyzeEndianess(&this->header));
  SwapHeaderBytesIfNecessary(&this->header);

  // just to be sure it is an Analyze file
  if (this->header.hk.sizeof_hdr != 348)
  {
    vtkErrorMacro("This is not an Analyze file " << headerfilename.c_str());
    return 0;
  }

  // set image information
  this->SetFileDimensionality(3);

  // set up image dimensions, spacing, components, etc
  this->SetDataOrigin(0, 0, 0);

  this->SetDataExtent(0, this->header.dime.dim[1] - 1, 0, this->header.dime.dim[2] - 1, 0,
      this->header.dime.dim[3] - 1);

  this->SetDataSpacing(this->header.dime.pixdim[1], this->header.dime.pixdim[2],
      this->header.dime.pixdim[3]);

  this->SetNumberOfScalarComponents(this->header.dime.dim[4]);

  // This probably needs to be changed, since we need a way to store
  // image metainformation along with the image data
  if (this->header.hist.orient == MSQ_ANALYZE_ORIENTATION_RPI_TRANSVERSE
      || this->header.hist.orient == MSQ_ANALYZE_ORIENTATION_RAI_TRANSVERSE_FLIPPED)
    this->MedicalImageProperties->SetOrientationType(vtkMedicalImageProperties::AXIAL);
  else if (this->header.hist.orient == MSQ_ANALYZE_ORIENTATION_RIP_CORONAL
      || this->header.hist.orient == MSQ_ANALYZE_ORIENTATION_RSP_CORONAL_FLIPPED)
    {
      this->MedicalImageProperties->SetOrientationType(vtkMedicalImageProperties::CORONAL);
      this->MedicalImageProperties->SetDirectionCosine(1, 0, 0, 0, 0, -1);
    }
  else if (this->header.hist.orient == MSQ_ANALYZE_ORIENTATION_PIR_SAGITTAL
      || this->header.hist.orient == MSQ_ANALYZE_ORIENTATION_PIL_SAGITTAL_FLIPPED)
    {
      this->MedicalImageProperties->SetOrientationType(vtkMedicalImageProperties::SAGITTAL);
      this->MedicalImageProperties->SetDirectionCosine(0, 1, 0, 0, 0, 1);      
    }

  // set up VTK data type
  switch (this->header.dime.datatype)
  {
    case ANALYZE_DT_BINARY:
      this->SetDataScalarTypeToChar();
      break;
    case ANALYZE_DT_UNSIGNED_CHAR:
      this->SetDataScalarTypeToUnsignedChar();
      break;
    case ANALYZE_DT_SIGNED_SHORT:
      this->SetDataScalarTypeToShort();
      break;
    case SPMANALYZE_DT_UNSIGNED_SHORT:
      this->SetDataScalarTypeToUnsignedShort();
      break;
    case ANALYZE_DT_SIGNED_INT:
      this->SetDataScalarTypeToInt();
      break;
    case SPMANALYZE_DT_UNSIGNED_INT:
      this->SetDataScalarTypeToUnsignedInt();
      break;
    case ANALYZE_DT_FLOAT:
      this->SetDataScalarTypeToFloat();
      break;
    case ANALYZE_DT_DOUBLE:
      this->SetDataScalarTypeToDouble();
      break;
    case ANALYZE_DT_RGB:
      this->SetDataScalarTypeToUnsignedChar();
      this->SetNumberOfScalarComponents(3);
    default:
      this->SetDataScalarTypeToShort();
  };

  // call father to finish up
  return this->Superclass::RequestInformation(request, inputVector, outputVector);
}

/***********************************************************************************//**
 * This function reads in one data of data.
 * templated to handle different data types.
 */
template<class OT>
void vtkmsqAnalyzeReaderUpdate(vtkmsqAnalyzeReader *self, vtkImageData *data, OT *outPtr,
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
int vtkmsqAnalyzeReader::RequestData(
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

  // open image for reading
  std::string imagefilename = GetAnalyzeImageFileName(this->FileName);

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

  data->GetPointData()->GetScalars()->SetName("AnalyzeImage");

  vtkDebugMacro(
      "Reading extent: " << ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]);

  // Call the correct templated function for the output
  ptr = data->GetScalarPointer();
  switch (this->GetDataScalarType())
  {
    vtkTemplateMacro(vtkmsqAnalyzeReaderUpdate(this, data, (VTK_TT *)(ptr), zfp));
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
void vtkmsqAnalyzeReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

