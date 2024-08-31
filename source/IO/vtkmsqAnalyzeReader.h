#ifndef __vtkmsqAnalyzeReader_h
#define __vtkmsqAnalyzeReader_h

#include "vtkMedicalImageReader2.h"
#include "vtkmsqMedicalImageProperties.h"
#include "vtkmsqIOWin32Header.h"
#include "vtkmsqAnalyzeHeader.h"

/**  \class AnalyzeImageIO
 * \ingroup IOFilters
 * \author Hans J. Johnson
 * \brief Class that defines how to read Analyze file format.
 * Analyze IMAGE FILE FORMAT - As much information as I can determine from the Medical image
 * formats web site, and the Analyze75.pdf file provided from the Mayo clinic.
 * A special note of thanks to Dennis P. Hanson (dph@mayo.edu) for his generous contributions
 * in getting this information correct.
 *
 * Analyze image file sets consist of at least 2 files:
 * REQUIRED:
 *    - an image file  ([basename].img or [basename].img.gz or [basename].img.Z)
 *          This contains the binary represenation of the raw voxel values.
 *          If the file is uncompressed, it should be of of size (sizeof(storagetype)*NX*NY*NZ(*NT).
 *          The format of the image file is very simple; containing usually
 *          uncompressed voxel data for the images in one of the several
 *          possible voxel formats:
 *             - 1 bit  packed binary (slices begin on byte boundaries)
 *             - 8 bit  (unsigned char) gray scale unless .lkup file present
 *             - 16 bit signed short
 *             - 32 bit signed integers or float
 *             - 24 bit RGB, 8 bits per channel
 *    - a header file  ([basename].hdr)
 *          This a 348 byte file 99.99% of all images that contains a binary represenation of the C-struct
 *          defined in this file.  The analyze 7.5 header structure may, however, be extended beyond this minimal defintion
 *          to encompase site specific information, and would have more than 348 bytes.  Given that the
 *          ability to extend the header has rarely been used, this implementation of the Analyze 7.5
 *          file will only read the first 348 bytes into the structure defined in this file, and all informaiton beyond the
 *          348 bytes will be ignored.
 * OPTIONAL:
 *    - a color lookup file ([basename].lkup)
 *      The .lkup file is a plain ASCII text file that contains 3 integer values between 0 and 255
 *      on each line.  Each line of the lkup file represents one color table entry for the Red,
 *      Green and Blue color components, respectively.  The total dynamic range of the image
 *      is divided by the number of colors in color table to determine mapping of the image through
 *      these colors.
 *       For example, an 8-color 'rainbow colors' lookup table is represented as:
 *       ===========================
 *       255 0 0
 *       255 128 0
 *       255 255 0
 *       128 255 0
 *       0 255 0
 *       0 0 255
 *       128 0 255
 *       255 0 255
 *       ===========================
 *    - an object file ([basename].obj)
 *      A specially formated file with a mapping between object name and image code used to associate
 *      image voxel locations with a label.  This file is run length encoded to save disk storage.
 */

class VTK_MSQ_IO_EXPORT vtkmsqAnalyzeReader: public vtkMedicalImageReader2
{
public:
  static vtkmsqAnalyzeReader *New();vtkTypeMacro(vtkmsqAnalyzeReader,vtkMedicalImageReader2)
  ;
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Is the given file an Analyze file?
  virtual int CanReadFile(const char* fname);

  // Description:
  // Turn on/off automatic byte swapping
  // By default it is on, allowing the reader to determine word
  // endianess from hints in the Analyze header
  vtkGetMacro(AutoByteSwapping, int)
  ;vtkSetMacro(AutoByteSwapping, int)
  ;vtkBooleanMacro(AutoByteSwapping, int)
  ;

  // Description:
  // Get/Set property object
  vtkGetObjectMacro(MedicalImageProperties,vtkmsqMedicalImageProperties)
  ;vtkSetObjectMacro(MedicalImageProperties,vtkmsqMedicalImageProperties)
  ;

  // Description:
  // Valid extensions
  virtual const char* GetFileExtensions()
  {
    return ".hdr .img .HDR .IMG";
  }

  // Description: 
  // A descriptive name for this format
  virtual const char* GetDescriptiveName()
  {
    return "Analyze";
  }

protected:
  //BTX
  struct analyze_dsr header; // Analyze header
  //ETX

  // Description:
  // Medical Image properties
  vtkmsqMedicalImageProperties *MedicalImageProperties;
  int AutoByteSwapping; // automatic byte swapping based on header hints

  vtkmsqAnalyzeReader();
  ~vtkmsqAnalyzeReader();

  virtual int RequestInformation(vtkInformation* request,
      vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  virtual void ExecuteData(vtkDataObject *, vtkInformation *outInfo);

private:
  vtkmsqAnalyzeReader(const vtkmsqAnalyzeReader&); // Not implemented.
  void operator=(const vtkmsqAnalyzeReader&); // Not implemented.
  //BTX
  void InitializeHeader(struct analyze_dsr *hdr);
  int GetAnalyzeEndianess(struct analyze_dsr *temphdr);
  void SwapHeaderBytesIfNecessary(struct analyze_dsr *hdr);
  //ETX
};
#endif

