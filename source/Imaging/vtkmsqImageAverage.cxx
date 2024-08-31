#include "vtkmsqImageAverage.h"

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

vtkStandardNewMacro(vtkmsqImageAverage);

// Construct object with no children.
vtkmsqImageAverage::vtkmsqImageAverage()
{
}

// ----------------------------------------------------------------------------
void vtkmsqImageAverage::AddInput(vtkImageData *input)
{
 this->vtkImageAlgorithm::AddInputData(input);
}

//----------------------------------------------------------------------------
// The default vtkImageAlgorithm semantics are that SetInput() puts
// each input on a different port, we want all the image inputs to
// go on the first port.
void vtkmsqImageAverage::SetInput(int idx, vtkDataObject *input)
{
  // Ask the superclass to connect the input.
  //this->SetNthInputConnection(0, idx, (input ? input->GetOuputPort() : 0));
}

//----------------------------------------------------------------------------
//void vtkmsqImageAverage::SetInput(int idx, vtkImageData *input)
//{
//  this->vtkImageAlgorithm::SetInput(idx, input);
//}

//----------------------------------------------------------------------------
vtkImageData *vtkmsqImageAverage::GetInput()
{
  return this->GetInput(0);
}

//----------------------------------------------------------------------------
vtkImageData *vtkmsqImageAverage::GetInput(int idx)
{
  if (this->GetNumberOfInputPorts() <= idx)
    {
      return NULL;
    }
  
  return (vtkImageData*)(this->GetInput(idx));
}

//----------------------------------------------------------------------------
 int vtkmsqImageAverage::FillInputPortInformation( int port, vtkInformation* info )
 {
  if(port == 0)
  {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData" );
    info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
    return 1;
  }
 
  vtkErrorMacro("This filter does not have more than 1 input port!");
  return 0;
}

//----------------------------------------------------------------------------
// int vtkmsqImageAverage::RequestUpdateExtent (
//   vtkInformation * vtkNotUsed(request),
//   vtkInformationVector **inputVector,
//   vtkInformationVector *vtkNotUsed( outputVector ))
// {
//   vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

//   // always request the whole extent
//   inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
//               inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()),6);

//   return 1;
// }

// ----------------------------------------------------------------------------
int vtkmsqImageAverage::RequestData(vtkInformation *vtkNotUsed(request),
                                         vtkInformationVector **inputVector,
                                         vtkInformationVector *outputVector)
{

  //printf("1) so far so good... ports = %d\n",this->GetNumberOfInputPorts());
  //printf("1.5) so far so good... connections = %d\n",this->GetNumberOfInputConnections(0));

  // Get the info objects
  //vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
 
  //printf("2) so far so good... ports = %d\n",this->GetNumberOfInputPorts());

  // Get the input and ouptut
  std::vector<vtkImageData *> inputs;

  for(int i=0; i < this->GetNumberOfInputConnections(0); i++) {
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(i);
    inputs.push_back(vtkImageData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT())));
  }

  //vtkImageData *input = vtkImageData::SafeDownCast(
  //    inInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  vtkImageData *output = vtkImageData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  int extent[6];
  inputs[0]->GetExtent(extent);
  int nc=inputs[0]->GetNumberOfScalarComponents();

  //printf("number of components=%d\n", nc);
  //printf("%d %d %d %d %d %d\n",extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);

  //output->SetNumberOfScalarComponents(nc);

  //printf("%d %d %d %d %d %d\n",extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);

  output->SetExtent(extent);
  //output->SetUpdateExtent(output->GetExtent());
  //output->SetWholeExtent(output->GetExtent());
  //output->SetScalarTypeToFloat();

  output->AllocateScalars(VTK_FLOAT, nc);

  //int *uExtent = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT());
  //this->AllocateOutputData(output, outInfo, uExtent);
  
  //printf("3) out here = %d\n",this->GetNumberOfInputPorts());

  this->SimpleExecute(inputs, output);
 
  return 1;
}

// ----------------------------------------------------------------------------
void vtkmsqImageAverage::SimpleExecute(std::vector<vtkImageData *> inputs, vtkImageData *output)
{
  if (this->GetNumberOfInputPorts()==0 || inputs.empty())
      {
	vtkErrorMacro(<<"No Input Image Data !!");
	return;
      }
  
  int dim_0[3];
  inputs[0]->GetDimensions(dim_0);
  
  for (int i=1;i<this->GetNumberOfInputConnections(0);i++)
    {
      if (inputs[i]==NULL)
    	{
    	  vtkErrorMacro(<<"No Input Image Data " << i << " !!");
    	  return;
    	}
      else
    	{
    	  int dim_1[3];
    	  inputs[i]->GetDimensions(dim_1);
    	  int sum=0;
    	  for (int kk=0;kk<=2;kk++)
    	    sum+=abs(dim_1[kk]-dim_0[kk]);
    	  if (sum!=0)
    	    {
    	      vtkErrorMacro(<<"Multiple Images have different Dimensions !!");
    	      return;
    	    }
    	}
    }
  

  // Create Output Array
  // -------------------
  int dim[3];  
  inputs[0]->GetDimensions(dim);
  int numframes = inputs[0]->GetNumberOfScalarComponents();
  
  vtkDebugMacro(<< "Averaging Frames\n" <<  "Dimensions " << dim[0] <<","<< dim[1] <<","<< dim[2] << "\n");

  vtkDataArray* out=output->GetPointData()->GetScalars();

  int numscalars=out->GetNumberOfTuples();
  int count = 0;
  float pog = 0.0;
  int tenth= (int)(numscalars*numframes / 10.0);

  this->UpdateProgress(0.01);

  for (int comp=0;comp<numframes;comp++)
    for (int n=0;n<numscalars;n++)      
      {
      	float sum=0.0;
      	float v;
      	//int c=0;
      	
         //printf("looping!\n");

      	for (int series=0; series<this->GetNumberOfInputConnections(0); series++)
      	  {
      	      v = inputs[series]->GetPointData()->GetScalars()->GetComponent(n,comp);	    
      	      sum += v;
      	  }
      	
      	float mean = sum / float(GetNumberOfInputConnections(0));

        //printf("writing!\n");
      	out->SetComponent(n,comp,mean);
           	
      	count++;
      	if (count==tenth)
      	  {
      	    pog+=0.1;
      	    this->UpdateProgress(pog);
      	    count=0;
      	  }
      }

  this->UpdateProgress(1.0);
}

