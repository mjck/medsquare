#ifndef __vtkmsqRawHeader_h
#define __vtkmsqRawHeader_h 

#include "vtkObject.h"
#include "vtkObjectFactory.h"

enum rawDataType
{
  FLOAT, DOUBLE, INT, UNSIGNEDINT, SHORT, UNSIGNEDSHORT, CHAR, UNSIGNEDCHAR, SIGNEDCHAR
};

enum rawOrientation
{
  RAWAXIAL, RAWCORONAL, RAWSAGITTAL
};

class VTK_EXPORT vtkmsqRawHeader: public vtkObject
{
public:
  static vtkmsqRawHeader *New();vtkTypeRevisionMacro(vtkmsqRawHeader, vtkObject)
  ;
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set dimensions
  vtkSetVector3Macro(Dimensions, int)
  ;vtkGetVector3Macro(Dimensions, int)
  ;

  // Description:
  // Get/Set spacing
  vtkSetVector3Macro(Spacing, double)
  ;vtkGetVector3Macro(Spacing, double)
  ;

  // Description:
  // Get/Set origin
  vtkSetVector3Macro(Origin, double)
  ;vtkGetVector3Macro(Origin, double)
  ;

  // Description:
  // Determine endianess
  // By default it is little endian
  vtkGetMacro(LittleEndian, int)
  ;vtkSetMacro(LittleEndian, int)
  ;vtkBooleanMacro(LittleEndian, int)
  ;

  // Description:
  // Get/Set number of volume
  vtkGetMacro(Volume, int)
  ;vtkSetMacro(Volume, int)
  ;

  // Description:
  // Get/Set orientation
  vtkGetMacro(Orientation, rawOrientation)
  ;vtkSetMacro(Orientation, rawOrientation)
  ;

  // Description:
  // Get/Set data type
  vtkGetMacro(Type, rawDataType)
  ;vtkSetMacro(Type, rawDataType)
  ;

  // Description:
  // Get/Set data type
  vtkGetMacro(Offset, int)
  ;vtkSetMacro(Offset, int)
  ;

protected:
  int Dimensions[3];
  double Spacing[3];
  double Origin[3];
  bool LittleEndian;
  int Volume;
  rawOrientation Orientation;
  rawDataType Type;
  int Offset;

  vtkmsqRawHeader();
  ~vtkmsqRawHeader()
  {
  }
  ;

private:
  vtkmsqRawHeader(const vtkmsqRawHeader&); // Not implemented.
  void operator=(const vtkmsqRawHeader&); // Not implemented.
};

#endif
