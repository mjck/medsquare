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

#include "gdcmStringFilter.h"

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
  // create color tables
  this->foregroundColorTable.clear();
  this->backgroundColorTable.clear();
  
  for(int c=0;c<256;c++)
    {
      this->foregroundColorTable.append(qRgb(c, c, c));
      this->backgroundColorTable.append(qRgb(c, c, c));
    }

  this->foregroundOpacity = 1.0;
  this->backgroundOpacity = 0.0;

  //this->window = 255;
  //this->center = 128;
  //this->colorTransferFunction = vtkColorTransferFunction::New();
  //this->colorTransferFunction->BuildFunctionFromTable( 
  //  center-window/2, center+window/2, 255, (double*) &this->colorTable);
  this->perc = 80;

  // create interface
  createInterface();

  this->roiType = 0;
  this->highQuality = true;
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

  mThreshold = new MSQDicomImageViewerButton(this);
  mThreshold->setColor(QColor(0, 255, 0));
  mThreshold->setMaximumSize(36,36);
  mThreshold->setMinimumSize(36,36);
  mThreshold->setType(4);
  mThreshold->setCheckable(true);
  buttonLayout->addWidget(mThreshold);
  QObject::connect(mThreshold, SIGNAL(clicked()), this, SLOT(thresholdButtonClick()));

  mPerc = new QLabel(this);
  mPerc->setText(QString("%1%").arg(perc));
  mPerc->setStyleSheet("QLabel { color: gray; }\n");
  mPerc->setFont(sansFont);
  mPerc->setMaximumSize(25,36);
  mPerc->setMinimumSize(25,36);
  buttonLayout->addWidget(mPerc);

  buttonGroup1->addButton(mWhole);
  buttonGroup1->addButton(mRect);
  buttonGroup1->addButton(mRectFilled);
  buttonGroup1->addButton(mEllipse);
  buttonGroup1->addButton(mEllipseFilled);
  buttonGroup1->addButton(mArc);
  buttonGroup1->addButton(mThreshold);

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
  mLabel->setThresholdPercentage(perc);
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
    MSQDicomImageViewerButton *wTools[7] = {
      this->mWhole, this->mRect, this->mRectFilled,
      this->mEllipse, this->mEllipseFilled, this->mArc , this->mThreshold
    };

    QAbstractButton *wOptions[4] = {
      this->mOnepx, this->mThreepx, this->mFivepx, this->mQuality
    };

    this->highQuality = !this->highQuality;

    if(this->highQuality) {

      this->mQuality->setText("Quality: High");

      for(int i=0; i<7; i++)
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
      
      for(int i=0; i<7; i++)
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
void MSQDicomImageViewer::statistics(const MSQDicomImage& source, std::vector<int>& mask, double *entropy, double *mean, double *stdev)
{
  //short pos;
  //int c, r, total=0;
  long hist[256];
  //std::map<short, int> hist;
  //unsigned int dimX = source.columns;
  //unsigned int dimY = source.rows;
  //double dimXdimY = dimX * dimY;
  double px, sumlog = 0.0;
  double sum = 0.0;
  double sum2 = 0.0;

  *entropy = -1;
  *mean = 0;
  *stdev = 0;

  //printf("inside\n");
  if (mask.size() == 0)
    return;

  //printf("going on with size = %d\n",mask.size());

  for(int j=0; j<256; j++) {
    hist[j] = 0;
  }

  float min;
  float max;
  float weight = 0;
  int index;

  //printf("size=%d\n", mask.size());

  // Let's start with the easy case
  if( source.interpretation == gdcm::PhotometricInterpretation::RGB )
    {
      // RGB image
      *entropy = -1;
      *mean = 0;
      *stdev = 0;
      return;
    } 
  else if( source.interpretation == gdcm::PhotometricInterpretation::MONOCHROME1 ||
           source.interpretation == gdcm::PhotometricInterpretation::MONOCHROME2 )
    {
      if( source.pixelformat == gdcm::PixelFormat::INT8 || source.pixelformat == gdcm::PixelFormat::UINT8 )
      {
        char *input = (char*)&source.vbuffer[0];

        min = max = input[mask[0]];
        for(int i=1; i<mask.size(); i++)
        {
          if (input[mask[i]] < min)
            min = input[mask[i]];
          if (input[mask[i]] > max)
            max = input[mask[i]];
        }

        weight = 255.0 / ((max - min) + 1);
        //printf("u8 min: %f, %f\n",min,max);
        for(int i=0; i<mask.size(); i++)
        {
          short value = input[mask[i]];
          index = round((value - min) * weight);
          hist[index]++;
          sum += value;
          sum2 += value * value;
        }
      }
      else if ( source.pixelformat == gdcm::PixelFormat::INT16 || source.pixelformat == gdcm::PixelFormat::UINT16 )
      {
        short *input = (short*)&source.vbuffer[0];
        
        min = max = input[mask[0]];
        for(int i=1; i<mask.size(); i++)
        {
          if (input[mask[i]] < min)
            min = input[mask[i]];
          if (input[mask[i]] > max)
            max = input[mask[i]];
        }

        weight = 255.0 / ((max - min) + 1);
        //printf("u16 min: %f, %f\n",min,max);
        for(int i=0; i<mask.size(); i++)
        {
          //printf("%d: ",mask[i]);
          short value = input[mask[i]];
          //printf("%d\n",value);
          //printf("index=%d\n",index);
          index = round((value - min) * weight);
          hist[index]++;
          //printf("index=%d\n",index);
          sum += value;
          sum2 += value * value;
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

  // calculate entropy
  //for ( std::pair<short , int> p : hist ) {
  //  double px = static_cast<double>( p.second ) / (double)total;
  //  sumlog -= px * log( px );

 // calculate entropy
  for(int j=0; j<256; j++)
  {
    if (hist[j] > 0) {
      px = hist[j] / (double)mask.size();
      sumlog -= px * log(px);
    }
  }

  *entropy = sumlog / M_LN2;
  *mean = sum / mask.size();
  *stdev = sqrt((sum2 / mask.size()) - (*mean * *mean));

  // print entropy
  //printf("%f, %f\n",sumlog / M_LN2, sum / dimXdimY);
  //return sumlog / M_LN2;
}

/***********************************************************************************//**
 *
 */
// bool MSQDicomImageViewer::ConvertToFormat_RGB888(gdcm::Image const & gimage, char *buffer, QImage* &imageQt, 
//   double window, double center, double slope, double intercept)
// {
//   const unsigned int* dimension = gimage.GetDimensions();

//   unsigned int dimX = dimension[0];
//   unsigned int dimY = dimension[1];

//   //double table[256][3];

//   //for( int i = 0; i < 256; i++)
//   //{
//   //  table[i][0] = qRed(colorTable.at(i)) / 255.;
//   //  table[i][1] = qGreen(colorTable.at(i))  / 255.;
//   //  table[i][2] = qBlue(colorTable.at(i)) / 255.;
//   //}
   
//    /*vtkWindowLevelLookupTable *lut = vtkWindowLevelLookupTable::New();
//    lut->SetWindow( window );
//    lut->SetLevel ( center );
//    lut->Build();

//    for(int i=0; i<256; i++) {
//     lut->SetTableValue(i, qRed(colorTable.at(i)) / 255., qGreen(colorTable.at(i)) / 255., qBlue(colorTable.at(i)) / 255.);
//    }*/

//    //colorTransferFunction->BuildFunctionFromTable( center-window/2, center+window/2, 255, (double*) &this->colorTable);

//   //QVector<QRgb> table(2);
//   //for(int c=0;c<256;c++)
//   //  {
//   //      table.append(qRgb(c,c,c));
//   //  }

//   // Let's start with the easy case:
//   if( gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::RGB )
//     {
//       // RGB image
//       if( gimage.GetPixelFormat() != gdcm::PixelFormat::UINT8 )
//         {
//           return false;
//         }
//       unsigned char *ubuffer = (unsigned char*)buffer;
//       // QImage::Format_RGB888  13  The image is stored using a 24-bit RGB format (8-8-8).
//       imageQt = new QImage((unsigned char *)ubuffer, dimX, dimY, 3*dimX, QImage::Format_RGB888);
//     }
//   else if( gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::MONOCHROME1 ||
//            gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::MONOCHROME2 )
//     {
//       if( gimage.GetPixelFormat() == gdcm::PixelFormat::INT8 || gimage.GetPixelFormat() == gdcm::PixelFormat::UINT8 )
//       {
//         //std::cerr << "Pixel Format is: " << gimage.GetPixelFormat() << std::endl;
//         // We need to copy each individual 8bits into R / G and B:
//         unsigned char *ubuffer = new unsigned char[dimX*dimY*3];
//         unsigned char *pubuffer = ubuffer;
        
//         for(unsigned int i = 0; i < dimX*dimY; i++)
//           {
//             *pubuffer++ = *buffer;
//             *pubuffer++ = *buffer;
//             *pubuffer++ = *buffer++;
//           }

//         imageQt = new QImage(ubuffer, dimX, dimY, 3*dimX, QImage::Format_RGB888);
//       }
//     else if ( gimage.GetPixelFormat() == gdcm::PixelFormat::INT16 || gimage.GetPixelFormat() == gdcm::PixelFormat::UINT16 )
//       {
//         short *input = (short*)buffer;
//         double scaled;
//         unsigned char *output = new unsigned char[dimX*dimY];//*3];
//         unsigned char *poutput = output;
//         unsigned char r;

//         for(unsigned int i = 0; i < dimX*dimY; i++)
//           {
//             scaled = *input * slope + intercept;
//             if (scaled <= center - 0.5 - (window - 1.0 ) / 2 )
//               r = 0;
//             else if (scaled > center - 0.5 + (window - 1.0) / 2)
//               r = 255;
//             else {
//               r = ((scaled - (center - 0.5)) / (window - 1.0) + 0.5) * 255;
//             }
//             *poutput++ = r;

//             /*unsigned char *rgb = colorTransferFunction->MapValue(scaled);
//             *poutput++ = rgb[0];
//             *poutput++ = rgb[1];
//             *poutput++ = rgb[2];*/

//             input++;
//           }

//         //imageQt = new QImage(output, dimX, dimY, 3*dimX, QImage::Format_RGB888);
//         imageQt = new QImage(output, dimX, dimY, dimX, QImage::Format_Indexed8);
//         imageQt->setColorTable(this->backgroundColorTable);

//         //colorTransferFunction->Delete();
//         //lut->Delete();
//       }
//     else
//       {
//         std::cerr << "Pixel Format is: " << gimage.GetPixelFormat() << std::endl;
//         return false;
//       }
//     }
//   else
//     {
//     std::cerr << "Unhandled PhotometricInterpretation: " << gimage.GetPhotometricInterpretation() << std::endl;
//     return false;
//     }

//   return true;
// }

/***********************************************************************************//**
 *
 */
bool MSQDicomImageViewer::convertToARGB32(MSQDicomImage &source)
{
  //const unsigned int* dimension = gimage.GetDimensions();

  //unsigned int dimX = dimension[0];
  //unsigned int dimY = dimension[1];

  //unsigned char *input = (unsigned char *)&source.vbuffer[0];
  int dimX = source.columns;
  int dimY = source.rows;

  // create Qt image
  source.image = QImage(dimX, dimY, QImage::Format_ARGB32_Premultiplied);
  source.buffer.clear();

  // Let's start with the easy case:
  if( source.interpretation == gdcm::PhotometricInterpretation::RGB )
  {
    // RGB image
    if( source.pixelformat != gdcm::PixelFormat::UINT8 )
      {
        return false;
      }
    
    unsigned char *input = (unsigned char *)&source.vbuffer[0];
    unsigned red, green, blue;

    for(unsigned int i = 0; i < dimY; i++) 
      {
        QRgb *row = (QRgb*)source.image.scanLine(i);

        for(unsigned int j = 0; j < dimX; j++)
        {
          red = *input++; green=*input++; blue=*input++;
          source.buffer.push_back(0.2126 * red + 0.7152 * green + 0.0722 * blue);
          *row++ = qRgba(red, green, blue, 255);
        }
      }
  } else if( source.interpretation == gdcm::PhotometricInterpretation::MONOCHROME1 ||
             source.interpretation == gdcm::PhotometricInterpretation::MONOCHROME2 )
  {
    // Grayscale 8-BIT
    if( source.pixelformat == gdcm::PixelFormat::INT8 || source.pixelformat == gdcm::PixelFormat::UINT8 )
    {
      unsigned char *input = (unsigned char *)&source.vbuffer[0];
      for(unsigned int i = 0; i < dimY; i++) 
      {
        QRgb *row = (QRgb*)source.image.scanLine(i);
        for(unsigned int j = 0; j < dimX; j++)
        {
          *row++ = qRgba(*input, *input, *input, 255);
          source.buffer.push_back(*input);
          input++;
        }
      }
      // Grayscale 16-BIT
    } else if ( source.pixelformat == gdcm::PixelFormat::INT16 || source.pixelformat == gdcm::PixelFormat::UINT16 )
    {
      short *input = (short *)&source.vbuffer[0];
      double scaled;
      unsigned char r;

      for(unsigned int i = 0; i < dimY; i++) 
      {
        QRgb *row = (QRgb*)source.image.scanLine(i);
        for(unsigned int j = 0; j < dimX; j++)
        {
          scaled = *input * source.slope + source.intercept;
          
          if (scaled <= source.center - 0.5 - (source.window - 1.0 ) / 2 )
            r = 0;
          else if (scaled > source.center - 0.5 + (source.window - 1.0) / 2)
            r = 255;
          else
            r = ((scaled - (source.center - 0.5)) / (source.window - 1.0) + 0.5) * 255;

          source.buffer.push_back(scaled);

          *row++ = qRgba(
            qRed(source.colorTable[r]), 
            qGreen(source.colorTable[r]), 
            qBlue(source.colorTable[r]), 
            255);

          input++;
        }
      }
    } else
    {
        std::cerr << "Pixel Format is: " << source.pixelformat << std::endl;
        return false;
    }
  } else
  {
    std::cerr << "Unhandled PhotometricInterpretation: " << source.interpretation << std::endl;
    return false;
  }

  return true;
}

/***********************************************************************************//**
 * 
 */
void MSQDicomImageViewer::getMaskLocations(const QImage& mask, std::vector<int>& locations) 
{
  int r, index = 0;
  int dimY = mask.height();
  int dimX = mask.width();

  QRgb *scan = (QRgb *)mask.scanLine(0);

  locations.clear();

  for(unsigned int i = 0; i < dimY; i++) 
  {
    scan = (QRgb*)mask.scanLine(i);
    for(unsigned int j = 0; j < dimX; j++) {
      r = qRed(scan[j]);
      if (r > 0) {
        locations.push_back(index);
      }
      index++;
    }
  }
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::updateInformation()
{
  QString topleft="", topmid="", topright="";
  QString botleft="", botmid="", botright="";
  double entrpy, mean, stdev, snr;

  QImage mask = mLabel->regionOfInterest();
  if ( mask.isNull() )
    return;

  QRect roi = mLabel->getRect();
  //printf("%d %d %d %d\n", roi.x(), roi.y(), roi.width(), roi.height());

  std::vector<int> mask_locations;
  this->getMaskLocations(mask, mask_locations);

  int perc = mLabel->getThresholdPercentage();
  mPerc->setText(QString("%1%").arg(perc));

  // study and series description
  if ( foreground.studydesc.empty() )
    topleft += "<br>";
  else
    topleft += QString::fromStdString(foreground.studydesc) + "<br>";

  if ( !foreground.seriesdesc.empty() )
    topleft += QString::fromStdString(foreground.seriesdesc);

  mHeader[0]->setText(topleft);

  // body part
  if ( !foreground.bodypart.empty() )
    topmid = QString::fromStdString(foreground.bodypart);

  mHeader[1]->setText(topmid);

  // Acquisition date
  if ( foreground.acqdate.empty() ) {
    
    if ( !foreground.acqdatetime.empty() ) {
      QString qstrdate = QString::fromStdString( foreground.acqdatetime );
      topright += QString("%1-%2-%3").arg(qstrdate.left(4)).arg(qstrdate.mid(4, 2)).arg(qstrdate.mid(6, 2));
    } else
      topright += "Unknown date";

  } else {
    
    QString qstrdate = QString::fromStdString( foreground.acqdate );
    topright += QString("%1-%2-%3").arg(qstrdate.left(4)).arg(qstrdate.mid(4, 2)).arg(qstrdate.mid(6, 2));
  }

  // Acquisition time
  if ( foreground.acqtime.empty() ) {
    
    if ( !foreground.acqdatetime.empty() ) {
      QString qstrtime = QString::fromStdString( foreground.acqdatetime );
      topright += QString("<br>%1:%2:%3").arg(qstrtime.mid(8, 2)).arg(qstrtime.mid(10, 2)).arg(qstrtime.mid(12, 2));
    } else
      topright += "<br>Unknown time";

  } else {

      QString qstrtime = QString::fromStdString( foreground.acqtime );
     topright += QString("<br>%1:%2:%3").arg(qstrtime.left(2)).arg(qstrtime.mid(2, 2)).arg(qstrtime.mid(4, 2));
  }

  mHeader[2]->setText(topright);

  // Entropy
  this->statistics(foreground, mask_locations, &entrpy, &mean, &stdev);
  if (isnormal(stdev))
    snr = mean / stdev;
  else snr = 0;

  botleft = QString("<FONT COLOR=Firebrick>Entropy: %1</FONT>").arg(entrpy);
  botleft += QString("<br>Image size: %1 x ").arg(foreground.columns);
  botleft += QString("%1").arg(foreground.rows);
  botleft += QString("<br>Pixel spacing: %1 mm x %2 mm").arg(foreground.resolution[0], 0, 'g', 2).arg(foreground.resolution[1], 0, 'g', 2);

  mFooter[0]->setText(botleft);

  // phase encoding direction
  if ( !foreground.phase.empty() ) {
      botmid = QString::fromStdString( foreground.phase );
  }
  
  mFooter[1]->setText(botmid);

  botright = QString("<FONT COLOR=RoyalBlue>SNR: %1</FONT>").arg(snr);
  //botright += QString("<br><FONT COLOR=RoyalBlue>SD: %1</FONT>").arg(stdev);

  // slice thickness
  if ( foreground.thickness.empty() ) {
      botright += "";
  } else {
      QString str = QString::fromStdString( foreground.thickness );
      botright += QString("<br>Thickness: %1 mm").arg(str.toFloat(), 0, 'g', 2);
  }

  // slice spacing
  if ( foreground.spacing.empty() ) {
      botright += "";
   } else {
      QString str = QString::fromStdString( foreground.spacing );
      botright += QString("<br>Spacing between slices: %1 mm").arg(str.toFloat(), 0, 'g', 2);
   }

   mFooter[2]->setText(botright);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::updateViewer()
{
  // convert background
  if (!convertToARGB32( background ))
  {
    std::cout<<"Could not convert background into QImage..."<<std::endl;
    return;
  }

  // convert foreground
  if (!convertToARGB32( foreground ))
  {
    std::cout<<"Could not convert foreground into QImage..."<<std::endl;
    return;
  }

  // create blended image
  QImage resultImage(foreground.image.size(), QImage::Format_ARGB32_Premultiplied);
  resultImage.fill(qRgba(0, 0, 0, 255));
 
  // blend foreground and background
  QPainter painter(&resultImage);
  painter.setOpacity(background.opacity);
  painter.drawImage(0, 0, background.image);
  painter.setOpacity(foreground.opacity);
  //painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.drawImage(0, 0, foreground.image);

  // set new pixmap
  mLabel->setPixmap( QPixmap::fromImage(resultImage), foreground.buffer );
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::clearBackground(bool update)
{
  // reset background
  background.vbuffer.clear();
  background.vbuffer.resize(foreground.vbuffer.size());
  background.dimensions[0] = foreground.dimensions[0];
  background.dimensions[1] = foreground.dimensions[1];
  background.columns = foreground.columns;
  background.rows = foreground.rows;

  // update viewer if requested
  if (update)
    this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::loadImage(const QString& fileName, MSQDicomImage *dest) 
{
  gdcm::ImageReader reader;
  reader.SetFileName( fileName.toLocal8Bit().constData() );

  if(!reader.Read())
    {
      qDebug() << "Could not open file: " << dest->fileName;  
      return;
    }
  
  dest->fileName = fileName;

  const gdcm::File &file = reader.GetFile();
  const gdcm::Image &gimage = reader.GetImage();
  const double *spacing = gimage.GetSpacing();
  const gdcm::DataSet &ds = file.GetDataSet();

  dest->window = 256;
  dest->center = 128;
  dest->min = 0;
  dest->max = 255;
  dest->slope = 1.0;
  dest->intercept = 0.0;
  dest->resolution[0] = spacing[0];
  dest->resolution[1] = spacing[1];

  dest->interpretation = gimage.GetPhotometricInterpretation();
  dest->pixelformat = gimage.GetPixelFormat();
  const unsigned int *dims = gimage.GetDimensions();
  dest->columns = dims[0];
  dest->rows = dims[1];

  gdcm::Tag tsmallestvalue(0x0028, 0x0106);
  gdcm::Tag tlargestvalue(0x0028, 0x0107);
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

  unsigned long len = gimage.GetBufferLength();
  //std::vector<char> vbuffer;
  dest->vbuffer.resize( len );
  char *buffer = &dest->vbuffer[0];
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
    //intercept = std::stod(sri); // C++ 11
    dest->intercept = ::atof(sri.c_str());
    //slope = std::stod(srs); // C++ 11
    dest->slope = ::atof(srs.c_str());
  }

  // smallest value and largest value
  if( ds.FindDataElement( tsmallestvalue ) && ds.FindDataElement( tlargestvalue) )
  {
    const gdcm::DataElement& smallest = ds.GetDataElement( tsmallestvalue );
    const gdcm::DataElement& largest = ds.GetDataElement( tlargestvalue );
    gdcm::StringFilter sf1;
    sf1.SetFile(file);
    gdcm::StringFilter sf2;
    sf2.SetFile(file);

    if ( !smallest.IsEmpty() && !largest.IsEmpty() ) {

      std::string s1 = sf1.ToString( tsmallestvalue );
      std::string s2 = sf2.ToString( tlargestvalue );
      dest->min = ::atoi(s1.c_str());
      dest->max = ::atoi(s2.c_str());
    
    } 
      
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
          dest->window = elww.GetValue(0);
          dest->center = elwc.GetValue(0);
          //printf("*window=%f, center=%f, length=%lu\n",window,center,elwc.GetLength());
        }
    }
  } else {
    //dest->center = dest->min + (dest->window) / 2;
    dest->window = (dest->max - dest->min) * 0.6;
    dest->center = dest->window * 0.42;
  }

  //printf("w: %d, c: %d, min: %d, max: %d\n", dest->window, dest->center, dest->min, dest->max);

  // Study and series description 
  if ( ds.FindDataElement (tstudesc) ) {
    const gdcm::DataElement& studesc = ds.GetDataElement( tstudesc );
    const gdcm::ByteValue *bvstu = studesc.GetByteValue();
    if (bvstu)
      dest->studydesc = std::string( bvstu->GetPointer(), bvstu->GetLength() );
    else
      dest->studydesc = "";
  }

  if ( ds.FindDataElement (tseriesdesc) ) {
    const gdcm::DataElement& seriesdesc = ds.GetDataElement( tseriesdesc );
    const gdcm::ByteValue *bvsed = seriesdesc.GetByteValue();
    if (bvsed)
      dest->seriesdesc = std::string( bvsed->GetPointer(), bvsed->GetLength() );
    else
      dest->seriesdesc = "";
  }
  
  if ( ds.FindDataElement (tbodypart) ) {
    const gdcm::DataElement& bodypart = ds.GetDataElement( tbodypart );
    const gdcm::ByteValue *bvbp = bodypart.GetByteValue();
    if (bvbp)
      dest->bodypart = std::string( bvbp->GetPointer(), bvbp->GetLength() );
    else
      dest->bodypart = "";
  }

  // Acquisition date and time
  if ( !ds.FindDataElement (tacqdate) )
  {
    if ( !ds.FindDataElement (tacqdatetime) ) {
      dest->acqdate = "";
    } else {
      const gdcm::DataElement& acqdate = ds.GetDataElement( tacqdatetime );
      const gdcm::ByteValue *bvdate = acqdate.GetByteValue();
      if (bvdate) {
        dest->acqdate = std::string( bvdate->GetPointer(), bvdate->GetLength() );
      } else {
        dest->acqdate = "";
      }
    }
  } else {
    const gdcm::DataElement& acqdate = ds.GetDataElement( tacqdate );
    const gdcm::ByteValue *bvdate = acqdate.GetByteValue();
    if (bvdate) {
      dest->acqdate = std::string( bvdate->GetPointer(), bvdate->GetLength() );
    } else {
      dest->acqdate = "";
    }
  }

  if ( !ds.FindDataElement (tacqtime) ) 
  {
    if ( !ds.FindDataElement (tacqdatetime) )
        dest->acqtime = "";
    else {
      const gdcm::DataElement& acqtime = ds.GetDataElement( tacqdatetime );
      const gdcm::ByteValue *bvtime = acqtime.GetByteValue();
      if (bvtime) {
        dest->acqtime = std::string( bvtime->GetPointer(), bvtime->GetLength() );
      } else{
        dest->acqtime = "";
      }
    }
  } else {
    const gdcm::DataElement& acqtime = ds.GetDataElement( tacqtime );
    const gdcm::ByteValue *bvtime = acqtime.GetByteValue();
    if (bvtime) {
      dest->acqtime = std::string( bvtime->GetPointer(), bvtime->GetLength() );
    } else {
      dest->acqtime = "";
    }
  }

  // phase encoding direction
  if ( ds.FindDataElement (tphase) ) {
    const gdcm::DataElement& phase = ds.GetDataElement( tphase );
    const gdcm::ByteValue *bvphase = phase.GetByteValue();
    if (bvphase) 
      dest->phase = std::string( bvphase->GetPointer(), bvphase->GetLength() );
    else
      dest->phase = "";
  }
  
  // slice thickness
  if ( !ds.FindDataElement (tthickness) )
  {
    dest->thickness = "";
  } else {
    const gdcm::DataElement& thick = ds.GetDataElement( tthickness );
    const gdcm::ByteValue *bvthick = thick.GetByteValue();
    if (bvthick) {
      dest->thickness = std::string( bvthick->GetPointer(), bvthick->GetLength() );
    } else {
      dest->thickness = "";
    }
  }

  if ( !ds.FindDataElement (tspacing) )
    dest->spacing = "";
  else {
    const gdcm::DataElement& space = ds.GetDataElement( tspacing );
    const gdcm::ByteValue *bvspace = space.GetByteValue();
    if (bvspace) {
      dest->spacing = std::string( bvspace->GetPointer(), bvspace->GetLength() );
    } else {
      dest->spacing = "";
    }
  }
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setInput(const QString& fileName) 
{
  // load foreground image
  this->loadImage(fileName, &this->foreground);

  // sizes differ, clear background
  if (this->foreground.vbuffer.size() != this->background.vbuffer.size())
    this->clearBackground();

  // update display
  this->updateViewer();

  // set information
  this->updateInformation();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setBackground(const QString& fileName) 
{
  // load background image
  this->loadImage(fileName, &this->background);

  // update display
  this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setForegroundColormap(vtkmsqLookupTable *lut) 
{
  double *color;
  int numColors = lut->GetNumberOfTableValues();
  
  // clear table
  this->foreground.colorTable.clear();
  
  // copy table
  for(int c=0;c<numColors;c++)
  {
    color = lut->GetTableValue(c);
    this->foreground.colorTable.append(qRgb(color[0]*255, color[1]*255, color[2]*255));
  }

  //colorTransferFunction->BuildFunctionFromTable( center-window/2, center+window/2, 255, (double*) &this->colorTable);

  // redisplay image
  this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setBackgroundColormap(vtkmsqLookupTable *lut) 
{
  double *color;
  int numColors = lut->GetNumberOfTableValues();
  
  // clear table
  this->background.colorTable.clear();
  
  // copy table
  for(int c=0;c<numColors;c++)
  {
    color = lut->GetTableValue(c);
    this->background.colorTable.append(qRgb(color[0]*255, color[1]*255, color[2]*255));
  }
  
  //colorTransferFunction->BuildFunctionFromTable( center-window/2, center+window/2, 255, (double*) &this->colorTable);

  // redisplay image
  this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setForegroundOpacity(qreal opacity) 
{
  foreground.opacity = opacity;

  // redisplay image
  this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::setBackgroundOpacity(qreal opacity) 
{
  background.opacity = opacity;

  // redisplay image
  this->updateViewer();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::regionOfInterestChanged()
{
  this->updateInformation();
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
void MSQDicomImageViewer::thresholdButtonClick()
{
  mLabel->setCursorToThreshold();
  mLabel->setCursorEnabled(true);
  mLabel->setCursorFilled(false);
  roiType = 6;
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
int MSQDicomImageViewer::getThresholdPercentage() 
{
  return mLabel->getThresholdPercentage();
}

/***********************************************************************************//**
 *
 */
QImage MSQDicomImageViewer::rectangularRegionOfInterest() const
{
  return mLabel->rectangularRegionOfInterest();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageViewer::refresh()
{
  //this->vtkWidget->update();
}

