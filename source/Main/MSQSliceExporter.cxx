#include "MSQSliceExporter.h"

#include "MedSquare.h"
#include "vtkmsqImageItem.h"
#include "vtkmsqMedicalImageProperties.h"

#include <vtkImageReslice.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkBMPWriter.h>
#include <vtkImageMapToColors.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkmsqLookupTable.h>
#include <vtkmsqImagePlane.h>

#include <QList>
#include <QString>

MSQSliceExporter::MSQSliceExporter(MedSquare *medSquare) : medSquare(medSquare)
{
}

void MSQSliceExporter::exportSlices(vtkmsqImageItem *image,QString path, QString prefix, QString format)
{
  vtkSmartPointer<vtkmsqLookupTable> lookupTable = image->GetColormap();
  vtkSmartPointer<vtkImageReslice> reslicer = vtkSmartPointer<vtkImageReslice>::New();
  vtkSmartPointer<vtkImageWriter> writer;
  if (format==QString("PNG"))    
    writer = vtkSmartPointer<vtkPNGWriter>::New();
  else if(format==QString("JPEG"))  
    writer = vtkSmartPointer<vtkJPEGWriter>::New();      
  else if(format==QString("TIFF"))
    writer = vtkSmartPointer<vtkTIFFWriter>::New();
  else if(format==QString("BMP"))
    writer = vtkSmartPointer<vtkBMPWriter>::New();
  
  vtkSmartPointer<vtkImageMapToColors> colorMapper = vtkSmartPointer<vtkImageMapToColors>::New();
  QList<vtkMatrix4x4 *> imagePlanes;
  QList<QString> imagePlaneNames;
  char fileName[255];

  int extent[6];
  double spacing[3];
  image->GetImage()->GetExtent(extent);
  image->GetImage()->GetSpacing(spacing);
  if (MSQ_REORIENT)
  {
    image->GetProperties()->GetReorientedExtent(extent, extent, 0);
    image->GetProperties()->GetReorientedDouble3(spacing, spacing);
  }

  reslicer->SetInput(image->GetImage());
  reslicer->SetOutputDimensionality(2);
  reslicer->SetOutputSpacing(spacing);
  reslicer->SetInterpolationModeToLinear();
  colorMapper->SetLookupTable(lookupTable);
  colorMapper->SetOutputFormatToRGB();
  colorMapper->SetInputConnection(reslicer->GetOutputPort());
  writer->SetInputConnection(colorMapper->GetOutputPort());

  imagePlanes.append(vtkmsqImagePlane::SagittalPlaneOrientationMatrix());
  imagePlanes.append(vtkmsqImagePlane::CoronalPlaneOrientationMatrix());
  imagePlanes.append(vtkmsqImagePlane::AxialPlaneOrientationMatrix());

  imagePlaneNames.append("sagittal");
  imagePlaneNames.append("coronal");
  imagePlaneNames.append("axial");

  this->medSquare->updateStatusBar(tr("Exporting slices..."), true);

  int totalSlices = (extent[1] - extent[0]) + (extent[3] - extent[2]) + (extent[5] - extent[4]);
  int currentSlice = 0;
  for (int i = 0; i < imagePlanes.size(); i++)
  {
    for (int slice = extent[2 * i]; slice < extent[2 * i + 1]; slice++, currentSlice++)
    {
      int totalProgress = (int) ((double) currentSlice / (double) totalSlices * 100);
      if (MSQ_REORIENT)
      {
          reslicer->SetResliceAxes(image->FindReslicingMatrix2(slice, imagePlanes.at(i)));
      }
      else
      {
          reslicer->SetResliceAxes(image->FindReslicingMatrix(slice, imagePlanes.at(i)));
      }
      sprintf(fileName, "%s/%s%s_%d.%s",path.toStdString().c_str(),prefix.toStdString().c_str(), 
	      imagePlaneNames.at(i).toStdString().c_str(), slice,format.toLower().toStdString().c_str());
      writer->SetFileName(fileName);
      writer->Write();
      this->medSquare->progressBar()->setValue(totalProgress);
      QApplication::processEvents();
    }
  }

  this->medSquare->updateStatusBar(tr("Ready"), false);
}
