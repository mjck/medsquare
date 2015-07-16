/*=========================================================================

 Program:   MedSquare
 Module:    MSQOpenRawDialog.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQExportSliceDialog.h"
#include "vtkmsqImageItem.h"
#include "MSQSliceExporter.h"
#include "vtkSmartPointer.h"
#include "vtkPNGWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkBMPWriter.h"
#include "MedSquare.h"

/***********************************************************************************//**
 * 
 */
MSQExportSliceDialog::MSQExportSliceDialog(MedSquare *parent) :
    QDialog(parent)
{
  
  this->medsquare = parent;
  pathLabel = new QLabel(tr("Path: "));
  prefLabel=new QLabel(tr("Prefix:  "));
  
  pathLineEdit = new QLineEdit();
  prefLineEdit=new QLineEdit("export_");
  
  formatLabel = new QLabel(tr("Format:  "));
  formatCombo=new QComboBox();
  formatCombo->addItem(tr("PNG"));
  formatCombo->addItem(tr("JPEG"));
  formatCombo->addItem(tr("TIFF"));
  formatCombo->addItem(tr("BMP"));
  
  
  /*orientationBox = new QComboBox();
  orientationBox->addItem(tr("Axial"));
  orientationBox->addItem(tr("Coronal"));
  orientationBox->addItem(tr("Sagittal"));*/
  
  QPushButton *pathButton=new QPushButton(tr("..."));  
  pathButton->setMaximumWidth(100);
    
  exportButton = new QPushButton(tr("Export"));
  buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);
  buttonBox->addButton(exportButton, QDialogButtonBox::AcceptRole);  
  

  QFont font11;
  font11.setPointSize(11);
  QLabel *infoLabel =
      new QLabel(
          tr(
              "Insert the directory path to export the slices.\n"));
  infoLabel->setFont(font11);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setColumnMinimumWidth(0, 100);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(infoLabel, 0, 0, 1, 4);
  mainLayout->addWidget(pathLabel, 1, 0, Qt::AlignRight);
  mainLayout->addWidget(pathLineEdit, 1, 1, 1, 3);
  mainLayout->addWidget(pathButton,1,4,1,1);
  mainLayout->addWidget(prefLabel, 2,0, Qt::AlignRight);
  mainLayout->addWidget(prefLineEdit,2,1,Qt::AlignRight);
  mainLayout->addWidget(formatLabel,3,0,Qt::AlignRight);
  mainLayout->addWidget(formatCombo,3,1,1,3);
  mainLayout->addWidget(buttonBox, 4, 0, 1, 5, Qt::AlignBottom);
  setLayout(mainLayout);

  connect(exportButton, SIGNAL(clicked()), this, SLOT(exportSlices()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));  
  connect(pathButton,SIGNAL(clicked()),this,SLOT(selectExportPath()));
  
  setWindowTitle("Export slices");
}


/***********************************************************************************//**
 * 
 */
void MSQExportSliceDialog::exportSlices()
{
  this->accept();
  
  vtkSmartPointer<vtkmsqImageItem> currentImageItem = this->medsquare->getCurrentImage();
  MSQSliceExporter *exporter = new MSQSliceExporter(this->medsquare);
  exporter->exportSlices(currentImageItem,this->pathLineEdit->text(),this->prefLineEdit->text(),this->formatCombo->currentText());
  delete exporter;  
}

/***********************************************************************************//**
 * 
 */
void MSQExportSliceDialog::selectExportPath()
{  
  this->pathLineEdit->setText(QFileDialog::getExistingDirectory(this,"Select destination directory"));    
}


