#include "vtkmsqImageInterleaving.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"

#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "math.h"

vtkStandardNewMacro(vtkmsqImageInterleaving);

// Construct object with no children.
vtkmsqImageInterleaving::vtkmsqImageInterleaving()
{
  this->NumberOfFrames = 1;
}

// ----------------------------------------------------------------------------
int vtkmsqImageInterleaving::RequestData(vtkInformation *vtkNotUsed(request),
                                         vtkInformationVector **inputVector,
                                         vtkInformationVector *outputVector)
{
  // Get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
 
  // Get the input and ouptut
  vtkImageData *input = vtkImageData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  vtkImageData *output = vtkImageData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  int extent[6];
  input->GetExtent(extent);
  int nc=input->GetNumberOfScalarComponents();

  //printf("%d %d %d %d %d %d\n",extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);

  output->SetNumberOfScalarComponents(this->NumberOfFrames);
  extent[5] = (extent[5] + 1) / this->NumberOfFrames - 1;

  //printf("%d %d %d %d %d %d\n",extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);

  output->SetExtent(extent);
  output->SetUpdateExtent(output->GetExtent());
  output->SetWholeExtent(output->GetExtent());

  output->AllocateScalars();
  
  this->SimpleExecute(input, output);
 
  return 1;
}

// ----------------------------------------------------------------------------
template <class IT>
void vtkmsqImageInterleavingExecute(vtkImageData* input,
                                    vtkImageData* output,
                                    IT* inPtr, IT* outPtr, 
                                    int NumberOfFrames)
{
  int dims[3];
  output->GetDimensions(dims);

  if (input->GetScalarType() != output->GetScalarType())
    {
      vtkGenericWarningMacro(<< "Execute: input ScalarType, " << input->GetScalarType()
      << ", must match out ScalarType " << output->GetScalarType());
      return;
    }

  vtkDataArray* in =input->GetPointData()->GetScalars();
  vtkDataArray* out=output->GetPointData()->GetScalars();

  int count = 0;
  int framesize = dims[0]*dims[1]*dims[2];

  for(int comp = 0; comp < NumberOfFrames; comp++)
  {
    int index = 0;
    while(index < framesize)
    {
      out->SetComponent(index, comp, in->GetComponent(count, 0));
      
      index++;
      count++;
    }
     
  }

}

// ----------------------------------------------------------------------------
void vtkmsqImageInterleaving::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  void *inPtr = input->GetScalarPointer();
  void *outPtr = output->GetScalarPointer();
  
  switch(output->GetScalarType())
  {
    // This is simply a #define for a big case list. It handles all
    // data types VTK supports.
    vtkTemplateMacro(
      vtkmsqImageInterleavingExecute(input, output,
                                     static_cast<VTK_TT *>(inPtr),
                                     static_cast<VTK_TT *>(outPtr),
                                     this->NumberOfFrames));
    default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
  }
} 

// ----------------------------------------------------------------------------
