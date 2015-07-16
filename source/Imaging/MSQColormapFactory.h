//
// .NAME MSQColormapFactory - implement colormap routines
// .SECTION Description
// MSQColormapFactory is a class that defines routines for reading, writing and defining colormaps.

#ifndef MSQ_COLORMAP_FACTORY_H
#define MSQ_COLORMAP_FACTORY_H

#include <QObject>
#include <QVector>
#include <QXmlStreamReader>

#include "MedSquare.h"

#include "vtkObject.h"
#include "vtkImageData.h"
#include "vtkMedicalImageProperties.h"
#include "vtkColorTransferFunction.h"

#include "vtkmsqLookupTable.h"

#include <string>

// Define preset colormaps
enum MSQLutType
{
  MSQ_LUT_GRAY = 0, MSQ_LUT_HUE, MSQ_LUT_SATURATION, MSQ_LUT_HOT, MSQ_LUT_CUSTOM
};

class MSQColormapFactory : public QObject
{
Q_OBJECT

public:

  MSQColormapFactory();

  // load colormap
  vtkmsqLookupTable *loadColormap(std::string fileName);

  // load PList colormap
  vtkmsqLookupTable *loadPListColormap(std::string colormapName);

  // create preset colormap
  vtkmsqLookupTable *createColormap(MSQLutType which, int numColors);

  //create preset transferFunction
  vtkColorTransferFunction *createTransferFunction(vtkmsqLookupTable *lut, int numColors);

private:

	//int readPListArray(QXmlStreamReader &xmlReader, QVector<int>& channel);

};

#endif
