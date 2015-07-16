// .NAME vtkmsqMedicalImageProperties - some medical image properties.
// .SECTION Description
// vtkmsqMedicalImageProperties is a helper class that can be used by medical
// image readers and applications to encapsulate medical image/acquisition
// properties. 

#ifndef __vtkmsqMedicalImageProperties_h
#define __vtkmsqMedicalImageProperties_h

// Thou shalt never eliminate this define or any of the ifs related to it.
// This define is important for debug purposes: it is much easier to debug the code without reorientation.
// This define is important for usability purposes: it may be necessary to give the user the option of using reorientation or not.
// After changing this value please "make clean".
#define MSQ_REORIENT 1

#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkDoubleArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkSmartPointer.h"

#include "vtkMedicalImageProperties.h"
#include "vtkmsqIOWin32Header.h"

class vtkMedicalImagePropertiesInternals;

class VTK_MSQ_IO_EXPORT vtkmsqMedicalImageProperties: public vtkMedicalImageProperties
{
public:
  static vtkmsqMedicalImageProperties *New();
  vtkTypeRevisionMacro(vtkmsqMedicalImageProperties,vtkMedicalImageProperties);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set orientation type
  virtual void SetOrientationType(int orient);
  virtual int GetOrientationType();

  vtkSmartPointer<vtkMatrix4x4> GetDirectionCosineMatrix();
  vtkMatrix4x4* GetDirectionCosineMatrixPerpendicular();
  void GetReorientedExtent(int original[6], int reoriented[6], int absolute = 1);
  void GetReorientedDouble3(double original[3], double reoriented[3], int absolute = 1);
  void GetOriginalDouble3(double reoriented[3], double original[3]);

  // Description:
  // Copy the contents of src to this instance.
  virtual void DeepCopy(vtkMedicalImageProperties *src);

  // Description:
  // Get/Set number of diffusion MR gradient orientations
  vtkGetMacro(NumberOfGradientValues, int);
  vtkSetMacro(NumberOfGradientValues, int);
  
  // Description:
  // Auxiliary functions for manipulating the gradient/b-value array
  virtual void AddDiffusionGradient(double *values);
  virtual void AddDiffusionGradient(vtkstd::vector<float> gradient, float bvalue); 
  virtual void GetNthDiffusionGradient(int idx, double *values);
  
  // Description:
  // Get/Set number of diffusion MR b-values
  vtkGetMacro(NumberOfBValues, int);
  vtkSetMacro(NumberOfBValues, int);
  
protected:
  vtkmsqMedicalImageProperties();
  ~vtkmsqMedicalImageProperties();

  int NumberOfGradientValues;
  int NumberOfBValues;
  vtkDoubleArray *DiffusionGradients;  // 0:gx, 1:gy, 2:gz, 3:b-value

private:
  vtkmsqMedicalImageProperties(const vtkmsqMedicalImageProperties&); // Not implemented.
  void operator=(const vtkmsqMedicalImageProperties&); // Not implemented.
};

#endif
