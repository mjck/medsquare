/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomImageViewer.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOMIMAGE_VIEWER_H
#define MSQ_DICOMIMAGE_VIEWER_H

#include <QtGui>
#include <QImage>

#include "MSQAspectRatioPixmapLabel.h"
#include "MSQDicomImageViewerButton.h"
#include "gdcmImageReader.h"

//#include "vtkColorTransferFunction.h"
//#include "vtkWindowLevelLookupTable.h"

#include "vtkmsqLookupTable.h"

class MSQDicomImage
{
public:
  MSQDicomImage() {
    opacity = 255;
    slope = 1.0;
    intercept = 0.0;
    window = 256;
    center = 128;
    min = 0;
    max = 255;
    dimensions[0] = columns = 25;
    dimensions[1] = rows = 25;
    resolution[0] = resolution[1] = 1.0;
    vbuffer.resize(625);
    pixelformat = gdcm::PixelFormat::UINT8;
    interpretation = gdcm::PhotometricInterpretation::MONOCHROME1;
    colorTable.clear();
    for(int c=0;c<256;c++)
      colorTable.append(qRgb(c, c, c));
  }

  QString fileName;
  std::vector<char> vbuffer;

  // Qt specific
  QImage image;
  QVector<QRgb> colorTable;
  qreal opacity;

  // pixel mapping
  double slope, intercept;
  double resolution[2];
  int window, center;
  int min, max;
  int dimensions[2];
  int columns, rows;

  // auxiliary information
  std::string studydesc;
  std::string seqdesc;
  std::string seriesdesc;
  std::string bodypart;
  std::string acqtime;
  std::string acqdate;
  std::string acqdatetime;
  std::string thickness;
  std::string spacing;
  std::string phase;

  // GDCM specific 
  gdcm::PixelFormat pixelformat;
  gdcm::PhotometricInterpretation interpretation;
};

class MSQDicomImageViewer : public QWidget
{
Q_OBJECT

public:
  MSQDicomImageViewer();
  virtual ~MSQDicomImageViewer();

  void reset();

  MSQAspectRatioPixmapLabel *label();

  void refresh();

  virtual void setInput(const QString& fileName);
  virtual void setBackground(const QString& fileName);
  virtual void clearBackground(bool update = false);
  virtual void setForegroundOpacity( qreal opacity );
  virtual void setBackgroundOpacity( qreal opacity );
  virtual void setForegroundColormap(vtkmsqLookupTable *lut);
  virtual void setBackgroundColormap(vtkmsqLookupTable *lut);

  QImage regionOfInterest() const;

  void showToolBar(bool show);
  void enableToolBar(bool enable);

public slots:
  //virtual void setComponent(int component) = 0;
  //virtual void setLevel(double value) = 0;
  //virtual void setWindow(double value) = 0;
  //virtual void setOpacity(double value) = 0;
private slots:
  void qualityChanged();
  void regionOfInterestChanged();
  void penSizeChanged();
  void wholeButtonClick();
  void rectButtonClick();
  void rectFilledButtonClick();
  void ellipseButtonClick();
  void ellipseFilledButtonClick();
  void arcButtonClick();

protected:
  int roiType;
  bool highQuality;

  QLabel *mHeader[3];
  QLabel *mFooter[3];
  QString fileName;
  QWidget *mButtonBar;
  MSQDicomImageViewerButton *mWhole, *mRect, *mRectFilled;
  MSQDicomImageViewerButton *mEllipse, *mEllipseFilled;
  MSQDicomImageViewerButton *mArc;
  QToolButton *mOnepx, *mThreepx, *mFivepx;
  QToolButton *mQuality;

  //vtkColorTransferFunction *colorTransferFunction;
  //double colorTable[256][3];
  //double window, center;

  MSQDicomImage foreground;
  MSQDicomImage background;

  std::vector<char> foregroundBuffer;
  QImage foregroundImage;
  QVector<QRgb> foregroundColorTable;
  qreal foregroundOpacity;

  std::vector<char> backgroundBuffer;
  QImage backgroundImage;
  QVector<QRgb> backgroundColorTable;
  qreal backgroundOpacity;

  MSQAspectRatioPixmapLabel *mLabel;
  
  void buildFrame();
  void createInterface();
  void loadImage(const QString& fileName, MSQDicomImage *dest);
  void statistics(const MSQDicomImage& source, const QImage& mask, double *entropy, double *mean, double *stdev);
  //bool ConvertToFormat_RGB888(gdcm::Image const & gimage, char *buffer, QImage* &imageQt, 
  //  double window, double center, double slope, double intercept);
  bool convertToARGB32(MSQDicomImage &source);
  void updateInformation();
  void updateViewer();
};

#endif

