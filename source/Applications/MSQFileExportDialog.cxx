/*=========================================================================

 Program:   MedSquare
 Module:    MSQFileExportDialog.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQFileExportDialog.h"

//#include <QDebug>

MSQFileExportDialog::MSQFileExportDialog(QWidget *parent) :
    QFileDialog(parent)
{
    this->setOption(QFileDialog::DontUseNativeDialog, true);
 QGridLayout* mainLayout = dynamic_cast<QGridLayout*>(layout());

 if ( ! mainLayout ) {

    printf("here\n");

  return;

 } else {
  
  printf("out here\n");

  QHBoxLayout *hbl = new QHBoxLayout;

  // add some widgets
  m_cb = new QCheckBox("My Checkbox");
  hbl->addWidget(m_cb);

  int numRows = mainLayout->rowCount();

  // add the new layout to the bottom of mainLayout
  // and span all columns
  mainLayout->addLayout( hbl, numRows,0,1,-1);
    }

}
