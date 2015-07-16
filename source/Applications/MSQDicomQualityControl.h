/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomQualityControl.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOMIMAGE_QUALITYCONTROL_H
#define MSQ_DICOMIMAGE_QUALITYCONTROL_H

#include "MSQDicomImageViewer.h"

#include <QtGui>

#include "gdcmSorter.h"
#include "gdcmElement.h"
#include "gdcmSerieHelper.h"
#include "gdcmFile.h"
#include "gdcmReader.h"
#include "gdcmImageReader.h"
#include "gdcmStringFilter.h"

class MSQDicomQualityControl : public QWidget
{
Q_OBJECT

public:
  MSQDicomQualityControl();
  virtual ~MSQDicomQualityControl();
  void setDicomViewer(MSQDicomImageViewer *viewer);
  void setDicomTree(QTreeWidgetItem *topItem);
  void reset();

public slots:
  virtual void setVisible(bool visible);

private slots:
  //void wholeButtonClick();
  //void rectButtonClick();
  //void ellipseButtonClick();
  //void arcButtonClick();
  //void filledButtonClick();
  void checkQuality();

signals:
  void regionOfInterestChanged();

protected:

  MSQDicomImageViewer *mDicomViewer;
  QProgressDialog *mProgressDialog;
  QTreeWidgetItem *mDicomTree;
  QPushButton *mQualityButton;
  //QPushButton *mCloseButton;
  QLineEdit *mQualityFrom, *mDistFrom;
  QLineEdit *mQualityTo, *mDistTo;
  //QRadioButton *mWholeButton;
  //QRadioButton *mRectButton;
  //QRadioButton *mEllipseButton;
  //QRadioButton *mArcButton;
  //QRadioButton *mEntropyButton;
  //QRadioButton *mMeanButton;
  //QRadioButton *mSD;
  QComboBox *mMeasureBox;
  QCheckBox *mRangeButton, *mDistButton;
  QCheckBox *mSelectionButton;
  //QCheckBox *mFilledButton;

  void createInterface();
  double calculateStat(std::string fileName, const QImage& mask, int type);
  short equalize(short input, double window, double center);
  void fileCheckQualityRecursive(QTreeWidgetItem *item, const QImage& mask, 
    bool selection, double toppercfrom, double toppercto);
  void statistics(gdcm::Image const & gimage, char *buffer, const QImage& mask, 
    double window, double center, double *entropy, double *mean, double *stdev);

 };

#endif

