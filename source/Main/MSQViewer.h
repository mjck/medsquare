#ifndef MSQ_VIEWER_H
#define MSQ_VIEWER_H

#include <QWidget>

class MSQGeometryItem;
class vtkmsqImageItem;
class vtkImageData;
class vtkmsqMedicalImageProperties;
class vtkmsqLookupTable;

class MSQViewer : public QWidget
{
Q_OBJECT

public:
  virtual void setInput(vtkmsqImageItem *newImageItem) = 0;
  virtual void setCurrentColormap(vtkmsqLookupTable *newColormap) = 0;

  virtual void addGeometry(MSQGeometryItem *item) = 0;
  virtual void removeGeometry(MSQGeometryItem *item) = 0;
  virtual void updateGeometry(MSQGeometryItem *item)=0;
  
  virtual void zoomIn() = 0;
  virtual void zoomOut() = 0;
  virtual void reset() = 0;  

  virtual void refresh() = 0;
};

#endif
