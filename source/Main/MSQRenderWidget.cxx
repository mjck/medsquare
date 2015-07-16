/*=========================================================================

 Program:   MedSquare
 Module:    MSQRenderWidget.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQRenderWidget.h"

#include "QVTKWidget2.h"

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

#include "vtkmsqRectangleActor2D.h"
#include "vtkmsqInteractorStyleImage.h"

/***********************************************************************************//**
 *
 */
MSQRenderWidget::MSQRenderWidget()
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

  this->currentAxialSlice = 1;
  this->currentSagittalSlice = 1;
  this->currentCoronalSlice = 1;

  this->setStyleSheet("QWidget { background-color: black; }");
  this->setLayout(layout);

  this->buildFrame();
}

/***********************************************************************************//**
 *
 */
MSQRenderWidget::~MSQRenderWidget()
{
  this->frame->Delete();
  this->interStyle->Delete();
  this->renderer->Delete();

  delete vtkWidget;
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::buildFrame()
{
  this->frame = vtkmsqRectangleActor2D::New();
  this->frame->GetProperty()->SetColor(0.95, 0.95, 0.25);
  this->frame->GetProperty()->SetOpacity(0.9);
  this->frame->GetProperty()->SetLineWidth(4);
  this->frame->SetVisibility(0);
  this->renderer->AddActor2D(this->frame);
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::setFrameEnabled(bool enabled)
{
  this->frame->SetVisibility(enabled ? 1 : 0);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
bool MSQRenderWidget::isFrameEnabled()
{
  return this->frame->GetVisibility() ? true : false;
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::setAxialSlice(int slice)
{
  this->currentAxialSlice = slice;
  this->setSlice(this->currentAxialSlice, this->currentSagittalSlice, this->currentCoronalSlice);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::setSagittalSlice(int slice)
{
  this->currentSagittalSlice = slice;
  this->setSlice(this->currentAxialSlice, this->currentSagittalSlice, this->currentCoronalSlice);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::setCoronalSlice(int slice)
{
  this->currentCoronalSlice = slice;
  this->setSlice(this->currentAxialSlice, this->currentSagittalSlice, this->currentCoronalSlice);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::disableInteraction()
{
  this->vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(NULL);
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::enableInteraction()
{
  this->vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->interStyle);
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::refresh()
{
  this->vtkWidget->update();
}

/***********************************************************************************//**
 *
 */
bool MSQRenderWidget::has(vtkRenderer *renderer)
{
  return this->renderer == renderer;
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::toggleMaximized(vtkRenderer *pokedRenderer)
{
  if (this->renderer == pokedRenderer)
  {
    this->setVisible(true);
  }
  else
  {
    this->setVisible(!this->isVisible());
  }
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::addActor(vtkActor *actor)
{
  this->renderer->AddActor(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::removeActor(vtkActor *actor)
{
  this->renderer->RemoveActor(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::addActor(vtkActor2D *actor)
{
  this->renderer->AddActor2D(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::removeActor(vtkActor2D *actor)
{
  this->renderer->RemoveActor2D(actor);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::addVolume(vtkVolume *volume)
{
  this->renderer->AddVolume(volume);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::removeVolume(vtkVolume *volume)
{
  this->renderer->RemoveVolume(volume);
  this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::reset()
{
  this->renderer->ResetCamera();
  this->dolly(1.6);
  //this->refresh();
}

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::dolly(double factor)
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

/***********************************************************************************//**
 *
 */
void MSQRenderWidget::takeSnapshot()
{
  QHash<QString, QPair<QString, vtkSmartPointer<vtkImageWriter> > > extensionByMessage;
  extensionByMessage.insert(tr("PNG (*.png)"), QPair<QString, vtkSmartPointer<vtkImageWriter> >(".png", vtkSmartPointer<vtkPNGWriter>::New()));
  extensionByMessage.insert(tr("JPEG (*.jpg *.jpeg)"), QPair<QString, vtkSmartPointer<vtkImageWriter> >(".jpg", vtkSmartPointer<vtkJPEGWriter>::New()));
  extensionByMessage.insert(tr("TIFF (*.tiff)"), QPair<QString, vtkSmartPointer<vtkImageWriter> >(".tiff", vtkSmartPointer<vtkTIFFWriter>::New()));
  extensionByMessage.insert(tr("BMP (*.bmp)"), QPair<QString, vtkSmartPointer<vtkImageWriter> >(".bmp", vtkSmartPointer<vtkBMPWriter>::New()));

  QString extensionsFilter;
  QHashIterator<QString, QPair<QString, vtkSmartPointer<vtkImageWriter> > > iterator(extensionByMessage);
  while (iterator.hasNext())
  {
    iterator.next();
    extensionsFilter += iterator.key();
    if (iterator.hasNext())
      extensionsFilter += ";;";
  }

  QString extensionChosen;
  QString fileName = QFileDialog::getSaveFileName(this, tr("File save"), QDir::homePath(), extensionsFilter, &extensionChosen);

  // In case a file was chosen try saving it
  if (!fileName.isEmpty())
  {
    QFileInfo file(fileName);
    if (file.suffix().isEmpty())
      fileName += extensionByMessage[extensionChosen].first;

		this->disableOrientationMarkerWidget();

    vtkSmartPointer<vtkRenderLargeImage> render = vtkSmartPointer<vtkRenderLargeImage>::New();
    render->SetInput(this->renderer);
    render->SetMagnification(4);
    render->Update();

		this->enableOrientationMarkerWidget();

    vtkSmartPointer<vtkImageWriter> writer = extensionByMessage[extensionChosen].second;
    writer->SetInputConnection(render->GetOutputPort());
    writer->SetFileName(fileName.toLocal8Bit().constData());
    writer->Write();

    this->refresh();

	}
}
