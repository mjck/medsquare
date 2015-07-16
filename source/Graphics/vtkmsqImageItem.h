#ifndef VTKMSQ_IMAGE_ITEM_H
#define VTKMSQ_IMAGE_ITEM_H

#include "vtkObject.h"

#include "vtkmsqGraphicsWin32Header.h"

class vtkmsqMedicalImageProperties;
class vtkmsqLookupTable;

class vtkImageData;
class vtkMatrix4x4;

class VTK_MSQ_GRAPHICS_EXPORT vtkmsqImageItem: public vtkObject
{
public:
  static vtkmsqImageItem *New();

  void PrintSelf(ostream &os, vtkIndent indent);
  vtkTypeMacro(vtkmsqImageItem, vtkObject);

  vtkGetMacro(Image, vtkImageData*);
  vtkSetMacro(Image, vtkImageData*);

  vtkGetMacro(Properties, vtkmsqMedicalImageProperties*);
  vtkSetMacro(Properties, vtkmsqMedicalImageProperties*);

  vtkGetMacro(Colormap, vtkmsqLookupTable*);
  void SetColormap(vtkmsqLookupTable *colormap);

  vtkMatrix4x4* FindTranslationToCenter(double multiplier = 1.0);

  // Description:
  // Compute the reslicing matrix for this image plane to select the given slice
  vtkMatrix4x4* FindReslicingMatrix(int slice, vtkMatrix4x4 *planeOrientationMatrix);
  vtkMatrix4x4* FindReslicingMatrix2(int slice, vtkMatrix4x4 *planeOrientationMatrix);

protected:
  vtkmsqImageItem();
  virtual ~vtkmsqImageItem();

  vtkImageData *Image;
  vtkmsqMedicalImageProperties *Properties;
  vtkmsqLookupTable *Colormap;

  vtkmsqLookupTable* defaultColormap();

private:
  vtkmsqImageItem(const vtkmsqImageItem&); // Not implemented.
  void operator=(const vtkmsqImageItem&); // Not implemented.
};

#endif
