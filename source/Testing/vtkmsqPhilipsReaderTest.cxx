/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqPhilipsReaderTest.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqPhilipsRECReader.h"

#include "vtkmsqAnalyzeReader.h"

#include "vtkMedicalImageProperties.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"

#include <string>
#include "gtest/gtest.h"

#define TEST_DATA_DIR "Data/"
#define TEST_FILENAME "dummy_test"

class vtkmsqPhilipsReaderTest: public testing::Test
{
protected:
  virtual void SetUp()
  {
    imageReader = vtkSmartPointer<vtkmsqPhilipsRECReader>::New();
    imageReader->SetFileName(TEST_DATA_DIR TEST_FILENAME ".rec");
    imageReader->UpdateWholeExtent();
    testImage = vtkImageData::New();
    testImage->ShallowCopy(imageReader->GetOutput());

    vtkSmartPointer<vtkmsqAnalyzeReader> analyzeReader = vtkSmartPointer<
        vtkmsqAnalyzeReader>::New();
    analyzeReader->SetFileName(TEST_DATA_DIR TEST_FILENAME ".hdr");
    analyzeReader->UpdateWholeExtent();
    referenceImage = vtkImageData::New();
    referenceImage->ShallowCopy(analyzeReader->GetOutput());
  }

  virtual void TearDown()
  {
    referenceImage->Delete();
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

  vtkSmartPointer<vtkmsqPhilipsRECReader> imageReader;
  vtkImageData *referenceImage, *testImage;
};

TEST_F(vtkmsqPhilipsReaderTest, CanReadValidFile)
{
  EXPECT_NE(0, imageReader->CanReadFile(TEST_DATA_DIR TEST_FILENAME ".rec"));
  EXPECT_NE(0, imageReader->CanReadFile(TEST_DATA_DIR TEST_FILENAME ".par"));
}

TEST_F(vtkmsqPhilipsReaderTest, ReadCorrectlyValidFile)
{

  EXPECT_EQ(VTK_SHORT, testImage->GetScalarType());

  double *spacing = testImage->GetSpacing();
  double expectedSpacing[] = { 2.0, 2.0, 2.0 };
  EXPECT_TRUE(areEqual(spacing, expectedSpacing, 3));

  int *dimensions = testImage->GetDimensions();
  int expectedDimensions[] = { 128, 128, 60 };
  EXPECT_TRUE(areEqual(dimensions, expectedDimensions, 3));

  for (int k = 0; k < 60; k++)
  {
    for (int j = 0; j < 128; j++)
    {
      for (int i = 0; i < 128; i++)
      {
        EXPECT_EQ(referenceImage->GetScalarComponentAsDouble(i, j, k, 0),
            testImage->GetScalarComponentAsDouble(i, j, k, 0));
      }
    }
  }
}

TEST_F(vtkmsqPhilipsReaderTest, CannotReadInvalidFile)
{
  EXPECT_EQ(0, imageReader->CanReadFile(TEST_DATA_DIR TEST_FILENAME ".hdr"));
}

TEST_F(vtkmsqPhilipsReaderTest, CannotReadFileThatDoesNotExist)
{
  EXPECT_EQ(0, imageReader->CanReadFile("IDontExist.par"));
}

TEST_F(vtkmsqPhilipsReaderTest, FillsImagePropertiesCorrectlyFromValidFile)
{
  vtkMedicalImageProperties *testProperties = vtkMedicalImageProperties::New();
  testProperties->DeepCopy(imageReader->GetMedicalImageProperties());

  EXPECT_EQ(vtkMedicalImageProperties::AXIAL, testProperties->GetOrientationType(0));
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
