/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqAnalyzeWriterTest.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqAnalyzeWriter.h"

#include "vtkmsqRawReader.h"
#include "vtkmsqAnalyzeReader.h"

#include "vtkMedicalImageProperties.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"

#include <string>
#include "gtest/gtest.h"

#define TEST_DATA_DIR "Data/"
#define TEST_FILENAME "imagem2D"
#define REFERENCE_FILENAME "imagem2D_test"

class vtkmsqAnalyzeWriterTest: public testing::Test
{
protected:
  virtual void SetUp()
  {
    imageReader = vtkSmartPointer<vtkmsqRawReader>::New();
    imageReader->SetFileName(TEST_DATA_DIR TEST_FILENAME ".raw");
    imageReader->SetDataExtent(0, 99, 0, 99, 0, 99);
    imageReader->SetDataSpacing(1, 1, 1);
    imageReader->SetDataOrigin(0, 0, 0);
    imageReader->SetFileDimensionality(3);
    imageReader->SetDataByteOrderToLittleEndian();
    imageReader->SetOrientation(RAWAXIAL);
    imageReader->SetDataScalarTypeToUnsignedChar();
    imageReader->UpdateWholeExtent();

    testImage = vtkImageData::New();
    testImage->ShallowCopy(imageReader->GetOutput());

    analyzeReader = vtkSmartPointer<vtkmsqAnalyzeReader>::New();
    analyzeReader->SetFileName(TEST_DATA_DIR REFERENCE_FILENAME ".hdr");

    analyzeWriter = vtkSmartPointer<vtkmsqAnalyzeWriter>::New();
    analyzeWriter->SetFileName(TEST_DATA_DIR REFERENCE_FILENAME ".hdr");

    analyzeWriter->SetMedicalImageProperties(imageReader->GetMedicalImageProperties());
    analyzeWriter->SetInput(testImage);
  }

  virtual void TearDown()
  {
    testImage->Delete();
  }

  template<typename T>
  static bool areEqual(T* t1, T* t2, int elements)
  {
    for (int i = 0; i < elements; i++)
    {
      if (t1[i] != t2[i])
      {
        return false;
      }
    }
    return true;
  }

  vtkSmartPointer<vtkmsqRawReader> imageReader;
  vtkSmartPointer<vtkmsqAnalyzeReader> analyzeReader;
  vtkSmartPointer<vtkmsqAnalyzeWriter> analyzeWriter;
  vtkImageData *testImage;
};

TEST_F(vtkmsqAnalyzeWriterTest, CanWriteFileOpenedByRawReader)
{
  EXPECT_NE(0, analyzeWriter->CanWriteFile(TEST_DATA_DIR REFERENCE_FILENAME ".hdr"));

  analyzeWriter->Write();

  EXPECT_NE(0, analyzeReader->CanReadFile(TEST_DATA_DIR REFERENCE_FILENAME ".hdr"));

  analyzeReader->UpdateWholeExtent();

  vtkImageData *imageDataTmp = vtkImageData::New();
  imageDataTmp->ShallowCopy(analyzeReader->GetOutput());

  EXPECT_EQ(VTK_UNSIGNED_CHAR, imageDataTmp->GetScalarType());

  double *spacing = imageDataTmp->GetSpacing();
  double expectedSpacing[] = { 1.0, 1.0, 1.0 };
  EXPECT_TRUE(areEqual(spacing, expectedSpacing, 3));

  int *dimensions = imageDataTmp->GetDimensions();
  int expectedDimensions[] = { 100, 100, 100 };
  EXPECT_TRUE(areEqual(dimensions, expectedDimensions, 3));

  for (int k = 0; k < 100; k++)
  {
    for (int j = 0; j < 100; j++)
    {
      for (int i = 0; i < 100; i++)
      {
        EXPECT_EQ(testImage->GetScalarComponentAsDouble(i, j, k, 0),
            imageDataTmp->GetScalarComponentAsDouble(i, j, k, 0));
      }
    }
  }

}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
