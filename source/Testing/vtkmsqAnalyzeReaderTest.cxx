/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqAnalyzeReaderTest.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vtkmsqAnalyzeReader.h"

#include "vtkMedicalImageProperties.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"

#include <string>
#include "gtest/gtest.h"

#define TEST_DATA_DIR "Data/"
#define TEST_FILENAME "analyze3x4x5"

class vtkmsqAnalyzeReaderTest: public testing::Test
{
protected:
  virtual void SetUp()
  {
    imageReader = vtkSmartPointer<vtkmsqAnalyzeReader>::New();
    imageReader->SetFileName(TEST_DATA_DIR TEST_FILENAME ".hdr");

  }

  virtual void TearDown()
  {

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

  vtkSmartPointer<vtkmsqAnalyzeReader> imageReader;
};

TEST_F(vtkmsqAnalyzeReaderTest, CanReadFileOpenedByAnalyzeReader)
{
  EXPECT_NE(0, imageReader->CanReadFile(TEST_DATA_DIR TEST_FILENAME ".hdr"));

  imageReader->UpdateWholeExtent();

  vtkSmartPointer<vtkImageData> imageDataTmp = vtkSmartPointer<vtkImageData>::New();
  imageDataTmp->ShallowCopy(imageReader->GetOutput());

  vtkSmartPointer<vtkmsqMedicalImageProperties> imageProps;

  imageProps = imageReader->GetMedicalImageProperties();

  EXPECT_EQ(vtkMedicalImageProperties::CORONAL, imageProps->GetOrientationType());

  EXPECT_EQ(VTK_UNSIGNED_CHAR, imageDataTmp->GetScalarType());

  double *spacing = imageDataTmp->GetSpacing();
  int expectedSpacing[] = { 3, 4, 5 };

  int spacingTrunc[] = { (int) (spacing[0] * 10), (int) (spacing[1] * 10),
      (int) (spacing[2] * 10) };

  EXPECT_TRUE(areEqual(spacingTrunc, expectedSpacing, 3));

  int *dimensions = imageDataTmp->GetDimensions();
  int expectedDimensions[] = { 3, 4, 5 };
  EXPECT_TRUE(areEqual(dimensions, expectedDimensions, 3));

  int counter = 0;
  for (int k = 0; k < 5; k++)
  {
    for (int j = 0; j < 4; j++)
    {
      for (int i = 0; i < 3; i++)
      {
        EXPECT_EQ(imageDataTmp->GetScalarComponentAsDouble(i, j, k, 0), counter);
        counter++;
      }
    }
  }

}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
