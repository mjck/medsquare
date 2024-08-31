/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqBruker2DSEQReader.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqBruker2DSEQReader.h"

#include "vtkByteSwap.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"

//#include <vtksys/ios/fstream>
//#include <vtksys/ios/sstream>
//#include <vtksys/ios/iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vtksys/SystemTools.hxx>
#include <vtkzlib/zlib.h>

#define MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR   '/'

#define MSQ_RECO_FILE            "reco"
#define MSQ_ACQP_FILE            "acqp"
#define MSQ_DTHREEPROC_FILE      "d3proc"

#define MSQ_METHOD_FILE          "method"
#define MSQ_Method_DiffGrad      "DiffGrad"
#define MSQ_Method_NoDiffExp     "NoDiffExp="
#define MSQ_PVM_SliceOrient      "##$PVM_SPackArrSliceOrient=("
#define MSQ_PVM_SpatResol        "##$PVM_SpatResol=("

#define MSQ_RECO_byte_order      "##$RECO_byte_order="
#define MSQ_BRUKER_LITTLE_ENDIAN "littleEndian"  
#define MSQ_BRUKER_BIG_ENDIAN    "bigEndian"
#define MSQ_RECO_fov             "##$RECO_fov=("
#define MSQ_RECO_size            "##$RECO_size=("
#define MSQ_RECO_wordtype        "##$RECO_wordtype="
#define MSQ_RECO_image_type      "##$RECO_image_type="
#define MSQ_RECO_transposition   "##$RECO_transposition=("
#define MSQ_MAGNITUDE_IMAGE      "MAGNITUDE_IMAGE"
#define MSQ_REAL_IMAGE           "REAL_IMAGE"
#define MSQ_IMAGINARY_IMAGE      "IMAGINARY_IMAGE"
#define MSQ_COMPLEX_IMAGE        "COMPLEX_IMAGE"
#define MSQ_PHASE_IMAGE          "PHASE_IMAGE"
#define MSQ_IR_IMAGE             "IR_IMAGE"
#define MSQ_ACQ_dim              "##$ACQ_dim="
#define MSQ_Ni                   "##$NI="
#define MSQ_Nr                   "##$NR="
#define MSQ_Nechoes              "##$NECHOES="
#define MSQ_ACQ_slice_thick      "##$ACQ_slice_thick="
#define MSQ_ACQ_slice_sepn       "##$ACQ_slice_sepn=("
#define MSQ_ACQ_slice_sepn_mode  "##$ACQ_slice_sepn_mode="
#define MSQ_BRUKER_SIGNED_CHAR   "_8BIT_SGN_INT"
#define MSQ_BRUKER_UNSIGNED_CHAR "_8BIT_UNSGN_INT"
#define MSQ_BRUKER_SIGNED_SHORT  "_16BIT_SGN_INT"
#define MSQ_BRUKER_SIGNED_INT    "_32BIT_SGN_INT"
#define MSQ_BRUKER_FLOAT         "_32BIT_FLOAT"
#define MSQ_ACQ_echo_time        "##$ACQ_echo_time=("
#define MSQ_ACQ_repetition_time  "##$ACQ_repetition_time=("
#define MSQ_ACQ_inversion_time   "##$ACQ_inversion_time=("
#define MSQ_ACQ_grad_matrix      "##$ACQ_grad_matrix=("
#define MSQ_DATTYPE              "##$DATTYPE="
#define MSQ_IM_SIX               "##$IM_SIX="
#define MSQ_IM_SIY               "##$IM_SIY="
#define MSQ_IM_SIZ               "##$IM_SIZ="
#define MSQ_IM_SIT               "##$IM_SIT="
#define MSQ_IP_CHAR              "ip_char"
#define MSQ_IP_SHORT             "ip_short"
#define MSQ_IP_INT               "ip_int"
#define MSQ_DIFFGRAD             "DiffGrad"

/** \cond 0 */
//vtkCxxRevisionMacro(vtkmsqBruker2DSEQReader, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkmsqBruker2DSEQReader);
/** \endcond */

/***********************************************************************************//**
 * 
 */
vtkmsqBruker2DSEQReader::vtkmsqBruker2DSEQReader()
{
  // find out byte endianess from header file
  this->AutoByteSwapping = 1;

  // Reset properties
  this->MedicalImageProperties = vtkmsqMedicalImageProperties::New();
}

/***********************************************************************************//**
 * 
 */
vtkmsqBruker2DSEQReader::~vtkmsqBruker2DSEQReader()
{
  this->MedicalImageProperties->Delete();
}

/***********************************************************************************//**
 * 
 */
int vtkmsqBruker2DSEQReader::CanReadFile(const char* fname)
{
  std::string file2Dseq = vtksys::SystemTools::CollapseFullPath(fname);
  vtksys::SystemTools::ConvertToUnixSlashes(file2Dseq);
  std::string path = vtksys::SystemTools::GetFilenamePath(file2Dseq);
  std::string pathalt = vtksys::SystemTools::GetFilenamePath(file2Dseq);
  std::string filereco = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  filereco += MSQ_RECO_FILE;
  std::string filed3proc = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  filed3proc += MSQ_DTHREEPROC_FILE;
  std::vector<std::string> pathComponents;
  vtksys::SystemTools::SplitPath(path.c_str(), pathComponents);
  if (pathComponents.size() < 3)
  {
    return 0;
  }
  // Go two directories up.
  pathComponents.pop_back();
  pathComponents.pop_back();
  pathalt = vtksys::SystemTools::JoinPath(pathComponents);

  std::string fileacqp = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  fileacqp += MSQ_ACQP_FILE;
  std::string filemethod = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  filemethod += MSQ_METHOD_FILE;

  std::string readFileBufferString = "";
  char readFileBuffer[512] = "";
  std::string::size_type index;
  unsigned long length2DSEQ = 0;
  unsigned long calcLength = 1;

  // Does the '2dseq' file exist?
  if (!vtksys::SystemTools::FileExists(file2Dseq.c_str()))
  {
    return 0;
  }

  // get length of file in bytes:
  length2DSEQ = vtksys::SystemTools::FileLength(file2Dseq.c_str());

  // Check reco for existance.
  std::ifstream reco_InputStream;
  reco_InputStream.open(filereco.c_str(), std::ios::in);
  if (reco_InputStream.fail())
  {
    return 0;
  }
  while (!reco_InputStream.eof())
  {
    reco_InputStream.getline(readFileBuffer, sizeof(readFileBuffer));
    readFileBufferString = readFileBuffer;

    // Get the image data type.
    index = readFileBufferString.find(MSQ_RECO_wordtype);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_RECO_wordtype;
      std::string dattypeString = readFileBufferString.substr(
          index + tempString.length());
      if (dattypeString.find(MSQ_BRUKER_SIGNED_CHAR) != std::string::npos)
      {
        calcLength *= (unsigned long) sizeof(char);
      }
      else if (dattypeString.find(MSQ_BRUKER_UNSIGNED_CHAR) != std::string::npos)
      {
        calcLength *= (unsigned long) sizeof(unsigned char);
      }
      else if (dattypeString.find(MSQ_BRUKER_SIGNED_SHORT) != std::string::npos)
      {
        calcLength *= (unsigned long) sizeof(short);
      }
      else if (dattypeString.find(MSQ_BRUKER_SIGNED_INT) != std::string::npos)
      {
        calcLength *= (unsigned long) sizeof(int);
      }
      else if (dattypeString.find(MSQ_BRUKER_FLOAT) != std::string::npos)
      {
        calcLength *= (unsigned long) sizeof(float);
      }
      else
      {
        reco_InputStream.close();
        return 0;
      }
    }
  }
  reco_InputStream.close();

  // Check acqp for existance.
  if (!vtksys::SystemTools::FileExists(fileacqp.c_str()))
  {
    fileacqp = pathalt + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR + MSQ_ACQP_FILE;
    if (!vtksys::SystemTools::FileExists(fileacqp.c_str()))
    {
      return 0;
    }
  }

  // Check method for existance
  if (!vtksys::SystemTools::FileExists(filemethod.c_str()))
  {
    filemethod = pathalt + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR + MSQ_METHOD_FILE;
    if (!vtksys::SystemTools::FileExists(filemethod.c_str()))
    {
      return 0;
    }
  }

  // Check d3proc for existance.
  std::ifstream d3proc_InputStream;
  d3proc_InputStream.open(filed3proc.c_str(), std::ios::in);
  if (d3proc_InputStream.fail())
  {
    return 0;
  }

  while (!d3proc_InputStream.eof())
  {
    d3proc_InputStream.getline(readFileBuffer, sizeof(readFileBuffer));
    readFileBufferString = readFileBuffer;

    // Get the x size.
    index = readFileBufferString.find(MSQ_IM_SIX);
    if (index != std::string::npos)
    {
      unsigned long xDim = 0;
      std::string tempString = MSQ_IM_SIX;
      std::istringstream im_sixString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_sixString)
      {
        d3proc_InputStream.close();
        return 0;
      }
      im_sixString >> xDim;
      calcLength *= xDim;
    }

    // Get the y size.
    index = readFileBufferString.find(MSQ_IM_SIY);
    if (index != std::string::npos)
    {
      unsigned long yDim = 0;
      std::string tempString = MSQ_IM_SIY;
      std::istringstream im_siyString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_siyString)
      {
        d3proc_InputStream.close();
        return 0;
      }
      im_siyString >> yDim;
      calcLength *= yDim;
    }

    // Get the z size.
    index = readFileBufferString.find(MSQ_IM_SIZ);
    if (index != std::string::npos)
    {
      unsigned long zDim = 0;
      std::string tempString = MSQ_IM_SIZ;
      std::istringstream im_sizString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_sizString)
      {
        d3proc_InputStream.close();
        return 0;
      }
      im_sizString >> zDim;
      calcLength *= zDim;
    }

    // Get the t size.
    index = readFileBufferString.find(MSQ_IM_SIT);
    if (index != std::string::npos)
    {
      unsigned long tDim = 0;
      std::string tempString = MSQ_IM_SIT;
      std::istringstream im_sizString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_sizString)
      {
        d3proc_InputStream.close();
        return 0;
      }
      im_sizString >> tDim;
      calcLength *= tDim;
    }
  }

  d3proc_InputStream.close();

  // Compare the file length to the calculated length.
  // Are they equal?
  if (calcLength != length2DSEQ)
  {
    return 0;
  }

  return 1;
}

/***********************************************************************************//**
 * This method returns the largest data that can be generated.
 */
int vtkmsqBruker2DSEQReader::RequestInformation(vtkInformation* request,
    vtkInformationVector** inputVector, vtkInformationVector* outputVector)

{
  std::string file2Dseq = vtksys::SystemTools::CollapseFullPath(this->FileName);
  vtksys::SystemTools::ConvertToUnixSlashes(file2Dseq);
  std::string path = vtksys::SystemTools::GetFilenamePath(file2Dseq);
  std::string pathalt = vtksys::SystemTools::GetFilenamePath(file2Dseq);
  std::string filereco = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  filereco += MSQ_RECO_FILE;
  std::string filed3proc = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  filed3proc += MSQ_DTHREEPROC_FILE;
  std::string filemethod = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  filemethod += MSQ_METHOD_FILE;
  std::vector<std::string> pathComponents;

  vtksys::SystemTools::SplitPath(path.c_str(), pathComponents);
  if (pathComponents.size() < 3)
  {
    vtkErrorMacro("Cannot create path for acqp file: " << path);
    return 0;
  }
  // Go two directories up.
  pathComponents.pop_back();
  pathComponents.pop_back();
  pathalt = vtksys::SystemTools::JoinPath(pathComponents);

  std::string fileacqp = path + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR;
  fileacqp += MSQ_ACQP_FILE;

  std::string readFileBufferString = "";
  char readFileBuffer[512] = "";
  std::string::size_type index;
  std::string::size_type tempIndex = 0;
  std::vector<double> imageFOV(3);
  std::vector<double> imageRes(3);
  std::vector<unsigned int> imageDim(4);
  bool numDimensions = false;
  bool byteOrder = false;
  bool slicesNotInSameOrientation = false;
  bool sliceThickness = false;
  int numSeperation = 0;
  int numRecoTranspose = -1;
  double sliceThick = 0;
  std::string seperationMode = "";
  std::vector<double> dirx(3, 0), diry(3, 0), dirz(3, 0);
  std::vector<int> recoTransposition;
  int acq_dim = -1;
  int transpose = 0;
  bool diffusionEPI = false;
  int diffusionDirs = 0;
  std::string sliceOrient = "";

  std::ifstream d3proc_InputStream;
  d3proc_InputStream.open(filed3proc.c_str(), std::ios::in);
  if (d3proc_InputStream.fail())
  {
    vtkErrorMacro("d3proc file: " << filed3proc << " cannot be opened.");
    return 0;
  }
  while (!d3proc_InputStream.eof())
  {
    d3proc_InputStream.getline(readFileBuffer, sizeof(readFileBuffer));
    readFileBufferString = readFileBuffer;

    // Get the x size.
    index = readFileBufferString.find(MSQ_IM_SIX);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_IM_SIX;
      std::istringstream im_sixString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_sixString)
      {
        d3proc_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$IM_SIX" << ". File is " << filed3proc);
        return 0;
      }
      im_sixString >> imageDim[0];
    }

    // Get the y size.
    index = readFileBufferString.find(MSQ_IM_SIY);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_IM_SIY;
      std::istringstream im_siyString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_siyString)
      {
        d3proc_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$IM_SIY" << ". File is " << filed3proc);
        return 0;
      }
      im_siyString >> imageDim[1];
    }

    // Get the z size.
    index = readFileBufferString.find(MSQ_IM_SIZ);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_IM_SIZ;
      std::istringstream im_sizString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_sizString)
      {
        d3proc_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$IM_SIZ" << ". File is " << filed3proc);

        return 0;
      }
      im_sizString >> imageDim[2];
    }

    // Get the t size
    index = readFileBufferString.find(MSQ_IM_SIT);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_IM_SIT;
      std::istringstream im_sitString(
          readFileBufferString.substr(index + tempString.length()));
      if (!im_sitString)
      {
        d3proc_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$IM_SIT" << ". File is " << filed3proc);

        return 0;
      }
      im_sitString >> imageDim[3];
    }

  }
  d3proc_InputStream.close();

  std::ifstream reco_InputStream;
  reco_InputStream.open(filereco.c_str(), std::ios::in);
  if (reco_InputStream.fail())
  {
    vtkErrorMacro("reco file: " << filereco << " cannot be opened");
    return 0;
  }
  while (!reco_InputStream.eof())
  {
    reco_InputStream.getline(readFileBuffer, sizeof(readFileBuffer));
    readFileBufferString = readFileBuffer;

    // Set number of dimensions and get fov.
    index = readFileBufferString.find(MSQ_RECO_fov);
    if (index != std::string::npos)
    {
      tempIndex = readFileBufferString.find("2");
      if (tempIndex != std::string::npos)
      {
        reco_InputStream >> imageFOV[0] >> imageFOV[1];
        imageFOV[2] = 0;
        numDimensions = true;
      }
      else
      {
        tempIndex = readFileBufferString.find("3");
        if (tempIndex != std::string::npos)
        {
          reco_InputStream >> imageFOV[0] >> imageFOV[1] >> imageFOV[2];
          numDimensions = true;
        }
        else
        {
          reco_InputStream.close();
          vtkErrorMacro(
              "Invalid reco file: Couldn't locate proper " << "fov parameters" << "Reco file is " << filereco);
          return 0;
        }
      }
    }

    // Get data type
    index = readFileBufferString.find(MSQ_RECO_wordtype);
    if (index != std::string::npos)
    {
      tempIndex = readFileBufferString.find(MSQ_BRUKER_SIGNED_CHAR);
      if (tempIndex != std::string::npos)
      {
        this->SetDataScalarTypeToUnsignedChar();
      }
      else
      {
        tempIndex = readFileBufferString.find(MSQ_BRUKER_UNSIGNED_CHAR);
        if (tempIndex != std::string::npos)
        {
          this->SetDataScalarTypeToUnsignedChar();
        }
        else
        {
          tempIndex = readFileBufferString.find(MSQ_BRUKER_SIGNED_SHORT);
          if (tempIndex != std::string::npos)
          {
            this->SetDataScalarTypeToShort();
          }
          else
          {
            tempIndex = readFileBufferString.find(MSQ_BRUKER_SIGNED_INT);
            if (tempIndex != std::string::npos)
            {
              this->SetDataScalarTypeToInt();
            }
            else
            {
              tempIndex = readFileBufferString.find(MSQ_BRUKER_FLOAT);
              if (tempIndex != std::string::npos)
              {
                this->SetDataScalarTypeToFloat();
              }
              else
              {
                reco_InputStream.close();
                vtkErrorMacro(
                    "Invalid reco file: Couldn't locate proper " << "wordtype parameter" << "Reco file is " << filereco);
                return 0;
              }
            }
          }
        }
      }
    }

    // OK, handle RECO_transposition!
    index = readFileBufferString.find(MSQ_RECO_transposition);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_RECO_transposition;
      std::istringstream recoTransposeString(
          readFileBufferString.substr(index + tempString.length()));
      if (!recoTransposeString)
      {
        reco_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$RECO_transposition" << "Reco file is " << filereco);
        return 0;
      }
      recoTransposeString >> numRecoTranspose;
      if (numRecoTranspose > 0)
      {
        recoTransposition.resize(numRecoTranspose);
        for (unsigned int i = 0; i < (unsigned int) numRecoTranspose; i++)
        {
          reco_InputStream >> recoTransposition[i];
        }
      }
    }

    // Set byte order
    index = readFileBufferString.find(MSQ_RECO_byte_order);
    if (index != std::string::npos)
    {
      tempIndex = readFileBufferString.find(MSQ_BRUKER_LITTLE_ENDIAN);
      if (tempIndex != std::string::npos)
      {
        SetDataByteOrder(VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN);
      }
      else
      {
        tempIndex = readFileBufferString.find(MSQ_BRUKER_BIG_ENDIAN);
        if (tempIndex != std::string::npos)
        {
          SetDataByteOrder(VTK_FILE_BYTE_ORDER_BIG_ENDIAN);
        }
        else
        {
          reco_InputStream.close();
          vtkWarningMacro(
              "Invalid reco file: Couldn't locate " << "'##$RECO_byte_order=' tag" << "Assuming little endian. Reco file is " << filereco);
          SetDataByteOrder(VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN);
        }
      }
      byteOrder = true;
    }
  }

  reco_InputStream.close();

  if (!numDimensions)
  {
    vtkErrorMacro(
        "Invalid reco file: Couldn't locate " << "'##$RECO_fov=(' tag" << "Reco file is " << filereco);
    return 0;
  }

  if (!byteOrder)
  {
    vtkErrorMacro(
        "Invalid reco file: Couldn't locate " << "'##$RECO_byte_order=' tag" << "Reco file is " << filereco);
    return 0;
  }

  if (numRecoTranspose < 0)
  {
    vtkErrorMacro(
        "Invalid reco file: Couldn't locate " << "'##$RECO_transposition=(' tag" << "Reco file is " << filereco);
    return 0;
  }

  // Open the acqp file & extract relevant info.
  std::ifstream acqp_InputStream;
  std::string acqpFileString = "";
  acqp_InputStream.open(fileacqp.c_str(), std::ios::in);

  if (acqp_InputStream.fail())
  {
    // try the alternate directory (two dirs up)
    fileacqp = pathalt + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR + MSQ_ACQP_FILE;

    acqp_InputStream.open(fileacqp.c_str(), std::ios::in);

    if (acqp_InputStream.fail())
    {

      vtkErrorMacro("acqp file cannot be opened. " << "File is " << fileacqp);
      return 0;
    }

    // assume method file is also two dirs up
    filemethod = pathalt + MSQ_FORWARDSLASH_DIRECTORY_SEPARATOR + MSQ_METHOD_FILE;
  }

  while (!acqp_InputStream.eof())
  {
    acqp_InputStream.getline(readFileBuffer, sizeof(readFileBuffer));

    acqpFileString = readFileBuffer;

    // Get ACQ_dim.
    index = acqpFileString.find(MSQ_ACQ_dim);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_ACQ_dim;
      std::istringstream acqDimString(
          acqpFileString.substr(index + tempString.length()));
      if (!acqDimString)
      {
        acqp_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$ACQ_dim. " << "The file is " << fileacqp);
        return 0;
      }
      acqDimString >> acq_dim;
    }

    //Get the slice thickness
    index = acqpFileString.find(MSQ_ACQ_slice_thick);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_ACQ_slice_thick;
      std::istringstream sliceThickString(
          acqpFileString.substr(index + tempString.length()));
      if (!sliceThickString)
      {
        acqp_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$ACQ_slice_thick" << "The file is " << fileacqp);
        return 0;
      }
      sliceThickString >> sliceThick;
      sliceThickness = true;
    }

    // Get the slice seperation.
    index = acqpFileString.find(MSQ_ACQ_slice_sepn);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_ACQ_slice_sepn;
      std::istringstream sliceSepString(
          acqpFileString.substr(index + tempString.length()));
      if (!sliceSepString)
      {
        acqp_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$ACQ_slice_sepn" << "The file is " << fileacqp);
        return 0;
      }
      sliceSepString >> numSeperation;
      if (numSeperation > 0)
      {
        std::vector<double> imageSliceSeperation(numSeperation);
        for (unsigned int i = 0; i < (unsigned int) numSeperation; i++)
        {
          acqp_InputStream >> imageSliceSeperation[i];
        }
      }
    }

    // Get direction cosines.
    index = acqpFileString.find(MSQ_ACQ_grad_matrix);
    if (index != std::string::npos)
    {
      std::string tempString = MSQ_ACQ_grad_matrix;
      int numMatrix = 0, dim1 = 0, dim2 = 0;
      tempString = acqpFileString.substr(index + tempString.length());
      // MS VC++ cannot handle commas, so replace with spaces.
      for (std::string::iterator iter = tempString.begin(); iter != tempString.end();
          iter++)
      {
        if (*iter == ',')
        {
          *iter = ' ';
        }
      }
      std::istringstream gradMatrixString(tempString);
      if (!gradMatrixString)
      {
        acqp_InputStream.close();
        vtkErrorMacro(
            "Could not create std::istringstream for " << "##$ACQ_grad_matrix" << "The file is " << fileacqp);
        return 0;
      }
      gradMatrixString >> numMatrix >> dim1 >> dim2;
      if (numMatrix && (dim1 == 3) && (dim2 == 3))
      {
        // OK, I need ACQ_dim at this point in the code,
        // so throw an exception if I don't have it.
        if (acq_dim < 0)
        {
          acqp_InputStream.close();
          vtkErrorMacro(
              "Invalid acqp file: Couldn't locate " << "'##$ACQ_dim=' tag" << "The file is " << fileacqp);
          return 0;
        }
        int i = 0;
        for (i = 0; i < 3; i++)
        {
          acqp_InputStream >> dirx[i];
          if (dirx[i] == -0)
          {
            dirx[i] = 0;
          }
        }
        for (i = 0; i < 3; i++)
        {
          acqp_InputStream >> diry[i];
          if (diry[i] == -0)
          {
            diry[i] = 0;
          }
        }
        for (i = 0; i < 3; i++)
        {
          acqp_InputStream >> dirz[i];
          if (dirz[i] == -0)
          {
            dirz[i] = 0;
          }
        }
        // Ok, now that the directions are read in transpose if necessary.
        if ((acq_dim == 2) && (numRecoTranspose == numMatrix) && recoTransposition[0])
        {
          // Transpose read/phase.
          transpose = 1;
          std::vector<double> temp(3, 0);
          for (i = 0; i < 3; i++)
          {
            temp[i] = dirx[i];
            dirx[i] = diry[i];
            diry[i] = temp[i];
          }
        }
        else if (recoTransposition[0] == 1)
        {
          // Transpose read/phase.
          transpose = 1;
          std::vector<double> temp(3, 0);
          for (i = 0; i < 3; i++)
          {
            temp[i] = dirx[i];
            dirx[i] = diry[i];
            diry[i] = temp[i];
          }
        }
        else if (recoTransposition[0] == 2)
        {
          // Transpose phase/slice.
          transpose = 2;
          std::vector<double> temp(3, 0);
          for (i = 0; i < 3; i++)
          {
            temp[i] = diry[i];
            diry[i] = dirz[i];
            dirz[i] = temp[i];
          }
        }
        else if (recoTransposition[0] == 3)
        {
          // Transpose read/slice.
          transpose = 3;
          std::vector<double> temp(3, 0);
          for (i = 0; i < 3; i++)
          {
            temp[i] = dirx[i];
            dirx[i] = dirz[i];
            dirz[i] = temp[i];
          }
        }
        // Check to see if all of the slices are in the same orientation.
        // If not then only use the first slice (may change this behavior later).
        if ((numMatrix - 1) > 0)
        {
          std::vector<double> gradMatrixX(3, 0);
          std::vector<double> gradMatrixY(3, 0);
          std::vector<double> gradMatrixZ(3, 0);
          for (int j = 0; j < (numMatrix - 1); j++)
          {
            int l = 0;
            for (l = 0; l < 3; l++)
            {
              acqp_InputStream >> gradMatrixX[l];
              if (gradMatrixX[l] == -0)
              {
                gradMatrixX[l] = 0;
              }
            }
            for (l = 0; l < 3; l++)
            {
              acqp_InputStream >> gradMatrixY[l];
              if (gradMatrixY[l] == -0)
              {
                gradMatrixY[l] = 0;
              }
            }
            for (l = 0; l < 3; l++)
            {
              acqp_InputStream >> gradMatrixZ[l];
              if (gradMatrixZ[l] == -0)
              {
                gradMatrixZ[l] = 0;
              }
            }
            // Transpose if necessary.
            if ((acq_dim == 2) && recoTransposition[j + 1])
            {
              // Transpose read/phase.
              std::vector<double> temp(3, 0);
              for (i = 0; i < 3; i++)
              {
                temp[i] = gradMatrixX[i];
                gradMatrixX[i] = gradMatrixY[i];
                gradMatrixY[i] = temp[i];
              }
            }
            else if (recoTransposition[j + 1] == 1)
            {
              // Transpose read/phase.
              std::vector<double> temp(3, 0);
              for (i = 0; i < 3; i++)
              {
                temp[i] = gradMatrixX[i];
                gradMatrixX[i] = gradMatrixY[i];
                gradMatrixY[i] = temp[i];
              }
            }
            else if (recoTransposition[j + 1] == 2)
            {
              // Transpose phase/slice.
              std::vector<double> temp(3, 0);
              for (i = 0; i < 3; i++)
              {
                temp[i] = gradMatrixY[i];
                gradMatrixY[i] = gradMatrixZ[i];
                gradMatrixZ[i] = temp[i];
              }
            }
            else if (recoTransposition[j + 1] == 3)
            {
              // Transpose read/slice.
              std::vector<double> temp(3, 0);
              for (i = 0; i < 3; i++)
              {
                temp[i] = gradMatrixX[i];
                gradMatrixX[i] = gradMatrixZ[i];
                gradMatrixZ[i] = temp[i];
              }
            }
            // Compare with original
            if (!std::equal(dirx.begin(), dirx.end(), gradMatrixX.begin())
                || !std::equal(diry.begin(), diry.end(), gradMatrixY.begin())
                || !std::equal(dirz.begin(), dirz.end(), gradMatrixZ.begin()))
            {
              slicesNotInSameOrientation = true;
              break;
            }
          }
        }
      }
      else
      {
        acqp_InputStream.close();
        vtkErrorMacro(
            "Could not retrieve ##$ACQ_grad_matrix" << "The file is " << fileacqp);
        return 0;
      }
    }
  }
  acqp_InputStream.close();

  // try opening method file for details on the sequence
  std::ifstream method_InputStream;
  std::string methodFileString = "";
  method_InputStream.open(filemethod.c_str(), std::ios::in);
  if (method_InputStream.fail())
  {
    vtkWarningMacro("method file: " << filemethod << " cannot be opened. Skipping.");
  }
  else
  {
    while (!method_InputStream.eof())
    {
      method_InputStream.getline(readFileBuffer, sizeof(readFileBuffer));
      methodFileString = readFileBuffer;

      // Get Method.
      index = methodFileString.find(MSQ_Method_DiffGrad);
      if (index != std::string::npos)
      {
        diffusionEPI = true;
      }

      // Get number of diffusion directions
      index = methodFileString.find(MSQ_Method_NoDiffExp);
      if (index != std::string::npos)
      {
        // Get number of gradient directions
        std::string tempString = MSQ_Method_NoDiffExp;
        std::istringstream methodDiffExpString(
            methodFileString.substr(index + tempString.length()));

        if (methodDiffExpString)
        {
          methodDiffExpString >> diffusionDirs;
        }
      }

      // Get slice orientation
      index = methodFileString.find(MSQ_PVM_SliceOrient);
      if (index != std::string::npos)
      {
        method_InputStream.getline(readFileBuffer, sizeof(readFileBuffer));
        methodFileString = readFileBuffer;

        // Check if is is axial
        tempIndex = methodFileString.find("axial");
        if (tempIndex != std::string::npos)
        {
          this->MedicalImageProperties->SetOrientationType(
              vtkMedicalImageProperties::AXIAL);
        }
        else
        {
          tempIndex = methodFileString.find("coronal");
          if (tempIndex != std::string::npos)
          {
            this->MedicalImageProperties->SetOrientationType(
                vtkMedicalImageProperties::CORONAL);
          }
          else
            this->MedicalImageProperties->SetOrientationType(
                vtkMedicalImageProperties::SAGITTAL);
        }
      }

      // Get spacing
      index = methodFileString.find(MSQ_PVM_SpatResol);
      if (index != std::string::npos)
      {
        tempIndex = methodFileString.find("2");
        if (tempIndex != std::string::npos)
        {
          method_InputStream >> imageRes[0] >> imageRes[1];
          imageRes[2] = 0;
        }
        else
        {
          tempIndex = methodFileString.find("3");
          if (tempIndex != std::string::npos)
          {
            method_InputStream >> imageRes[0] >> imageRes[1] >> imageRes[2];
          }
        }
      }
    }

    method_InputStream.close();
  }

  if (!sliceThickness)
  {
    vtkErrorMacro(
        "Invalid acqp file: Couldn't locate " << "'##$ACQ_slice_thick=' tag" << "The file is " << fileacqp);
    return 0;
  }

  // For diffusion sequences 
  if (diffusionEPI && diffusionDirs)
  {
    imageDim[2] /= diffusionDirs;
    imageDim[3] = diffusionDirs;
  }

  // This is definitely a hack that will not always be correct, but should work
  // for Bruker images that have been acquired as homogeneous volumes.
  if (imageFOV[2] == 0)
  {
    imageFOV[2] = imageDim[2] * sliceThick;
    imageFOV[2] /= 10.0f; //Convert from mm to cm.
  }

  if (slicesNotInSameOrientation)
  {
    imageDim[2] = 1;
  }

  // Transpose the dims and FOV if required.
  printf("transpose=%d\n", transpose);
  switch (transpose)
  {
    case 1:
    {
      double tempFOV;
      tempFOV = imageFOV[0];
      imageFOV[0] = imageFOV[1];
      imageFOV[1] = tempFOV;
    }
      break;
    case 2:
    {
      double tempFOV;
      tempFOV = imageFOV[1];
      imageFOV[1] = imageFOV[2];
      imageFOV[2] = tempFOV;
    }
      break;
    case 3:
    {
      double tempFOV;
      tempFOV = imageFOV[0];
      imageFOV[0] = imageFOV[2];
      imageFOV[2] = tempFOV;
    }
      break;
  }

  // convert FOV to mm
  imageFOV[0] *= 10.0f;
  imageFOV[1] *= 10.0f;
  imageFOV[2] *= 10.0f;

  // set image information
  this->SetFileDimensionality(3);

  // set up image dimensions, spacing, components, etc
  this->SetDataOrigin(0, 0, 0);
  this->SetDataExtent(0, imageDim[0] - 1, 0, imageDim[1] - 1, 0, imageDim[2] - 1);

  this->SetDataSpacing(imageFOV[0] / (double) imageDim[0],
      imageFOV[1] / (double) imageDim[1], imageFOV[2] / (double) imageDim[2]);

  // set number of components
  this->SetNumberOfScalarComponents(imageDim[3]);

  // call father to finish up
  return this->Superclass::RequestInformation(request, inputVector, outputVector);
}

/***********************************************************************************//**
 * This function reads in one data of data.
 * templated to handle different data types.
 */
template<class OT>
void vtkmsqBruker2DSEQReaderUpdate(vtkmsqBruker2DSEQReader *self, vtkImageData *data,
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
void vtkmsqBruker2DSEQReader::ExecuteData(vtkDataObject *output, vtkInformation *outInfo)
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
  std::string imagefilename(this->FileName);

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

  data->GetPointData()->GetScalars()->SetName("Bruker2DSEQImage");

  vtkDebugMacro(
      "Reading extent: " << ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]);

  // Call the correct templated function for the output
  ptr = data->GetScalarPointer();
  switch (this->GetDataScalarType())
  {
    vtkTemplateMacro(vtkmsqBruker2DSEQReaderUpdate(this, data, (VTK_TT *)(ptr), zfp));
    default:
      vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
  }

  // close file
  gzclose(zfp);
}

/***********************************************************************************//**
 * 
 */
void vtkmsqBruker2DSEQReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

