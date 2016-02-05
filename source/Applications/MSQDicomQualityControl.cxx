/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomQualityControl.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomQualityControl.h"

#include <qglobal.h>

#include <QtCore>
#include <QtGui>

#include <set>
#include <algorithm>
#include <iostream>
#include <string>

typedef std::pair<double, int> stat_pair;
bool stat_compare(const stat_pair& a, const stat_pair& b)
{
  return a.first > b.first;
}

/***********************************************************************************//**
 *
 */
MSQDicomQualityControl::MSQDicomQualityControl()
{
  //mLabel = NULL;
  mDicomTree = NULL;
  mDicomViewer = NULL;
  createInterface();
}

/***********************************************************************************//**
 *
 */
void MSQDicomQualityControl::createInterface()
{
  QFont font;
  font.setPointSize(11);

  QFont font12;
  font12.setPointSize(12);
  font12.setBold(true); 

  QFont boldFont;
  boldFont.setPointSize(12);
  boldFont.setBold(false);  

  mProgressDialog = new QProgressDialog(this);

  // create main layout
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setContentsMargins(QMargins(4,0,4,0));
  mainLayout->setSpacing(0);
  
  QLabel *label = new QLabel("Measure: ", this);
  label->setFont(font);

  mMethodBox = new QComboBox;
  mMethodBox->addItem("Entropy measure");
  mMethodBox->addItem("SNR measure");
  mMethodBox->addItem("SNR maximization");
  mMethodBox->addItem("Entropy minimization");
  mMethodBox->addItem("SNR max / Entropy min");
  mMethodBox->addItem("Entropy min / SNR max");
  //mMethodBox->addItem("Mean intensity");
  //mMethodBox->addItem("SD of intensity");

  mSelectionButton = new QCheckBox("Perform quality check on marked images only");
  mSelectionButton->setFont(font);

  mRangeButton = new QCheckBox("Use range");
  mRangeButton->setStyleSheet("min-width: 120px; max-width: 120px;");
  mRangeButton->setFont(font);
  mRangeButton->setChecked(true);
  //mRangeButton->setContentsMargins(12,0,12,0);

  QLabel *dummy1 = new QLabel("From:");
  dummy1->setFont(font);

  mQualityFrom = new QLineEdit("0.0", this);
  mQualityFrom->setPlaceholderText("0.0");
  mQualityFrom->setFont(font);
  mQualityFrom->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  mQualityFrom->setFixedWidth(40);

  QLabel *dummy2 = new QLabel("to");
  dummy2->setFont(font);

  mQualityTo = new QLineEdit("90.0", this);
  mQualityTo->setPlaceholderText("90.0");
  mQualityTo->setFont(font);
  mQualityTo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  mQualityTo->setFixedWidth(40);

  //mQuality->setEnabled(false);
  QDoubleValidator *validator = new QDoubleValidator;
  validator->setBottom(1);
  validator->setTop(100);
  validator->setDecimals(1);
  mQualityFrom->setValidator(validator);
  mQualityTo->setValidator(validator);

  QLabel *dummy3 = new QLabel("% highest values");
  dummy3->setFont(font);


  mDistButton = new QCheckBox("Use histogram");
  mDistButton->setStyleSheet("min-width: 120px; max-width: 120px;");
  mDistButton->setFont(font);

  QLabel *dummy4 = new QLabel("From:");
  dummy4->setFont(font);

  mDistFrom = new QLineEdit("0.0", this);
  mDistFrom->setPlaceholderText("0.0");
  mDistFrom->setFont(font);
  mDistFrom->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  mDistFrom->setFixedWidth(40);

  QLabel *dummy5 = new QLabel("to");
  dummy5->setFont(font);

  mDistTo = new QLineEdit("3.0", this);
  mDistTo->setPlaceholderText("3.0");
  mDistTo->setFont(font);
  mDistTo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  mDistTo->setFixedWidth(40);

  mDistFrom->setValidator(validator);
  mDistTo->setValidator(validator);

  QLabel *dummy6 = new QLabel("SDs from the mean");
  dummy6->setFont(font);


  mQualityButton = new QPushButton("Check quality", this);
  QObject::connect(mQualityButton, SIGNAL(clicked()), this, SLOT(checkQuality()));
  //mQualityButton->setEnabled(false);
  mQualityButton->setFont(font);
  mQualityButton->setStyleSheet("min-width: 50px; max-height: 25px;");

  QButtonGroup *buttonGroup1 = new QButtonGroup(this);
  buttonGroup1->setExclusive(true);
  buttonGroup1->addButton(mRangeButton);
  buttonGroup1->addButton(mDistButton);

  QHBoxLayout *headerLayout = new QHBoxLayout;
  headerLayout->setContentsMargins(QMargins(0,0,0,12));
  headerLayout->setSpacing(10);
  headerLayout->addWidget(label);
  headerLayout->addWidget(mMethodBox);
  headerLayout->addStretch();
  headerLayout->addWidget(mQualityButton);

  QHBoxLayout *midLayout = new QHBoxLayout;
  midLayout->setContentsMargins(QMargins(0,0,0,12));
  midLayout->setSpacing(10);
  midLayout->addWidget(mRangeButton);
  midLayout->addWidget(dummy1);
  midLayout->addWidget(mQualityFrom);
  midLayout->addWidget(dummy2);
  midLayout->addWidget(mQualityTo);
  midLayout->addWidget(dummy3);
  midLayout->addStretch();

  QHBoxLayout *midLayout2 = new QHBoxLayout;
  midLayout2->setContentsMargins(QMargins(0,0,0,12));
  midLayout2->setSpacing(10);
  midLayout2->addWidget(mDistButton);
  midLayout2->addWidget(dummy4);
  midLayout2->addWidget(mDistFrom);
  midLayout2->addWidget(dummy5);
  midLayout2->addWidget(mDistTo);
  midLayout2->addWidget(dummy6);
  midLayout2->addStretch();

  QHBoxLayout *footerLayout = new QHBoxLayout;
  footerLayout->setContentsMargins(QMargins(0,0,0,12));
  footerLayout->setSpacing(10);
  footerLayout->addWidget(mSelectionButton);

  //QFrame* line = new QFrame(this);
  //line->setGeometry(QRect());
  //line->setFrameShape(QFrame::HLine); // Replace by VLine for vertical line
  //line->setFrameShadow(QFrame::Sunken);
  //line->setLineWidth(1);

  mainLayout->addLayout(headerLayout);
  mainLayout->addLayout(midLayout);
  mainLayout->addLayout(midLayout2);
  mainLayout->addStretch();
  mainLayout->addLayout(footerLayout);
  //mainLayout->addWidget(line);
  //mainLayout->addLayout(footerLayout);

  this->setLayout(mainLayout);
  //this->setEnabled(false);
  //this->setContentsMargins(QMargins(0,0,0,0));
}

/***********************************************************************************//**
 *
 */
void MSQDicomQualityControl::setDicomViewer(MSQDicomImageViewer *viewer)
{
  mDicomViewer = viewer;
}

/***********************************************************************************//**
 *
 */
void MSQDicomQualityControl::setDicomTree(QTreeWidgetItem *topItem)
{
  mDicomTree = topItem;
}

/***********************************************************************************//**
 * 
 */
double MSQDicomQualityControl::calculateStat(std::string fileName, const QImage& mask, int type)
{

  //double window = 256, center = 128;
  //gdcm::Tag twindowcenter(0x0028, 0x1050);
  //gdcm::Tag twindowwidth(0x0028, 0x1051);

  gdcm::ImageReader reader;

  const gdcm::File &file = reader.GetFile();
  const gdcm::Image &gimage = reader.GetImage();
  reader.SetFileName(fileName.c_str());
  //printf("%s\n",fileName.c_str());
  if (!reader.Read()) {
    //this->warningMessage(QString("Could not read file %1").arg(QString::fromStdString(fileName)), "Pleack check and try again."); 
    // if it falls here, it is not a DICOM image.
    return -1;
  }

  //const gdcm::Image &image = reader.GetImage();
  //const gdcm::DataSet &ds = file.GetDataSet();

   // window and level
  //if( ds.FindDataElement( twindowcenter ) && ds.FindDataElement( twindowwidth) )
  //{
   // const gdcm::DataElement& windowcenter = ds.GetDataElement( twindowcenter );
   // const gdcm::DataElement& windowwidth = ds.GetDataElement( twindowwidth );
   // const gdcm::ByteValue *bvwc = windowcenter.GetByteValue();
   // const gdcm::ByteValue *bvww = windowwidth.GetByteValue();

   // if( bvwc && bvww ) // Can be Type 2
   // {
        //gdcm::Attributes<0x0028,0x1050> at;
  //       gdcm::Element<gdcm::VR::DS,gdcm::VM::VM1_n> elwc;
  //       std::stringstream ss1;
  //       std::string swc = std::string( bvwc->GetPointer(), bvwc->GetLength() );
  //       ss1.str( swc );
  //       gdcm::VR vr = gdcm::VR::DS;
  //       unsigned int vrsize = vr.GetSizeof();
  //       unsigned int count = gdcm::VM::GetNumberOfElementsFromArray(swc.c_str(), (unsigned int)swc.size());
  //       elwc.SetLength( count * vrsize );
  //       elwc.Read( ss1 );
  //       std::stringstream ss2;
  //       std::string sww = std::string( bvww->GetPointer(), bvww->GetLength() );
  //       ss2.str( sww );
  //       gdcm::Element<gdcm::VR::DS,gdcm::VM::VM1_n> elww;
  //       elww.SetLength( count * vrsize );
  //       elww.Read( ss2 );

  //       if (elwc.GetLength() > 0)
  //       {
  //         window = elww.GetValue(0);
  //         center = elwc.GetValue(0);
  //       }
  //   }
  // }

  unsigned long len = gimage.GetBufferLength();
  std::vector<char> vbuffer;
  vbuffer.resize( len );
  char *buffer = &vbuffer[0];
  gimage.GetBuffer(buffer);

  // Calculate entropy
  double entropy, mean, stdev;

  this->statistics(gimage, buffer, mask, &entropy, &mean, &stdev);

  if (type == 0)
    return entropy;
  else {
    if (isnormal(stdev))
      return mean / stdev;
    else
      return 0;
  }

    //if (type == 1)
    //return mean;
  //else return stdev;
  //return (type == 0) ? entropy : mean;
  //printf("entropy=%f, mean=%f\n",entropy,mean);

}

/***********************************************************************************//**
 * 
 */
void MSQDicomQualityControl::collectFilenamesRecursive(QTreeWidgetItem *item, bool selection, 
  std::vector<std::string>& fileNames, std::vector<QTreeWidgetItem *>& qtItems)
{
  if (item->childCount() > 0) {

    if ((selection && item->checkState(0)) || !selection || item->parent() == NULL) {

      if (item->child(0)->childCount() > 0)
      {
        for(int i=0; i<item->childCount(); i++) {

          if ((selection && item->child(i)->checkState(0)) || !selection)
            collectFilenamesRecursive(item->child(i), selection, fileNames, qtItems);

        } 
    
      } else {

        // fetch entropy values
        for(int i=0; i<item->childCount(); i++) {

          if ((selection && item->child(i)->checkState(0)) || !selection) {

            fileNames.push_back(item->child(i)->text(0).toStdString());
            qtItems.push_back(item->child(i));
            //cout << item->child(i)->text(0).toStdString() << std::endl;

          }

        }

      }
    }
  }
}

/***********************************************************************************//**
 * 
 */
void MSQDicomQualityControl::fileCheckQualityCombinations(
  std::vector<std::string>& fileNames, 
  std::vector<QTreeWidgetItem *>& qtItems,
  const QImage& mask, combination& cmb,
  int option)
{
  int num; 
  float factor;
  double entropy, mean, stdev;
  //double window = 256, center = 128;
  //std::vector<char> vbuffer;

  //gdcm::Tag twindowcenter(0x0028, 0x1050);
  //gdcm::Tag twindowwidth(0x0028, 0x1051);

  gdcm::ImageReader reader;
  gdcm::File &file = reader.GetFile();
  gdcm::Image &gimage = reader.GetImage();

  // read first image
  reader.SetFileName(fileNames[0].c_str());
  if (!reader.Read()) {
      std::cout << "could not read image" << std::endl;
      return;
  }

  // buffer length
  const unsigned int* dimension = gimage.GetDimensions();
  int dimX = dimension[0];
  int dimY = dimension[1];
  std::vector<float> average(dimX * dimY);
  float *avg = &average[0];

  //cout << "dimX: " << dimX << " dimY: " << dimY << std::endl;

  // compute SNR and entropy for all combinations
  for(int k = 0; k < cmb.list.size(); k++) {

    // compute set size and average factor
    num = cmb.list[k].vec.size();
    factor = 1.0 / num;

    // zero average image
    std::fill(average.begin(), average.end(), 0);

    for(int i = 0; i < num; i++) {

      // average image
      this->calculateAverage(fileNames[cmb.list[k].vec[i]], mask, avg, factor);
    }

    // done averaging, now compute SNR and entropy for this set
    this->stat_average(avg, mask, dimX, dimY, &entropy, &mean, &stdev);

    // store snr and entropy
    if (isnormal(stdev))
      cmb.list[k].snr = mean / stdev;
    else
      cmb.list[k].snr = 0;
  
    cmb.list[k].entropy = entropy;

  }

  int index = 0;
  double val;

  if (option == 2) {

    double max_snr = cmb.list[0].snr;
    for(int k = 1; k < cmb.list.size(); k++)
    {
      if (cmb.list[k].snr > max_snr) {
        max_snr = cmb.list[k].snr;
        index = k;
      }  
    }

    cout <<  "selected ==> snr: " << max_snr << std::endl;

  } else if (option == 3) {

    double min_entropy = cmb.list[0].entropy;
    for(int k = 1; k < cmb.list.size(); k++)
    {
      if (cmb.list[k].entropy < min_entropy) {
        min_entropy = cmb.list[k].entropy;
        index = k;
      }  
    }

    cout <<  "selected ==> entropy: " << min_entropy << std::endl;

  } else if (option == 4) {
 
    double max_snr = cmb.list[0].snr;
    double min_entropy = cmb.list[0].entropy;
    for(int k = 1; k < cmb.list.size(); k++)
    {
      if (cmb.list[k].snr > max_snr && cmb.list[k].entropy < min_entropy) {
        max_snr = cmb.list[k].snr;
        min_entropy = cmb.list[k].entropy;
        index = k;
      }  
    }

    cout <<  "selected ==> snr: " << max_snr << " , entropy: " << min_entropy << std::endl;
  
  } else {

    double max_snr = cmb.list[0].snr;
    double min_entropy = cmb.list[0].entropy;
    for(int k = 1; k < cmb.list.size(); k++)
    {
      if (cmb.list[k].entropy < min_entropy && cmb.list[k].snr > max_snr) {
        max_snr = cmb.list[k].snr;
        min_entropy = cmb.list[k].entropy;
        index = k;
      }  
    }

     cout <<  "selected ==> snr: " << max_snr << " , entropy: " << min_entropy << std::endl;
  }

  bool found;

  for(int i = 0; i < cmb.list[0].vec.size(); i++) {
    
    found = false;
    
    for(int j = 0; j < cmb.list[index].vec.size() && !found; j++)
    {
      //cout << " " << i << ", " << cmb.list[maxk].vec[j] << std::endl;
      if (cmb.list[index].vec[j] == i) {
        qtItems[cmb.list[index].vec[j]]->setCheckState(0, Qt::Checked);
        found = true;
      } 
    }

    if (found)
        qtItems[i]->setCheckState(0, Qt::Checked);
      else
        qtItems[i]->setCheckState(0, Qt::Unchecked);
  }
}

/***********************************************************************************//**
 * 
 */
void MSQDicomQualityControl::fileCheckQualityRecursive(QTreeWidgetItem *item, 
  const QImage& mask, bool selection, double toppercfrom, double toppercto)
{
  if (item->childCount() > 0) {

    if ((selection && item->checkState(0)) || !selection || item->parent() == NULL) {

      if (item->child(0)->childCount() > 0)
      {
        for(int i=0; i<item->childCount(); i++) {

          if ((selection && item->child(i)->checkState(0)) || !selection)
            fileCheckQualityRecursive(item->child(i), mask, selection, toppercfrom, toppercto);

        } 
    
      } else {

        std::vector<stat_pair> vec;

        double value;
        double sum=0, sum2=0;
        double mean=0, stdev=0;

        // fetch entropy values
        for(int i=0; i<item->childCount(); i++) {

          if ((selection && item->child(i)->checkState(0)) || !selection) {
          
            value = calculateStat(item->child(i)->text(0).toStdString(), mask, mMethodBox->currentIndex());
              //mEntropyButton->isChecked() ? 0 : (mMeanButton->isChecked() ? 1 : 2));
            //item->setData(1, Qt::UserRole, QVariant(entropy));

            sum += value;
            sum2 += value * value;

            vec.push_back(std::make_pair(value, i));
            //printf("%f, %d\n", value, i);
            //printf("%d %d\n",i, item->child(i)->checkState(0));
          }

           mean = sum / vec.size();
           stdev = sqrt((sum2 / vec.size()) - (mean * mean));

        }

        if (this->mRangeButton->isChecked()) {

          //printf("using range\n");
        
          // sort
          std::sort(vec.begin(), vec.end(), stat_compare);

          int fromp = vec.size()*toppercfrom;
          int top = vec.size()*toppercto;

          // uncheck
          for(int i=0; i<vec.size(); i++) {
            if (i >= fromp && i < top)
              item->child(vec[i].second)->setCheckState(0, Qt::Checked);
            else
              item->child(vec[i].second)->setCheckState(0, Qt::Unchecked);
          }

        } else {
        
          double from = mean + stdev * mDistFrom->text().toDouble();
          double to = mean + stdev * mDistTo->text().toDouble();
          double nfrom = mean - stdev * mDistTo->text().toDouble();
          double nto = mean - stdev * mDistFrom->text().toDouble();

          //printf("using distribution [from=%lf, to=%lf], %lf, [from=%lf, to=%lf]\n", nfrom, nto, mean, from, to);

          for(int i=0; i<vec.size(); i++) {

            if ((vec[i].first >= from && vec[i].first <= to) || 
                (vec[i].first >= nfrom && vec[i].first <= nto))
               item->child(vec[i].second)->setCheckState(0, Qt::Checked);
             else
               item->child(vec[i].second)->setCheckState(0, Qt::Unchecked);

          }

        }

      }

    }
  }
}

/***********************************************************************************//**
 * 
 */
inline short MSQDicomQualityControl::equalize(short input, double window, double center)
{
  if (input <= center - 0.5 - (window - 1.0 ) / 2 )
    return 0;
  else if (input > center - 0.5 + (window - 1.0) / 2)
    return 255;
  else 
    return ((input - (center - 0.5)) / (window - 1.0) + 0.5) * 255;
}

/***********************************************************************************//**
 * 
 */
void MSQDicomQualityControl::calculateAverage(std::string fileName, const QImage& mask, float *output, float factor)
{
  double window = 256, center = 128;
  gdcm::Tag twindowcenter(0x0028, 0x1050);
  gdcm::Tag twindowwidth(0x0028, 0x1051);

  gdcm::ImageReader reader;

  const gdcm::File &file = reader.GetFile();
  const gdcm::Image &gimage = reader.GetImage();
  reader.SetFileName(fileName.c_str());
  //printf("%s\n",fileName.c_str());
  if (!reader.Read()) {
    cout << "Could not read file " << fileName << std::endl;
    //this->warningMessage(QString("Could not read file %1").arg(QString::fromStdString(fileName)), "Pleack check and try again."); 
    // if it falls here, it is not a DICOM image.
    return;
  }

  //const gdcm::Image &image = reader.GetImage();
  const gdcm::DataSet &ds = file.GetDataSet();

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

  unsigned long len = gimage.GetBufferLength();
  std::vector<char> vbuffer;
  vbuffer.resize( len );
  char *buffer = &vbuffer[0];
  gimage.GetBuffer(buffer);

  double intercept = gimage.GetIntercept();
  double slope = gimage.GetSlope();

  const unsigned int* dimension = gimage.GetDimensions();

  //cout << "factor: " << factor << std::endl;

  int c, r, pos, total=0;
  unsigned int dimX = dimension[0];
  unsigned int dimY = dimension[1];
  //double dimXdimY = (dimX / 3) * (dimY / 3);
  double dimXdimY = dimX * dimY;

  QRgb *scan = (QRgb *)mask.scanLine(0);

  // Let's start with the easy case:
  if( gimage.GetPhotometricInterpretation() == gdcm::PhotometricInterpretation::RGB )
    {
      // RGB image
      // undefined
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
              output[i] = output[i] + factor * buffer[i];
            }
          }
      }
    else if ( gimage.GetPixelFormat() == gdcm::PixelFormat::INT16 || gimage.GetPixelFormat() == gdcm::PixelFormat::UINT16 )
      {
        short *input = (short*)buffer;
        //short *start = input + dimX * (dimY / 3);

        for(unsigned int i = 0; i < dimX*dimY; i++)
        //  {
        //for(unsigned int i = 0; i < dimY / 3; i++)
        {
          //short *offset = start + i * dimX + dimX / 3;
          //for(unsigned int j = 0; j < dimX / 3; j++)
          //{
            //pos = equalize(offset[j], window, center);
          r = qRed(scan[i]);
          if (r > 0) {
            //cout << "before: " << output[i];
            output[i] = output[i] + factor * (input[i] * slope + intercept);
            //cout << " after: " << output[i] << std::endl;
          }
          //}
        }

      }
    else
      {
        return;
      }
    }
  else
    {
      return;
    }

}
/***********************************************************************************//**
 * 
 */
void MSQDicomQualityControl::stat_average(float *buffer, const QImage& mask, int dimX, int dimY, double *entropy, double *mean, double *stdev)
{
  //const unsigned int* dimension = gimage.GetDimensions();
  std::map<short, int> hist;

  int c, r, total=0;
  //long hist[256];
  short input;
  //unsigned int dimX = dimension[0];
  //unsigned int dimY = dimension[1];
  //double dimXdimY = (dimX / 3) * (dimY / 3);
  double dimXdimY = dimX * dimY;
  double px, sumlog = 0.0;
  double sum = 0.0;
  double sum2 = 0.0;

  QRgb *scan = (QRgb *)mask.scanLine(0);

  *entropy = 0;
  *mean = 0;
  *stdev = 0;

  //for(c=0;c<256;c++)
  //  {
  //      hist[c] = 0;
  //  }
  
  for(unsigned int i = 0; i < dimX*dimY; i++)
  {
    r = qRed(scan[i]);
    if (r > 0) {

      input = (short)round(buffer[i]);

      //pos = equalize(buffer[i], window, center);
      sum += buffer[i];
      sum2 += buffer[i] * buffer[i];
      //hist[pos]++;

      // freq
      hist[input]++;

      total++;
    }
  }

  //double entr = 0.0;

  if (total > 0) {

    // calculate entropy
    //for(c=0;c<256;c++)
    //{
    //  if (hist[c] > 0) {
    //     px = hist[c] / (double)total;
    //     sumlog -= px * log(px);
         //total += c;
    //  }
    //}

    for ( std::pair<short , int> p : hist ) {
      double px = static_cast<double>( p.second ) / (double)total;
      sumlog -= px * log( px );
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

}

/***********************************************************************************//**
 * 
 */
void MSQDicomQualityControl::statistics(gdcm::Image const & gimage, char *buffer, const QImage& mask, double *entropy, double *mean, double *stdev)
{
  const unsigned int* dimension = gimage.GetDimensions();

  short pos;
  int c, r, total=0;
  //long hist[256];
  std::map<short, int> hist;
  unsigned int dimX = dimension[0];
  unsigned int dimY = dimension[1];
  //double dimXdimY = (dimX / 3) * (dimY / 3);
  double dimXdimY = dimX * dimY;
  double px, sumlog = 0.0;
  double sum = 0.0;
  double sum2 = 0.0;

  double intercept = gimage.GetIntercept();
  double slope = gimage.GetSlope();

  QRgb *scan = (QRgb *)mask.scanLine(0);

  *entropy = 0;
  *mean = 0;
  *stdev = 0;

  //for(c=0;c<256;c++)
  //  {
  //      hist[c] = 0;
  //  }

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
      
        //short *start = input + dimX * (dimY / 3);

        for(unsigned int i = 0; i < dimX*dimY; i++)
        //  {
        //for(unsigned int i = 0; i < dimY / 3; i++)
        {
          short value = input[i] * slope + intercept;
          //short *offset = start + i * dimX + dimX / 3;
          //for(unsigned int j = 0; j < dimX / 3; j++)
          //{
            //pos = equalize(offset[j], window, center);
          r = qRed(scan[i]);
          if (r > 0) {
            //pos = equalize(input[i], window, center);
            sum += value;
            sum2 += value * value;
            hist[value]++;
            total++;
          }
          //}
        }

        //for(unsigned int i = 0; i < dimX*dimY; i++)
        //  {

        //    pos = equalize(input[i], window, center);

            //pos = (int)round((double)input[i] / 256);
        //    sum += pos;
        //    hist[pos]++;
          //}
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

    for ( std::pair<short, int> p : hist ) {
      double px = static_cast<double>( p.second ) / (double)total;
      sumlog -= px * log( px );
    }

    //for(c=0;c<256;c++)
    //{
    //  if (hist[c] > 0) {
    //     px = hist[c] / (double)total;
    //     sumlog -= px * log(px);
         //total += c;
    //  }
    //}

    *entropy = sumlog / M_LN2;
    *mean = sum / total;
    *stdev = sqrt((sum2 / total) - (*mean * *mean));

    //printf("total=%d\n",total);
  } else {
    *entropy = -1;
    *mean = 0;
    *stdev = 0;
  }

}

/***********************************************************************************//**
 * 
 */
void MSQDicomQualityControl::checkQuality()
{
  mProgressDialog->setMinimum(0);
  mProgressDialog->setMaximum(0);
  mProgressDialog->setWindowModality(Qt::WindowModal);
  mProgressDialog->setLabelText("Check quality of DICOM files... Please wait");
  mProgressDialog->show();

  if (mMethodBox->currentIndex() < 2) {

    fileCheckQualityRecursive(mDicomTree, 
      this->mDicomViewer->regionOfInterest(),
      this->mSelectionButton->isChecked(), 
      mQualityFrom->text().toDouble() / 100.0,
      mQualityTo->text().toDouble() / 100.0);

  } else {

    std::vector<std::string> fileNames;
    std::vector<QTreeWidgetItem *> qtItems;
    collectFilenamesRecursive(mDicomTree, this->mSelectionButton->isChecked(), fileNames, qtItems);

    combination cmb;
    cmb.generate(fileNames.size());
    //cmb.print();

    // calculate SNR and entropy for each combination
    fileCheckQualityCombinations(fileNames, qtItems, 
      this->mDicomViewer->regionOfInterest(),
      cmb, mMethodBox->currentIndex());

    cmb.print();
    // display warning
  }

  mProgressDialog->hide();
}

/***********************************************************************************//**
 *
 */
void MSQDicomQualityControl::reset()
{
  //this->mSelectionButton->setChecked(false);
  //this->mWholeButton->setChecked(true);
}

/***********************************************************************************//**
 *
 */
void MSQDicomQualityControl::setVisible(bool visible)
{
  QWidget::setVisible(visible);
  mDicomViewer->showToolBar(visible);
  //if (mLabel) {
  //  if (visible && !this->mWholeButton->isChecked())
  //    mLabel->setCursorEnabled(true);
  //  else
  //    mLabel->setCursorEnabled(false);
  //}
}

/***********************************************************************************//**
 *
 */
MSQDicomQualityControl::~MSQDicomQualityControl()
{
}

