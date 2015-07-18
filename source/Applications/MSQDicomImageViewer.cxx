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
#include "MSQDicomImageViewerButton.h"

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

/*
   if      (x  <= c - 0.5 - (w-1)/2), then y = ymin
>           else if (x > c - 0.5 + (w-1)/2), then y = ymax,
>           else    y = ((x - (c - 0.5)) / (w-1) + 0.5) * (ymax - ymin)+ ymin
*/


/***********************************************************************************//**
 *
 */
MSQDicomImageViewer::MSQDicomImageViewer()
{
  // create color table
  this->colorTable.clear();
  for(int c=0;c<256;c++)
    {
      //this->colorTable[c][0] = c / 255;
      //this->colorTable[c][1] = c / 255;
      //this->colorTable[c][2] = c / 255;
      this->colorTable.append(qRgb(c, c, c));
    }

  //this->window = 255;
  //this->center = 128;
  //this->colorTransferFunction = vtkColorTransferFunction::New();
  //this->colorTransferFunction->BuildFunctionFromTable( 
  //  center-window/2, center+window/2, 255, (double*) &this->colorTable);

  // create interface
  createInterface();

  roiType = 0;
  highQuality = true;
}

/***********************************************************************************//**
 *
 */
MSQDicomImageViewer::~MSQDicomImageViewer()
{
  //this->interStyle->Delete();
  //this->renderer->Delete();

  //delete vtkWidget;

  //colorTransferFunction->Delete();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::createInterface()
{
  QFont font;
  font.setPointSize(11);

  mButtonBar = new QWidget(this);
  mButtonBar->setMaximumHeight(50);
  //mButtonBar->setStyleSheet("background-color:red;");
  mButtonBar->setEnabled(false);
  
  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->setSpacing(10);
  buttonLayout->setContentsMargins(10, 0, 10, 0);
  //buttonLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  QFont sansFont("Helvetica [Cronyx]", 10);

  QButtonGroup *buttonGroup1 = new QButtonGroup(this);
  buttonGroup1->setExclusive(true);

  mWhole = new MSQDicomImageViewerButton(this);
  mWhole->setColor(QColor(0, 255, 0));
  mWhole->setMaximumSize(36,36);
  mWhole->setMinimumSize(36,36);
  mWhole->setType(3);
  mWhole->setCheckable(true);
  mWhole->setChecked(true);
  QObject::connect(mWhole, SIGNAL(clicked()), this, SLOT(wholeButtonClick()));
 
  buttonLayout->addWidget(mWhole);

  mRect = new MSQDicomImageViewerButton(this);
  mRect->setColor(QColor(0, 255, 0));
  mRect->setMaximumSize(36,36);
  mRect->setMinimumSize(36,36);
  mRect->setCheckable(true);
  buttonLayout->addWidget(mRect);
  QObject::connect(mRect, SIGNAL(clicked()), this, SLOT(rectButtonClick()));

  mRectFilled = new MSQDicomImageViewerButton(this);
  mRectFilled->setColor(QColor(0, 255, 0));
  mRectFilled->setMaximumSize(36,36);
  mRectFilled->setMinimumSize(36,36);
  mRectFilled->setFilled(true);
  mRectFilled->setCheckable(true);
  buttonLayout->addWidget(mRectFilled);
  QObject::connect(mRectFilled, SIGNAL(clicked()), this, SLOT(rectFilledButtonClick()));

  mEllipse = new MSQDicomImageViewerButton(this);
  mEllipse->setColor(QColor(0, 255, 0));
  mEllipse->setMaximumSize(36,36);
  mEllipse->setMinimumSize(36,36);
  mEllipse->setType(1);
  mEllipse->setCheckable(true);
  buttonLayout->addWidget(mEllipse);
  QObject::connect(mEllipse, SIGNAL(clicked()), this, SLOT(ellipseButtonClick()));

  mEllipseFilled = new MSQDicomImageViewerButton(this);
  mEllipseFilled->setColor(QColor(0, 255, 0));
  mEllipseFilled->setMaximumSize(36,36);
  mEllipseFilled->setMinimumSize(36,36);
  mEllipseFilled->setType(1);
  mEllipseFilled->setFilled(true);
  mEllipseFilled->setCheckable(true);
  buttonLayout->addWidget(mEllipseFilled);
  QObject::connect(mEllipseFilled, SIGNAL(clicked()), this, SLOT(ellipseFilledButtonClick()));

  mArc = new MSQDicomImageViewerButton(this);
  mArc->setColor(QColor(0, 255, 0));
  mArc->setMaximumSize(36,36);
  mArc->setMinimumSize(36,36);
  mArc->setType(2);
  mArc->setCheckable(true);
  buttonLayout->addWidget(mArc);
  QObject::connect(mArc, SIGNAL(clicked()), this, SLOT(arcButtonClick()));

  buttonGroup1->addButton(mWhole);
  buttonGroup1->addButton(mRect);
  buttonGroup1->addButton(mRectFilled);
  buttonGroup1->addButton(mEllipse);
  buttonGroup1->addButton(mEllipseFilled);
  buttonGroup1->addButton(mArc);

  buttonLayout->addStretch();

  QButtonGroup *buttonGroup2 = new QButtonGroup(this);
  buttonGroup2->setExclusive(true);

  mOnepx = new QToolButton(this);
  mOnepx->setCheckable(true);
  mOnepx->setChecked(true);
  mOnepx->setStyleSheet(
               "QToolButton { color: gray; }\n"
                "QToolButton:hover {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:pressed {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:checked {\n"
                "   color: lime;\n}" );
  mOnepx->setMaximumSize(25,36);
  mOnepx->setMinimumSize(25,36);
  mOnepx->setText("1 px");
  mOnepx->setFont(sansFont);
  QObject::connect(mOnepx, SIGNAL(clicked()), this, SLOT(penSizeChanged()));
  buttonLayout->addWidget(mOnepx);

  mThreepx = new QToolButton(this);
  mThreepx->setCheckable(true);
  mThreepx->setStyleSheet(
                "QToolButton { color: gray; }\n"
                "QToolButton:hover {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:pressed {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:checked {\n"
                "   color: lime;\n}" );
  mThreepx->setMaximumSize(25,36);
  mThreepx->setMinimumSize(25,36);
  mThreepx->setText("3 px");
  mThreepx->setFont(sansFont);
  QObject::connect(mThreepx, SIGNAL(clicked()), this, SLOT(penSizeChanged()));
  buttonLayout->addWidget(mThreepx);

  mFivepx = new QToolButton(this);
  mFivepx->setCheckable(true);
  mFivepx->setStyleSheet(
                "QToolButton { color: gray; }\n"
                "QToolButton:hover {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:pressed {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:checked {\n"
                "   color: lime;\n}" );
  mFivepx->setMaximumSize(25,36);
  mFivepx->setMinimumSize(25,36);
  mFivepx->setText("5 px");
  mFivepx->setFont(sansFont);
  QObject::connect(mFivepx, SIGNAL(clicked()), this, SLOT(penSizeChanged()));
  buttonLayout->addWidget(mFivepx);

  buttonGroup2->addButton(mOnepx);
  buttonGroup2->addButton(mThreepx);
  buttonGroup2->addButton(mFivepx);

  mQuality = new QToolButton(this);
  mQuality->setStyleSheet(
                "QToolButton { color: gray; }\n"
                "QToolButton:hover {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:pressed {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:checked {\n"
                "   color: lime;\n}" );
  mQuality->setMaximumHeight(36);
  mQuality->setMinimumHeight(36);
  mQuality->setText("Quality: High");
  mQuality->setFont(sansFont);
  QObject::connect(mQuality, SIGNAL(clicked()), this, SLOT(qualityChanged()));
  buttonLayout->addStretch();
  buttonLayout->addWidget(mQuality);

  mButtonBar->setLayout(buttonLayout);
  mButtonBar->hide();

  //mLabel = new QLabel(this);
  mLabel = new MSQAspectRatioPixmapLabel(this);
  //mLabel->setScaledContents(true);
  mLabel->setAlignment(Qt::AlignCenter);
  QObject::connect(mLabel, SIGNAL(changed()), this, SLOT(regionOfInterestChanged()));

  mHeader[0] = new QLabel(this);
  mHeader[0]->setText("");
  mHeader[0]->setFont(font);
  mHeader[0]->setMargin(3);
  mHeader[0]->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mHeader[0]->setStyleSheet("QLabel { background-color : black; color : rgb(235, 235, 235); }");

  mHeader[1] = new QLabel(this);
  mHeader[1]->setText("");
  mHeader[1]->setFont(font);
  mHeader[1]->setMargin(3);
  mHeader[1]->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  mHeader[1]->setStyleSheet("QLabel { background-color : black; color : rgb(235, 235, 235); }");

  mHeader[2] = new QLabel(this);
  mHeader[2]->setText("");
  mHeader[2]->setFont(font);
  mHeader[2]->setMargin(3);
  mHeader[2]->setAlignment(Qt::AlignRight | Qt::AlignTop);
  mHeader[2]->setStyleSheet("QLabel { background-color : black; color : rgb(235, 235, 235); }");

  mFooter[0] = new QLabel(this);
  mFooter[0]->setText("");
  mFooter[0]->setFont(font);
  mFooter[0]->setMargin(3);
  mFooter[0]->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
  mFooter[0]->setStyleSheet("QLabel { background-color : black; color : rgb(235, 235, 235); }");
  
  mFooter[1] = new QLabel(this);
  mFooter[1]->setText("");
  mFooter[1]->setFont(font);
  mFooter[1]->setMargin(3);
  mFooter[1]->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  mFooter[1]->setStyleSheet("QLabel { background-color : black; color : rgb(235, 235, 235); }");

  mFooter[2] = new QLabel(this);
  mFooter[2]->setText("");
  mFooter[2]->setFont(font);
  mFooter[2]->setMargin(3);
  mFooter[2]->setAlignment(Qt::AlignRight | Qt::AlignBottom);
  mFooter[2]->setStyleSheet("QLabel { background-color : black; color : rgb(235, 235, 235); }");

  //QLabel *layer = new QLabel("Layer: ");
  //layer->setFont(font);
  //layer->setMargin(3);
  //layer->setMaximumHeight(40);
  //layer->setMinimumHeight(40);
  //layer->setStyleSheet("QLabel { background-color : grey; color : rgb(235, 235, 235); }");
  //layer->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

  //QComboBox *layerNumber = new QComboBox;
  //layerNumber->setStyleSheet("QLabel { background-color : grey; color : rgb(235, 235, 235); }");
  //layerNumber->setFont(font);
  //layerNumber->addItem("1");
  //layerNumber->addItem("2");
  //layerNumber->addItem("3");

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  QHBoxLayout *top = new QHBoxLayout;
  top->setSpacing(0);
  top->setContentsMargins(0, 0, 0, 0);
  top->addWidget(mHeader[0]);
  top->addWidget(mHeader[1]);
  top->addWidget(mHeader[2]);

  QHBoxLayout *bot = new QHBoxLayout;
  bot->setSpacing(0);
  bot->setContentsMargins(0, 0, 0, 0);  
  bot->addWidget(mFooter[0]);
  bot->addWidget(mFooter[1]);
  bot->addWidget(mFooter[2]);

  //QHBoxLayout *tools = new QHBoxLayout;
  //tools->setSpacing(0);
  //tools->setContentsMargins(0, 0, 0, 0);  
  //tools->addWidget(layer);

  //layout->addWidget(vtkWidget);
  layout->addWidget(mButtonBar);
  layout->addLayout(top);
  layout->addWidget(mLabel);
  layout->addLayout(bot);
  //layout->addLayout(tools);

  this->setStyleSheet("QWidget { background-color: black; color: white;}");
  this->setLayout(layout);
}

/***********************************************************************************//**
 *
 */
MSQAspectRatioPixmapLabel *MSQDicomImageViewer::label()
{
  return mLabel;
}


/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::qualityChanged()
{
    MSQDicomImageViewerButton *wTools[6] = {
      this->mWhole, this->mRect, this->mRectFilled,
      this->mEllipse, this->mEllipseFilled, this->mArc 
    };

    QAbstractButton *wOptions[4] = {
      this->mOnepx, this->mThreepx, this->mFivepx, this->mQuality
    };

    this->highQuality = !this->highQuality;

    if(this->highQuality) {

      this->mQuality->setText("Quality: High");

      for(int i=0; i<6; i++)
        wTools[i]->setColor(QColor(0, 255, 0));

      for(int i=0; i<4; i++)
        wOptions[i]->setStyleSheet(
                "QToolButton { color: gray; }\n"
                "QToolButton:hover {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:pressed {\n"
                "   color: lime;\n"
                "}\n"
                "QToolButton:checked {\n"
                "   color: lime;\n}" );
    } else {

      this->mQuality->setText("Quality: Low");
      
      for(int i=0; i<6; i++)
        wTools[i]->setColor(QColor(255, 0, 0));

      for(int i=0; i<4; i++)
        wOptions[i]->setStyleSheet(
          "QToolButton { color: gray; }\n"
          "QToolButton:hover {\n"
          "   color: red;\n"
          "}\n"
          "QToolButton:pressed {\n"
          "   color: red;\n"
          "}\n"
          "QToolButton:checked {\n"
          "   color: red;\n}" );

    }
    mLabel->setHighQuality(this->highQuality);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::penSizeChanged()
{
    if (this->mOnepx->isChecked())
      mLabel->setPenSize(1);
    else if (this->mThreepx->isChecked())
      mLabel->setPenSize(3);
    else if (this->mFivepx->isChecked())
      mLabel->setPenSize(5);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::statistics(gdcm::Image const & gimage, char *buffer, const QImage& mask, 
  double window, double center, double slope, double intercept, double *entropy, double *mean, double *stdev)
{

  const unsigned int* dimension = gimage.GetDimensions();

  int c, r, pos, total=0;
  long hist[256];
  unsigned int dimX = dimension[0];
  unsigned int dimY = dimension[1];
  double dimXdimY = dimX * dimY;
  double px, sumlog = 0.0;
  double sum = 0.0;
  double sum2 = 0.0;

  *entropy = -1;
  *mean = 0;
  *stdev = 0;

  QRgb *scan = (QRgb *)mask.scanLine(0);

  for(c=0;c<256;c++)
    {
        hist[c] = 0;
    }

  // Let's start with the easy case:
  if( gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::RGB )
    {
      // RGB image
      *entropy = -1;
      *mean = 0;
      *stdev = 0;
      return;
    }
  else if( gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::MONOCHROME1 ||
           gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::MONOCHROME2 )
    {
      if( gimage.GetPixelFormat() == gdcm::PixelFormat::INT8 || gimage.GetPixelFormat() == gdcm::PixelFormat::UINT8 )
      {
        for(unsigned int i = 0; i < dimX*dimY; i++)
          {
            r = qRed(scan[i]);
            if (r > 0) {
              pos = buffer[i];
              hist[pos]++;
              sum += pos;
              sum2 += pos * pos;
              total++;
            }
          }
      }
    else if ( gimage.GetPixelFormat() == gdcm::PixelFormat::INT16 || gimage.GetPixelFormat() == gdcm::PixelFormat::UINT16 )
      {
        short *input = (short*)buffer;
        double scaled;

        for(unsigned int i = 0; i < dimX*dimY; i++)
          {
            r = qRed(scan[i]);
            if (r > 0) {

              if (input[i] <= center - 0.5 - (window - 1.0 ) / 2 )
                pos = 0;
              else if (input[i] > center - 0.5 + (window - 1.0) / 2)
                pos = 255;
              else 
                pos = ((input[i] - (center - 0.5)) / (window - 1.0) + 0.5) * 255;

            //pos = (int)round((double)input[i] / 256);

              hist[pos]++;
              sum += input[i];
              sum2 += input[i] * input[i];
              total++;
            }
          }
      }
    else
      {
        *entropy = -1;
        *mean = 0;
        *stdev = 0;
        return;
      }
    }
  else
    {
      *entropy = -1;
      *mean = 0;
      *stdev = 0;
      return;
    }

  if (total > 0) {
    // calculate entropy
    for(c=0;c<256;c++)
    {
      if (hist[c] > 0) {
         px = hist[c] / (double)total;
         sumlog -= px * log(px);
         //total += c;
      }
    }

    *entropy = sumlog / M_LN2;
    *mean = sum / total;
    *stdev = sqrt((sum2 / total) - (*mean * *mean));

    //printf("total=%d\n",total);
  } else {
    *entropy = -1;
    *mean = 0;
    *stdev = 0;
  }
  // print entropy
  //printf("%f, %f\n",sumlog / M_LN2, sum / dimXdimY);
  //return sumlog / M_LN2;
}

/***********************************************************************************//**
 *
 */
bool MSQDicomImageViewer::ConvertToFormat_RGB888(gdcm::Image const & gimage, char *buffer, QImage* &imageQt, 
  double window, double center, double slope, double intercept)
{
  const unsigned int* dimension = gimage.GetDimensions();

  unsigned int dimX = dimension[0];
  unsigned int dimY = dimension[1];

  //double table[256][3];

  //for( int i = 0; i < 256; i++)
  //{
  //  table[i][0] = qRed(colorTable.at(i)) / 255.;
  //  table[i][1] = qGreen(colorTable.at(i))  / 255.;
  //  table[i][2] = qBlue(colorTable.at(i)) / 255.;
  //}
   
   /*vtkWindowLevelLookupTable *lut = vtkWindowLevelLookupTable::New();
   lut->SetWindow( window );
   lut->SetLevel ( center );
   lut->Build();

   for(int i=0; i<256; i++) {
    lut->SetTableValue(i, qRed(colorTable.at(i)) / 255., qGreen(colorTable.at(i)) / 255., qBlue(colorTable.at(i)) / 255.);
   }*/

   //colorTransferFunction->BuildFunctionFromTable( center-window/2, center+window/2, 255, (double*) &this->colorTable);

  //QVector<QRgb> table(2);
  //for(int c=0;c<256;c++)
  //  {
  //      table.append(qRgb(c,c,c));
  //  }

  // Let's start with the easy case:
  if( gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::RGB )
    {
      // RGB image
      if( gimage.GetPixelFormat() != gdcm::PixelFormat::UINT8 )
        {
          return false;
        }
      unsigned char *ubuffer = (unsigned char*)buffer;
      // QImage::Format_RGB888  13  The image is stored using a 24-bit RGB format (8-8-8).
      imageQt = new QImage((unsigned char *)ubuffer, dimX, dimY, 3*dimX, QImage::Format_RGB888);
    }
  else if( gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::MONOCHROME1 ||
           gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::MONOCHROME2 )
    {
      if( gimage.GetPixelFormat() == gdcm::PixelFormat::INT8 || gimage.GetPixelFormat() == gdcm::PixelFormat::UINT8 )
      {
        //std::cerr << "Pixel Format is: " << gimage.GetPixelFormat() << std::endl;
        // We need to copy each individual 8bits into R / G and B:
        unsigned char *ubuffer = new unsigned char[dimX*dimY*3];
        unsigned char *pubuffer = ubuffer;
        
        for(unsigned int i = 0; i < dimX*dimY; i++)
          {
            *pubuffer++ = *buffer;
            *pubuffer++ = *buffer;
            *pubuffer++ = *buffer++;
          }

        imageQt = new QImage(ubuffer, dimX, dimY, 3*dimX, QImage::Format_RGB888);
      }
    else if ( gimage.GetPixelFormat() == gdcm::PixelFormat::INT16 || gimage.GetPixelFormat() == gdcm::PixelFormat::UINT16 )
      {
        short *input = (short*)buffer;
        double scaled;
        unsigned char *output = new unsigned char[dimX*dimY];//*3];
        unsigned char *poutput = output;
        unsigned char r;

        for(unsigned int i = 0; i < dimX*dimY; i++)
          {
            scaled = *input * slope + intercept;
            if (scaled <= center - 0.5 - (window - 1.0 ) / 2 )
              r = 0;
            else if (scaled > center - 0.5 + (window - 1.0) / 2)
              r = 255;
            else {
              r = ((scaled - (center - 0.5)) / (window - 1.0) + 0.5) * 255;
            }
            *poutput++ = r;

            /*unsigned char *rgb = colorTransferFunction->MapValue(scaled);
            *poutput++ = rgb[0];
            *poutput++ = rgb[1];
            *poutput++ = rgb[2];*/

            input++;
          }

        //imageQt = new QImage(output, dimX, dimY, 3*dimX, QImage::Format_RGB888);
        imageQt = new QImage(output, dimX, dimY, dimX, QImage::Format_Indexed8);
        imageQt->setColorTable(this->colorTable);

        //colorTransferFunction->Delete();
        //lut->Delete();
      }
    else
      {
        std::cerr << "Pixel Format is: " << gimage.GetPixelFormat() << std::endl;
        return false;
      }
    }
  else
    {
    std::cerr << "Unhandled PhotometricInterpretation: " << gimage.GetPhotometricInterpretation() << std::endl;
    return false;
    }

  return true;
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::updateViewer()
{
  gdcm::ImageReader reader;
  reader.SetFileName( this->fileName.toLocal8Bit().constData() );
  if(!reader.Read())
    {
      //std::cout<<"Could not open file" << std::endl;
      return;
      //Read failed
      //return 1;
    }

  const gdcm::File &file = reader.GetFile();
  const gdcm::Image &gimage = reader.GetImage();
  const double *spacing = gimage.GetSpacing();
  const gdcm::DataSet &ds = file.GetDataSet();

  double window = 256, center = 128;
  gdcm::Tag twindowcenter(0x0028, 0x1050);
  gdcm::Tag twindowwidth(0x0028, 0x1051);

  // rescale slope and intercept
  gdcm::Tag tintercept(0x0028, 0x1052);
  gdcm::Tag tslope(0x0028, 0x1053);

  // study and series description
  gdcm::Tag tstudesc(0x0008, 0x1030);
  gdcm::Tag tseqdesc(0x0018, 0x0024);
  gdcm::Tag tseriesdesc(0x0008, 0x103e);
  gdcm::Tag tbodypart(0x0018, 0x0015);

  // number of columns and rows
  gdcm::Tag tcolumns(0x0028, 0x0011);
  gdcm::Tag trows(0x0028, 0x0010);

  // date and time
  gdcm::Tag tacqtime(0x0008, 0x0032);
  gdcm::Tag tacqdate(0x0008, 0x0022);
  gdcm::Tag tacqdatetime(0x0008, 0x002a); // philips

  // slicethickness or spacing between slices
  gdcm::Tag tthickness(0x0018, 0x0050);
  gdcm::Tag tspacing(0x0018, 0x0088); // philips

  // in-plane phase encoding direction
  gdcm::Tag tphase(0x0018, 0x1312);

  QString topleft="", topmid="", topright="";
  QString botleft="", botmid="", botright="";

  unsigned long len = gimage.GetBufferLength();
  std::vector<char> vbuffer;
  vbuffer.resize( len );
  char *buffer = &vbuffer[0];
  gimage.GetBuffer(buffer);

  double intercept = 0.0;
  double slope = 1.0;

  QImage *imageQt = NULL;

  // find intercept and slope
  if ( ds.FindDataElement( tintercept ) && ds.FindDataElement( tslope ) ) 
  {
    const gdcm::DataElement& rescaleintercept = ds.GetDataElement( tintercept );
    const gdcm::DataElement& rescaleslope = ds.GetDataElement( tslope );
    const gdcm::ByteValue *bvri = rescaleintercept.GetByteValue();
    const gdcm::ByteValue *bvrs = rescaleslope.GetByteValue();
    std::string sri = std::string( bvri->GetPointer(), bvri->GetLength() );
    std::string srs = std::string( bvrs->GetPointer(), bvrs->GetLength() );
    //intercept = std::stod(sri); // C++ 11
    intercept = ::atof(sri.c_str());
    //slope = std::stod(srs); // C++ 11
    slope = ::atof(srs.c_str());
  }

  // window and level
  if( ds.FindDataElement( twindowcenter ) && ds.FindDataElement( twindowwidth) )
  {
    const gdcm::DataElement& windowcenter = ds.GetDataElement( twindowcenter );
    const gdcm::DataElement& windowwidth = ds.GetDataElement( twindowwidth );
    const gdcm::ByteValue *bvwc = windowcenter.GetByteValue();
    const gdcm::ByteValue *bvww = windowwidth.GetByteValue();

    if( bvwc && bvww ) // Can be Type 2
    {
        //gdcm::Attributes<0x0028,0x1050> at;
        gdcm::Element<gdcm::VR::DS,gdcm::VM::VM1_n> elwc;
        std::stringstream ss1;
        std::string swc = std::string( bvwc->GetPointer(), bvwc->GetLength() );
        ss1.str( swc );
        gdcm::VR vr = gdcm::VR::DS;
        unsigned int vrsize = vr.GetSizeof();
        unsigned int count = gdcm::VM::GetNumberOfElementsFromArray(swc.c_str(), (unsigned int)swc.size());
        elwc.SetLength( count * vrsize );
        elwc.Read( ss1 );
        std::stringstream ss2;
        std::string sww = std::string( bvww->GetPointer(), bvww->GetLength() );
        ss2.str( sww );
        gdcm::Element<gdcm::VR::DS,gdcm::VM::VM1_n> elww;
        elww.SetLength( count * vrsize );
        elww.Read( ss2 );

        if (elwc.GetLength() > 0)
        {
          window = elww.GetValue(0);
          center = elwc.GetValue(0);

          //printf("*window=%f, center=%f, length=%lu\n",window,center,elwc.GetLength());

        }
    }
  }

//printf("**window=%f, center=%f\n",window,center);
 if( !ConvertToFormat_RGB888( gimage, buffer, imageQt, window, center, slope, intercept ) )
    {
      std::cout<<"Could not convert into QImage..."<<std::endl;
      return;
    }

  mLabel->setPixmap( QPixmap::fromImage(*imageQt) );

  // Study and series description 
  if ( ds.FindDataElement (tstudesc) ) {
    const gdcm::DataElement& studesc = ds.GetDataElement( tstudesc );
    const gdcm::ByteValue *bvstu = studesc.GetByteValue();
    if (bvstu)
      topleft += QString::fromStdString( std::string( bvstu->GetPointer(), bvstu->GetLength() ) ) + "<br>";
    else
      topleft += "<br>";
  }

  if ( ds.FindDataElement (tseriesdesc) ) {
    const gdcm::DataElement& seriesdesc = ds.GetDataElement( tseriesdesc );
    const gdcm::ByteValue *bvsed = seriesdesc.GetByteValue();
    if (bvsed)
      topleft += QString::fromStdString( std::string( bvsed->GetPointer(), bvsed->GetLength() ) );
  }
  mHeader[0]->setText(topleft);

 // body part
  if ( ds.FindDataElement (tbodypart) ) {
    const gdcm::DataElement& bodypart = ds.GetDataElement( tbodypart );
    const gdcm::ByteValue *bvbp = bodypart.GetByteValue();
    if (bvbp)
      topmid = QString::fromStdString( std::string( bvbp->GetPointer(), bvbp->GetLength() ) );
  }
  mHeader[1]->setText(topmid);

  // Acquisition date and time
  if ( !ds.FindDataElement (tacqdate) )
  {
    if ( !ds.FindDataElement (tacqdatetime) )
      topright = "Unknown date";
    else {
      const gdcm::DataElement& acqdate = ds.GetDataElement( tacqdatetime );
      const gdcm::ByteValue *bvdate = acqdate.GetByteValue();
      if (bvdate) {
        QString qstrdate = QString::fromStdString( std::string( bvdate->GetPointer(), bvdate->GetLength() ) );
        topright += QString("%1-%2-%3").arg(qstrdate.left(4)).arg(qstrdate.mid(4, 2)).arg(qstrdate.mid(6, 2));
      } else {
        topright += "Unknown date";
      }
    }
  } else {
    const gdcm::DataElement& acqdate = ds.GetDataElement( tacqdate );
    const gdcm::ByteValue *bvdate = acqdate.GetByteValue();
    if (bvdate) {
      QString qstrdate = QString::fromStdString( std::string( bvdate->GetPointer(), bvdate->GetLength() ) );
      topright += QString("%1-%2-%3").arg(qstrdate.left(4)).arg(qstrdate.mid(4, 2)).arg(qstrdate.mid(6, 2));
    } else {
      topright += "Unknown date";
    }
  }

  if ( !ds.FindDataElement (tacqtime) ) 
  {
    if ( !ds.FindDataElement (tacqdatetime) )
        topright += "<br>Unknown time";
    else {
      const gdcm::DataElement& acqtime = ds.GetDataElement( tacqdatetime );
      const gdcm::ByteValue *bvtime = acqtime.GetByteValue();
      if (bvtime) {
        QString qstrtime = QString::fromStdString( std::string( bvtime->GetPointer(), bvtime->GetLength() ) );
        topright += QString("<br>%1:%2:%3").arg(qstrtime.mid(8, 2)).arg(qstrtime.mid(10, 2)).arg(qstrtime.mid(12, 2));
      } else{
        topright += "<br>Unknown time";
      }
    }
  } else {
    const gdcm::DataElement& acqtime = ds.GetDataElement( tacqtime );
    const gdcm::ByteValue *bvtime = acqtime.GetByteValue();
    if (bvtime) {
      QString qstrtime = QString::fromStdString( std::string( bvtime->GetPointer(), bvtime->GetLength() ) );
      topright += QString("<br>%1:%2:%3").arg(qstrtime.left(2)).arg(qstrtime.mid(2, 2)).arg(qstrtime.mid(4, 2));
    } else {
      topright += "<br>Unknown time";
    }
  }
  mHeader[2]->setText(topright);

  // Entropy
  const QImage mask = mLabel->regionOfInterest();
  double entrpy, mean, stdev;
  this->statistics(gimage, buffer, mask, window, center, slope, intercept, &entrpy, &mean, &stdev);
  botleft = QString("<FONT COLOR=Firebrick>Entropy: %1</FONT>").arg(entrpy);

  // Number of columns and rows, spacing
  if ( !ds.FindDataElement (tcolumns) )
    botleft += "<br>Image size: ? x ";
  else {
    const gdcm::DataElement& columns = ds.GetDataElement( tcolumns );
    const gdcm::ByteValue *bvcol = columns.GetByteValue();
    const int vcols = *(int *)bvcol->GetPointer();
    botleft += QString("<br>Image size: %1 x ").arg(vcols);
  }

  if ( !ds.FindDataElement (trows) )
    botleft += "?";
  else {
    const gdcm::DataElement& rows = ds.GetDataElement( trows );
    const gdcm::ByteValue *bvrow = rows.GetByteValue();
    const int vrows = *(int *)bvrow->GetPointer();
    botleft += QString("%1").arg(vrows);
  }

  // add pixel spacing
  botleft += QString("<br>Pixel spacing: %1 mm x %2 mm").arg(spacing[0], 0, 'g', 2).arg(spacing[1], 0, 'g', 2);

  mFooter[0]->setText(botleft);

  // phase encoding direction
  if ( ds.FindDataElement (tphase) ) {
    const gdcm::DataElement& phase = ds.GetDataElement( tphase );
    const gdcm::ByteValue *bvphase = phase.GetByteValue();
    if (bvphase) 
      botmid = QString::fromStdString( std::string( bvphase->GetPointer(), bvphase->GetLength() ) );
  }
  
  mFooter[1]->setText(botmid);

  botright = QString("<FONT COLOR=RoyalBlue>Mean: %1</FONT>").arg(mean);
  botright += QString("<br><FONT COLOR=RoyalBlue>SD: %1</FONT>").arg(stdev);

  // slice thickness
  if ( !ds.FindDataElement (tthickness) )
  {
    botright += "";
  } else {
    const gdcm::DataElement& thick = ds.GetDataElement( tthickness );
    const gdcm::ByteValue *bvthick = thick.GetByteValue();
    if (bvthick) {
      QString str = QString::fromStdString( std::string( bvthick->GetPointer(), bvthick->GetLength() ) );
      botright += QString("<br>Thickness: %1 mm").arg(str.toFloat(), 0, 'g', 2);
    } else {
      botright += "";
    }
  }

  if ( !ds.FindDataElement (tspacing) )
    botright += "";
  else {
    const gdcm::DataElement& space = ds.GetDataElement( tspacing );
    const gdcm::ByteValue *bvspace = space.GetByteValue();
    if (bvspace) {
      QString str = QString::fromStdString( std::string( bvspace->GetPointer(), bvspace->GetLength() ) );
      botright += QString("<br>Spacing between slices: %1 mm").arg(str.toFloat(), 0, 'g', 2);
    } else {
      botright += "";
    }
  }

  mFooter[2]->setText(botright);

}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setInput(const QString& fileName) 
{
  this->fileName = fileName;

  this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setColormap(vtkmsqLookupTable *lut) 
{
  double *color;
  int numColors = lut->GetNumberOfTableValues();
  
  this->colorTable.clear();

  for(int c=0;c<numColors;c++)
  {
    color = lut->GetTableValue(c);
    this->colorTable.append(qRgb(color[0]*255, color[1]*255, color[2]*255));
  }

  //colorTransferFunction->BuildFunctionFromTable( center-window/2, center+window/2, 255, (double*) &this->colorTable);

  // redisplay image
  this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setBackgroundOpacity(qreal opacity) 
{
  mLabel->setBackgroundOpacity( opacity );
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setForegroundOpacity(qreal opacity) 
{
  mLabel->setForegroundOpacity( opacity );
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::regionOfInterestChanged()
{
  gdcm::ImageReader reader;
  if (this->fileName.isEmpty())
    return;

  reader.SetFileName( this->fileName.toLocal8Bit().constData() );
  if(!reader.Read())
    {
      //std::cout<<"Could not open file" << std::endl;
      return;
      //Read failed
      //return 1;
    }

  const gdcm::File &file = reader.GetFile();
  const gdcm::Image &gimage = reader.GetImage();
  const double *spacing = gimage.GetSpacing();
  const gdcm::DataSet &ds = file.GetDataSet();

  double window = 256, center = 128;
  gdcm::Tag twindowcenter(0x0028, 0x1050);
  gdcm::Tag twindowwidth(0x0028, 0x1051);

  // rescale slope and intercept
  gdcm::Tag tintercept(0x0028, 0x1052);
  gdcm::Tag tslope(0x0028, 0x1053);

  // number of columns and rows
  gdcm::Tag tcolumns(0x0028, 0x0011);
  gdcm::Tag trows(0x0028, 0x0010);

  // date and time
  gdcm::Tag tacqtime(0x0008, 0x0032);
  gdcm::Tag tacqdate(0x0008, 0x0022);
  gdcm::Tag tacqdatetime(0x0008, 0x002a); // philips

  // slicethickness or spacing between slices
  gdcm::Tag tthickness(0x0018, 0x0050);
  gdcm::Tag tspacing(0x0018, 0x0088); // philips

  QString topleft, topmid, topright;
  QString botleft, botmid, botright;

  double slope = 1.0;
  double intercept = 0.0;

  unsigned long len = gimage.GetBufferLength();
  std::vector<char> vbuffer;
  vbuffer.resize( len );
  char *buffer = &vbuffer[0];
  gimage.GetBuffer(buffer);

  // find intercept and slope
  if ( ds.FindDataElement( tintercept ) && ds.FindDataElement( tslope ) ) 
  {
    const gdcm::DataElement& rescaleintercept = ds.GetDataElement( tintercept );
    const gdcm::DataElement& rescaleslope = ds.GetDataElement( tslope );
    const gdcm::ByteValue *bvri = rescaleintercept.GetByteValue();
    const gdcm::ByteValue *bvrs = rescaleslope.GetByteValue();
    std::string sri = std::string( bvri->GetPointer(), bvri->GetLength() );
    std::string srs = std::string( bvrs->GetPointer(), bvrs->GetLength() );
    //intercept = std::stod(sri);
    intercept = ::atof(sri.c_str());
    //slope = std::stod(srs);
    slope = ::atof(srs.c_str());
  }

  // window and level
  if( ds.FindDataElement( twindowcenter ) && ds.FindDataElement( twindowwidth) )
  {
    const gdcm::DataElement& windowcenter = ds.GetDataElement( twindowcenter );
    const gdcm::DataElement& windowwidth = ds.GetDataElement( twindowwidth );
    const gdcm::ByteValue *bvwc = windowcenter.GetByteValue();
    const gdcm::ByteValue *bvww = windowwidth.GetByteValue();

    if( bvwc && bvww ) // Can be Type 2
    {
        //gdcm::Attributes<0x0028,0x1050> at;
        gdcm::Element<gdcm::VR::DS,gdcm::VM::VM1_n> elwc;
        std::stringstream ss1;
        std::string swc = std::string( bvwc->GetPointer(), bvwc->GetLength() );
        ss1.str( swc );
        gdcm::VR vr = gdcm::VR::DS;
        unsigned int vrsize = vr.GetSizeof();
        unsigned int count = gdcm::VM::GetNumberOfElementsFromArray(swc.c_str(), (unsigned int)swc.size());
        elwc.SetLength( count * vrsize );
        elwc.Read( ss1 );
        std::stringstream ss2;
        std::string sww = std::string( bvww->GetPointer(), bvww->GetLength() );
        ss2.str( sww );
        gdcm::Element<gdcm::VR::DS,gdcm::VM::VM1_n> elww;
        elww.SetLength( count * vrsize );
        elww.Read( ss2 );

        if (elwc.GetLength() > 0)
        {
          window = elww.GetValue(0);
          center = elwc.GetValue(0);
        }
    }
  }

  // Entropy
  //printf("changed\n");
  const QImage mask = mLabel->regionOfInterest();
  double entrpy, mean, stdev;
  this->statistics(gimage, buffer, mask, window, center, slope, intercept, &entrpy, &mean, &stdev);
  botleft = QString("<FONT COLOR=Firebrick>Entropy: %1</FONT>").arg(entrpy);

  // Number of columns and rows, spacing
  if ( !ds.FindDataElement (tcolumns) )
    botleft += "<br>Image size: ? x ";
  else {
    const gdcm::DataElement& columns = ds.GetDataElement( tcolumns );
    const gdcm::ByteValue *bvcol = columns.GetByteValue();
    const int vcols = *(int *)bvcol->GetPointer();
    botleft += QString("<br>Image size: %1 x ").arg(vcols);
  }

  if ( !ds.FindDataElement (trows) )
    botleft += "?";
  else {
    const gdcm::DataElement& rows = ds.GetDataElement( trows );
    const gdcm::ByteValue *bvrow = rows.GetByteValue();
    const int vrows = *(int *)bvrow->GetPointer();
    botleft += QString("%1").arg(vrows);
  }

  // add pixel spacing
  botleft += QString("<br>Pixel spacing: %1 mm x %2 mm").arg(spacing[0], 0, 'g', 2).arg(spacing[1], 0, 'g', 2);

  mFooter[0]->setText(botleft);

  botright = QString("<FONT COLOR=RoyalBlue>Mean: %1</FONT>").arg(mean);
  botright += QString("<br><FONT COLOR=RoyalBlue>SD: %1</FONT>").arg(stdev);

  // slice thickness
  if ( !ds.FindDataElement (tthickness) )
  {
    botright += "";
  } else {
    const gdcm::DataElement& thick = ds.GetDataElement( tthickness );
    const gdcm::ByteValue *bvthick = thick.GetByteValue();
    QString str = QString::fromStdString( std::string( bvthick->GetPointer(), bvthick->GetLength() ) );
    botright += QString("<br>Thickness: %1 mm").arg(str.toFloat(), 0, 'g', 2);
  }

  if ( !ds.FindDataElement (tspacing) )
    botright += "";
  else {
    const gdcm::DataElement& space = ds.GetDataElement( tspacing );
    const gdcm::ByteValue *bvspace = space.GetByteValue();
    QString str = QString::fromStdString( std::string( bvspace->GetPointer(), bvspace->GetLength() ) );
    botright += QString("<br>Spacing between slices: %1 mm").arg(str.toFloat(), 0, 'g', 2);
  }

  mFooter[2]->setText(botright);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::wholeButtonClick()
{
  mLabel->setCursorEnabled(false);
  roiType = 0;
  //emit regionOfInterestChanged();
}


/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::rectButtonClick()
{
  mLabel->setCursorToRect();
  mLabel->setCursorEnabled(true);
  mLabel->setCursorFilled(false);
  roiType = 1;
  //emit regionOfInterestChanged();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::rectFilledButtonClick()
{
  mLabel->setCursorToRect();
  mLabel->setCursorEnabled(true);
  mLabel->setCursorFilled(true);
  roiType = 2;
  //emit regionOfInterestChanged();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::ellipseButtonClick()
{
  mLabel->setCursorToEllipse();
  mLabel->setCursorEnabled(true);
  mLabel->setCursorFilled(false);
  roiType = 3;
  //emit regionOfInterestChanged();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::ellipseFilledButtonClick()
{
  mLabel->setCursorToEllipse();
  mLabel->setCursorEnabled(true);
  mLabel->setCursorFilled(true);
  roiType = 4;
  //emit regionOfInterestChanged();
}


/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::arcButtonClick()
{
  mLabel->setCursorToArc();
  mLabel->setCursorEnabled(true);
  mLabel->setCursorFilled(false);
  roiType = 5;
  //emit regionOfInterestChanged();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::showToolBar(bool show)
{
  if (show) {
    this->mButtonBar->show();
  } else {
    this->mButtonBar->hide();
  }
  mLabel->setCursorEnabled( show && roiType != 0);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::enableToolBar(bool enable)
{
    this->mButtonBar->setEnabled(enable);
    mLabel->setCursorEnabled( enable && roiType != 0);
}

/***********************************************************************************//**
 *
 */
QImage MSQDicomImageViewer::regionOfInterest() const
{
  return mLabel->regionOfInterest();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::refresh()
{
  //this->vtkWidget->update();
}

