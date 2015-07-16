/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomImageViewer.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomImageViewer.h"

#include "QVTKWidget2.h"

#include "vtkGDCMImageReader.h"

#include "vtkActor2D.h"
#include "vtkBMPWriter.h"
#include "vtkCamera.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkTIFFWriter.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderLargeImage.h"
#include "vtkProperty2D.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"

#include "vtkmsqRectangleActor2D.h"
#include "vtkmsqInteractorStyleImage.h"

/***********************************************************************************//**
 *
 */
MSQDicomImageViewer::MSQDicomImageViewer()
{
  QGLFormat fmt;
  fmt.setAlpha(true);
  fmt.setAlphaBufferSize(8);
  vtkWidget = new QVTKWidget2(fmt);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing(0);
  layout->setContentsMargins(1, 1, 1, 1);
  layout->addWidget(vtkWidget);

  this->vtkWidget->GetRenderWindow()->SetAlphaBitPlanes(1);
  this->vtkWidget->GetRenderWindow()->SetMultiSamples(0);

  this->renderer = vtkRenderer::New();
  this->renderer->UseDepthPeelingOn();
  this->renderer->SetMaximumNumberOfPeels(100);
  this->renderer->SetOcclusionRatio(0.1);

  this->vtkWidget->GetRenderWindow()->AddRenderer(this->renderer);

  this->setStyleSheet("QWidget { background-color: black; }");
  this->setLayout(layout);
}

/***********************************************************************************//**
 *
 */
MSQDicomImageViewer::~MSQDicomImageViewer()
{
  //this->interStyle->Delete();
  this->renderer->Delete();

  delete vtkWidget;
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setInput(const QString& fileName) 
{
  vtkGDCMImageReader *reader = vtkGDCMImageReader::New();
  reader->SetFileName( fileName.toLocal8Bit().constData() );
  reader->Update();

  //vtkImageFlip *flip = vtkImageFlip::New();
  //flip->SetInput(reader->GetOutput());
  //flip->SetFilteredAxis(0);
  //flip->Update();

  //vtkImageReslice *reslice = vtkImageReslice::New();
  //reslice->SetInput(reader->GetOutput());
  //reslice->SetInput(flip->GetOutput());
  //reslice->SetResliceAxesDirectionCosines()
  //reader->GetDirectionCosines()->Print(std::cout);
  //vtkMatrix4x4 *invert = vtkMatrix4x4::New();
  //invert->DeepCopy( reader->GetDirectionCosines() );
  //invert->Invert();

  //reslice->SetResliceAxes( reader->GetDirectionCosines() );
  //reslice->SetResliceAxes( invert );
  //reslice->Update();
  vtkImageData* im = reader->GetOutput();

  vtkLookupTable* table = vtkLookupTable::New();
  table->SetNumberOfColors(1000);
  table->SetTableRange(0,1000);
  table->SetSaturationRange(0,0);
  table->SetHueRange(0,1);
  table->SetValueRange(0,1);
  table->SetAlphaRange(1,1);
  table->Build();

  // Texture
  vtkTexture* texture = vtkTexture::New();
  texture->SetInput(im);
  texture->InterpolateOn();
  texture->SetLookupTable(table);

  // PlaneSource
  vtkPlaneSource* plane = vtkPlaneSource::New();

   // PolyDataMapper
  vtkPolyDataMapper *planeMapper = vtkPolyDataMapper::New();
  planeMapper->SetInput(plane->GetOutput());
  plane->Delete();

  // Actor
  vtkActor* planeActor = vtkActor::New();
  planeActor->SetTexture(texture);
  planeActor->SetMapper(planeMapper);
  planeActor->PickableOff();
  texture->Delete();
  planeMapper->Delete();

  addActor(planeActor);

  //enableInteraction();
  reset();

  //vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  //iren->SetRenderWindow(renwin);
  //ren->AddActor(planeActor);
  //ren->SetBackground(0,0,0.5);

  // DICOM is RAH:
  //vtkAnnotatedCubeActor* cube = vtkAnnotatedCubeActor::New();
  //cube->SetXPlusFaceText ( "R" );
  //cube->SetXMinusFaceText( "L" );
  //cube->SetYPlusFaceText ( "A" );
  //cube->SetYMinusFaceText( "P" );
  //cube->SetZPlusFaceText ( "H" );
  //cube->SetZMinusFaceText( "F" );

  //vtkAxesActor* axes2 = vtkAxesActor::New();

  //vtkTransform *transform = vtkTransform::New();
  //transform->Identity();
  //reader->GetDirectionCosines()->Print(std::cout);
  //transform->Concatenate(invert);
  //axes2->SetShaftTypeToCylinder();

  //axes2->SetShaftTypeToCylinder();
  //axes2->SetUserTransform( transform );
  //cube->GetAssembly()->SetUserTransform( transform ); // cant get it to work

  //vtkPropAssembly* assembly = vtkPropAssembly::New();
  //assembly->AddPart( axes2 );
  //assembly->AddPart( cube );

  //vtkOrientationMarkerWidget* widget = vtkOrientationMarkerWidget::New();
  //widget->SetOrientationMarker( assembly );
  //widget->SetInteractor( iren );
  //widget->SetEnabled( 1 );
  //widget->InteractiveOff();
  //widget->InteractiveOn();

}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::disableInteraction()
{
  this->vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(NULL);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::enableInteraction()
{
  this->vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->interStyle);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::refresh()
{
  this->vtkWidget->update();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::addActor(vtkActor *actor)
{
  this->renderer->AddActor(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::removeActor(vtkActor *actor)
{
  this->renderer->RemoveActor(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::addActor(vtkActor2D *actor)
{
  this->renderer->AddActor2D(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::removeActor(vtkActor2D *actor)
{
  this->renderer->RemoveActor2D(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::reset()
{
  this->renderer->ResetCamera();
  this->dolly(1.6);
  //this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::dolly(double factor)
{
  vtkCamera *camera = this->renderer->GetActiveCamera();

  if (camera->GetParallelProjection())
  {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
  }
  else
  {
    camera->Dolly(factor);
    this->renderer->ResetCameraClippingRange();
  }

  this->renderer->UpdateLightsGeometryToFollowCamera();
  this->refresh();
}
