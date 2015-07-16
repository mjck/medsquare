/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryDialog.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQGeometryDialog.h"

#include "MSQGeometryItem.h"

MSQGeometryDialog::MSQGeometryDialog(QWidget *parent, const QString &lbl, const QList<MSQGeometryItem*> geometries) : geometries(geometries)
{
  QVBoxLayout *vbox = new QVBoxLayout(parent);

  QLabel *label = new QLabel(lbl, parent);
  vbox->addWidget(label);
  vbox->addStretch(1);

  this->combo = new QComboBox(parent);
  for (int i = 0; i < geometries.size(); ++i) {
    this->combo->addItem(geometries.at(i)->getName());
  }
  this->combo->setCurrentIndex(0);

  vbox->addWidget(combo);
  vbox->addStretch(1);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
  QPushButton *okButton = static_cast<QPushButton *>(buttonBox->addButton(QDialogButtonBox::Ok));
  okButton->setDefault(true);
  vbox->addWidget(buttonBox);

  setLayout(vbox);

  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

  parent->resize(parent->sizeHint());
}

MSQGeometryDialog::~MSQGeometryDialog()
{
}

MSQGeometryItem *MSQGeometryDialog::getSelectedItem()
{
  return geometries.at(combo->currentIndex());
}

MSQGeometryItem *MSQGeometryDialog::getGeometryItem(QWidget *parent, const QString &title, const QString &label, const QList<MSQGeometryItem*> geometries)
{
  MSQGeometryDialog dialog(parent, label, geometries);
  dialog.setWindowTitle(title);

  if (dialog.exec() == QDialog::Accepted)
  {
    return dialog.getSelectedItem();
  }

  return NULL;
}
