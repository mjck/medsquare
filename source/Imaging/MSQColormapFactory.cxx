/*=========================================================================

 Program:   MedSquare
 Module:    vtkmsqColormapFactory.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQColormapFactory.h"

#include <vector> 
#include <sstream>
#include <fstream>
#include <iostream>

#include "MSQPListParser.h"

/***********************************************************************************//**
 *
 */
MSQColormapFactory::MSQColormapFactory()
{
}

/***********************************************************************************//**
 * create transferFunction
 */
vtkColorTransferFunction *MSQColormapFactory::createTransferFunction(vtkmsqLookupTable *lut, int numColors)
{
  vtkColorTransferFunction *transferFunction=vtkColorTransferFunction::New();
  numColors=lut->GetNumberOfTableValues();

  for (int i=0;i<numColors-1;i++)
  {
    int ipp=i+1;
    double* rgb1;
    double* rgb2;
    rgb1 = lut->GetTableValue(i);
    rgb2 = lut->GetTableValue(ipp);
    transferFunction->AddRGBSegment(i,rgb1[0],rgb1[1],rgb1[2],ipp,rgb2[0],rgb2[1],rgb2[2]);
  }

  return transferFunction;
}


/***********************************************************************************//**
 * Create preset lookuptables
 */
vtkmsqLookupTable *MSQColormapFactory::createColormap(MSQLutType which, int numColors)
{
  vtkmsqLookupTable *lut = vtkmsqLookupTable::New();
  lut->SetNumberOfTableValues(numColors);

  switch (which)
  {
    case MSQ_LUT_GRAY:
      lut->SetSaturationRange(0, 0);
      lut->SetValueRange(0, 1);
      lut->SetRampToLinear();
      lut->Build();
      lut->SetName("Gray");
      break;
    case MSQ_LUT_HUE:
      lut->SetHueRange(0, 1);
      lut->SetSaturationRange(1, 1);
      lut->SetValueRange(1, 1);
      lut->Build();
      lut->SetName("Hue");
      break;
    case MSQ_LUT_SATURATION:
      lut->SetHueRange(0.6, 0.6);
      lut->SetSaturationRange(0, 1);
      lut->SetValueRange(1, 1);
      lut->Build();
      lut->SetName("Saturation");
      break;
    case MSQ_LUT_HOT:
      lut->SetColormapToHot();
      lut->SetName("Hot");
      break;
    case MSQ_LUT_CUSTOM:
      lut->SetName("Custom");
      break;
  }

  return lut;
}

/***********************************************************************************//**
 * Colormap changed by user, must update the current colormap
 */
vtkmsqLookupTable *MSQColormapFactory::loadPListColormap(std::string colormapName)
{     
  QFile file(QString::fromStdString(colormapName));
  int blue_size = 0;
  int green_size = 0;
  int red_size = 0;

  if (file.open(QFile::ReadOnly | QFile::Text)) {
     
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);

    QVariantMap results = MSQPListParser::parsePList(&file).toMap();
    
    //for(QVariantMap::const_iterator iter = results.begin(); iter != results.end(); ++iter) {
    // qDebug() << "key: " << iter.key() << ", value: " << iter.value();
    /// }

    QVariantList blue = results["Blue"].value<QVariantList>();
    blue_size = blue.size();

    QVariantList green = results["Green"].value<QVariantList>();
    green_size = green.size();

    QVariantList red = results["Red"].value<QVariantList>();
    red_size = red.size();

    //QVariantList blue = results["Blue"].value<QVariantList>();
    //for(int i = 0; i < blue.size(); i++) {
    //  qDebug() << "got val: " << blue.at(i).toInt();
    //}

    file.close();

    if (red_size == green_size && red_size == blue_size && red_size > 0) {

      // create colormap
      vtkmsqLookupTable *lut = vtkmsqLookupTable::New();
      lut->SetNumberOfTableValues(red.size());

      for(int i = 0; i < red.size(); i++) {
        
        //cout << red.at(i) << ", " << green.at(i) << ", " << blue.at(i) << std::endl;
        
        lut->SetTableValue(i, 
          red.at(i).toInt() / 255., 
          green.at(i).toInt() / 255., 
          blue.at(i).toInt() / 255., 
          1.0);

      }

      lut->SetName(colormapName);

      return lut;
    
    } else {

      qDebug() << "Colormap does not have equal number of R, G and B items!";

    } 

  }

  return 0;

}


/***********************************************************************************//**
 * Colormap changed by user, must update the current colormap
 */
vtkmsqLookupTable *MSQColormapFactory::loadColormap(std::string colormapName)
{
  int i = 0;
  int numberLines = 0;
  std::string line;
  std::vector<std::string> fields;
  std::istringstream iss;
  ifstream file;

  file.open(colormapName.c_str(), ios::in);

  if (file.fail())
  {
    return NULL;
  }

  std::getline(file, line);

  iss.str(line);
  iss>> std::dec >> numberLines;

  vtkmsqLookupTable *lut = vtkmsqLookupTable::New();
  lut->SetNumberOfTableValues(numberLines);

  while(!file.eof() && i < numberLines)
  {
    double d1, d2, d3;
    std::string str1, str2, str3;

    std::getline(file, line);

    int found1 = line.find("\t");
    int found2 = line.find("\t", found1+1);

    str1 = line.substr(0, found1);
    str2 = line.substr(found1, found2);
    str3 = line.substr(found2);

    d1 = strtod(str1.c_str(), NULL);
    d2 = strtod(str2.c_str(), NULL);
    d3 = strtod(str3.c_str(), NULL);

    lut->SetTableValue(i, d1, d2, d3, 1.0);
    i++;
  }

  file.close();

  lut->SetName(colormapName);
  return lut;
}
