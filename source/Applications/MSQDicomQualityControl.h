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

class combination {
public:

  struct comb_type {
    std::vector<int> vec;
    double snr;
    double entropy;
  };

  std::vector<comb_type> list; 

  void add(double snr, double entropy, std::vector<int> vec) {

    comb_type item;
    item.snr = snr;
    item.entropy = entropy;
    item.vec = vec;
    list.push_back(item);

  }

  void print()
  {
    std::cout << "total: " << list.size() << std::endl;

    for(int i = 0; i < list.size(); i++) {
      for(int j = 0; j < list[i].vec.size(); j++)
        std::cout << " " << list[i].vec[j];
      std::cout << " // snr: " << list[i].snr << ", entropy: " << list[i].entropy << std::endl;
    }

  }

  // generate all possible combinations of N items, in lengths 1 to N
  void generate(int N) {

    list.clear();

    for(int k=N; k>0; k--) {
      comb(N, k);
    }
  }

  // generate combinations of N items of length K
  void comb(int N, int K) {
    
    std::string bitmask(K, 1); // K leading 1's
    bitmask.resize(N, 0); // N-K trailing 0's
    std::vector<int> item;

    // print integers and permute bitmask
    do {

      item.clear();

      for (int i = 0; i < N; ++i) // [0..N-1] integers
      {
          if (bitmask[i]) {
            //std::cout << " " << i;
            item.push_back(i);
          }
      }
      //std::cout << std::endl;
      add(0, -1, item);

    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
  }

};

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
  QComboBox *mMethodBox;
  QCheckBox *mRangeButton, *mDistButton;
  QCheckBox *mSelectionButton;
  //QCheckBox *mFilledButton;

  void createInterface();
  double calculateStat(std::string fileName, const QImage& mask, int type);
  short equalize(short input, double window, double center);
  void getMaskLocations(const QImage& mask, int dimX, int dimY, std::vector<int>& locations); 
  void fileCheckQualityRecursive(QTreeWidgetItem *item, const QImage& mask, 
    bool selection, double toppercfrom, double toppercto);
  void collectFilenamesRecursive(QTreeWidgetItem *item, bool selection, std::vector<std::string>& fileNames, std::vector<QTreeWidgetItem *>& qtItems);
  void fileCheckQualityCombinations(std::vector<std::string>& fileNames,  std::vector<QTreeWidgetItem *>& qtItems, 
    const QImage& mask, combination& cmb, int option);
  void statistics(gdcm::Image const & gimage, char *buffer, const QImage& mask, double *entropy, double *mean, double *stdev);
  
  void addAverage(std::string fileName, std::vector<int>& mask, std::vector<float>& average);
  void getStatistics(std::vector<float>& average, double *entropy, double *mean, double *stdev);

  void calculateAverage(std::string fileName, const QImage& mask, float *output, float factor);
  void stat_average(float *buffer, const QImage& mask, int dimX, int dimY, double *entropy, double *mean, double *stdev);

 };

#endif

