/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqRawReader.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqRawReader.h"

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

/** \cond 0 */
//vtkCxxRevisionMacro(vtkmsqRawReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkmsqRawReader);
/** \endcond */

/***********************************************************************************//**
 * 
 */
static std::string GetRawExtension(const std::string& filename)
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
  if (fileExt != ".RAW.gz" && fileExt != ".RAW" && fileExt != ".raw.gz"
      && fileExt != ".raw")
  {
    return ("");
  }

  return (fileExt);
}

/***********************************************************************************//**
 * 
 */
vtkmsqRawReader::vtkmsqRawReader()
{
  // Reset properties
  this->MedicalImageProperties = vtkmsqMedicalImageProperties::New();
}

/***********************************************************************************//**
 * 
 */
vtkmsqRawReader::~vtkmsqRawReader()
{
  this->MedicalImageProperties->Delete();
}

/***********************************************************************************//**
 * 
 */
int vtkmsqRawReader::CanReadFile(const char* fname)
{
  std::string filename(fname);
  std::string ext = GetRawExtension(filename);

  // check if we can read the file
  FILE *fp = fopen(filename.c_str(), "rb");
  if (!fp)
  {
    return 0;
  }

  return 1;
}

/***********************************************************************************//**
 * 
 */
void vtkmsqRawReader::SetOrientation(int orientation)
{
  this->MedicalImageProperties->SetOrientationType(orientation);
}

/***********************************************************************************//**
 * This function reads in one data of data.
 * templated to handle different data types.
 */
template<class OT>
void vtkmsqRawReaderUpdate(vtkmsqRawReader *self, vtkImageData *data, OT *outPtr,
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

int vtkmsqRawReader::RequestData(
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
  std::string imagefilename = this->FileName;
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

  data->GetPointData()->GetScalars()->SetName("RawImage");

  //vtkDebugMacro(
  // cout <<   "Reading extent: " << ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5] << endl;//);

  // Call the correct templated function for the output
  ptr = data->GetScalarPointer();
  switch (this->GetDataScalarType())
  {
    vtkTemplateMacro(vtkmsqRawReaderUpdate(this, data, (VTK_TT *)(ptr), zfp));
    default:
      vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
  }

  // close file
  gzclose(zfp);

  return 1;
}

/***********************************************************************************//**
 * This function reads a data from a file.  The datas extent/axes
 * are assumed to be the same as the file extent/order.
 */
/*void vtkmsqRawReader::ExecuteData(vtkDataObject *output, vtkInformation *outInfo)
{
  vtkImageData *data = this->AllocateOutputData(output, outInfo);

  gzFile zfp;
  void *ptr;

  if (!this->FileName && !this->FilePattern)
  {
    vtkErrorMacro("Either a valid FileName or FilePattern must be specified.");
    return;
  }

  // open image for reading
  std::string imagefilename = this->FileName;
  // NOTE: gzFile operations act just like FILE * operations when the files
  // are not in gzip format.
  // This greatly simplifies the following code, and gzFile types are used
  // everywhere.
  if (!(zfp = gzopen(imagefilename.c_str(), "rb")))
  {
    imagefilename += ".gz";
    if (!(zfp = gzopen(imagefilename.c_str(), "rb")))
      return;
  }

  int *ext = data->GetExtent();

  data->GetPointData()->GetScalars()->SetName("RawImage");

  //vtkDebugMacro(
   cout <<   "Reading extent: " << ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5] << endl;//);

  // Call the correct templated function for the output
  ptr = data->GetScalarPointer();
  switch (this->GetDataScalarType())
  {
    vtkTemplateMacro(vtkmsqRawReaderUpdate(this, data, (VTK_TT *)(ptr), zfp));
    default:
      vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
  }

  // close file
  gzclose(zfp);
}
*/

/***********************************************************************************//**
 * 
 */
void vtkmsqRawReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

