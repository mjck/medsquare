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
  virtual void setColormap(vtkmsqLookupTable *lut);
  virtual void setBackgroundOpacity( qreal opacity );
  virtual void setForegroundOpacity( qreal opacity );
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

  // current colormap
  QVector<QRgb> colorTable;
  //vtkColorTransferFunction *colorTransferFunction;
  //double colorTable[256][3];
  //double window, center;

  MSQAspectRatioPixmapLabel *mLabel;
  
  void buildFrame();
  void createInterface();
  void statistics(gdcm::Image const & gimage, char *buffer, const QImage& mask, 
    double window, double center, double slope, double intercept,
    double *entropy, double *mean, double *stdev);
  bool ConvertToFormat_RGB888(gdcm::Image const & gimage, char *buffer, QImage* &imageQt, 
    double window, double center, double slope, double intercept);
  void updateViewer();
};

#endif

