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

#include "MSQOpenRawDialog.h"

/***********************************************************************************//**
 * 
 */
MSQOpenRawDialog::MSQOpenRawDialog(const QString &fileName,
    vtkSmartPointer<vtkmsqRawHeader> header, QWidget *parent) :
    QDialog(parent)
{
  fileLabel = new QLabel(tr("File Name: "));
  dimensionLabel = new QLabel(tr("Dimensions: "));
  originLabel = new QLabel(tr("Origin (mm): "));
  spacingLabel = new QLabel(tr("Spacing (mm): "));
  orientationLabel = new QLabel(tr("Orientation: "));
  dataTypeLabel = new QLabel(tr("Data type: "));
  endianLabel = new QLabel(tr("Endianness: "));
  volumeLabel = new QLabel(tr("Volumes: "));
  offsetLabel = new QLabel(tr("Offset: "));

  QIntValidator *intValidator = new QIntValidator;
  intValidator->setBottom(0);
  QDoubleValidator *doubleValidator = new QDoubleValidator(this);
  doubleValidator->setBottom(0);
  doubleValidator->setNotation(QDoubleValidator::StandardNotation);

  fileLineEdit = new QLineEdit(fileName);
  fileLineEdit->setReadOnly(true);
  dimensionXLineEdit = new QLineEdit(tr("%1").arg(header->GetDimensions()[0]));
  dimensionYLineEdit = new QLineEdit(tr("%1").arg(header->GetDimensions()[1]));
  dimensionZLineEdit = new QLineEdit(tr("%1").arg(header->GetDimensions()[2]));
  originXLineEdit = new QLineEdit(tr("%1").arg(header->GetOrigin()[0]));
  originYLineEdit = new QLineEdit(tr("%1").arg(header->GetOrigin()[1]));
  originZLineEdit = new QLineEdit(tr("%1").arg(header->GetOrigin()[2]));
  spacingXLineEdit = new QLineEdit(tr("%1").arg(header->GetSpacing()[0]));
  spacingYLineEdit = new QLineEdit(tr("%1").arg(header->GetSpacing()[1]));
  spacingZLineEdit = new QLineEdit(tr("%1").arg(header->GetSpacing()[2]));
  volumeLineEdit = new QLineEdit(tr("%1").arg(header->GetVolume()));
  offsetLineEdit = new QLineEdit(tr("%1").arg(header->GetOffset()));

  dimensionXLineEdit->setValidator(intValidator);
  dimensionYLineEdit->setValidator(intValidator);
  dimensionZLineEdit->setValidator(intValidator);
  volumeLineEdit->setValidator(intValidator);
  originXLineEdit->setValidator(doubleValidator);
  originYLineEdit->setValidator(doubleValidator);
  originZLineEdit->setValidator(doubleValidator);
  spacingXLineEdit->setValidator(doubleValidator);
  spacingYLineEdit->setValidator(doubleValidator);
  spacingZLineEdit->setValidator(doubleValidator);

  connect(dimensionXLineEdit, SIGNAL(textChanged(const QString &)), this,
      SLOT(updateFileSize()));
  connect(dimensionYLineEdit, SIGNAL(textChanged(const QString &)), this,
      SLOT(updateFileSize()));
  connect(dimensionZLineEdit, SIGNAL(textChanged(const QString &)), this,
      SLOT(updateFileSize()));
  connect(volumeLineEdit, SIGNAL(textChanged(const QString &)), this,
      SLOT(updateFileSize()));
  connect(offsetLineEdit, SIGNAL(textChanged(const QString &)), this,
      SLOT(updateFileSize()));

  orientationBox = new QComboBox();
  orientationBox->addItem(tr("Axial"));
  orientationBox->addItem(tr("Coronal"));
  orientationBox->addItem(tr("Sagittal"));

  dataTypeBox = new QComboBox();
  dataTypeBox->addItem(tr("Char"));
  dataTypeBox->addItem(tr("Unsigned char"));
  dataTypeBox->addItem(tr("Short"));
  dataTypeBox->addItem(tr("Unsigned short"));
  dataTypeBox->addItem(tr("Int"));
  dataTypeBox->addItem(tr("Unsigned int"));
  dataTypeBox->addItem(tr("Float"));
  dataTypeBox->addItem(tr("Double"));

  connect(dataTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFileSize()));

  endianBox = new QComboBox();
  endianBox->addItem(tr("Little endian"));
  endianBox->addItem(tr("Big endian"));

  importButton = new QPushButton(tr("Import"));
  extensionButton = new QPushButton(tr("More"));
  extensionButton->setCheckable(true);
  extensionButton->setAutoDefault(false);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);
  buttonBox->addButton(importButton, QDialogButtonBox::AcceptRole);
  buttonBox->addButton(extensionButton, QDialogButtonBox::ActionRole);

  QGridLayout *extensionLayout = new QGridLayout;
  extensionLayout->setColumnMinimumWidth(0, 100);
  extensionLayout->setMargin(0);
  extensionLayout->addWidget(originLabel, 0, 0, Qt::AlignRight);
  extensionLayout->addWidget(originXLineEdit, 0, 1);
  extensionLayout->addWidget(originYLineEdit, 0, 2);
  extensionLayout->addWidget(originZLineEdit, 0, 3);
  extensionLayout->addWidget(endianLabel, 1, 0, Qt::AlignRight);
  extensionLayout->addWidget(endianBox, 1, 1);
  extensionLayout->addWidget(volumeLabel, 2, 0, Qt::AlignRight);
  extensionLayout->addWidget(volumeLineEdit, 2, 1);
  extensionLayout->addWidget(offsetLabel, 1, 2, Qt::AlignRight);
  extensionLayout->addWidget(offsetLineEdit, 1, 3);

  extension = new QWidget;
  extension->setLayout(extensionLayout);
  extension->hide();

  QFont font11;
  font11.setPointSize(11);
  QLabel *infoLabel =
      new QLabel(
          tr(
              "Please fill out below the information about the dataset, then select Import.\n"));
  infoLabel->setFont(font11);

  QFrame* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  // Determine the file size
  QFile file(fileName);
  fileSize = file.size();

  sizeLabel = new QLabel();
  sizeLabel->setFont(font11);
  updateFileSize();

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->setColumnMinimumWidth(0, 100);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(infoLabel, 0, 0, 1, 4);
  mainLayout->addWidget(fileLabel, 1, 0, Qt::AlignRight);
  mainLayout->addWidget(fileLineEdit, 1, 1, 1, 3);
  mainLayout->addWidget(dimensionLabel, 2, 0, Qt::AlignRight);
  mainLayout->addWidget(dimensionXLineEdit, 2, 1);
  mainLayout->addWidget(dimensionYLineEdit, 2, 2);
  mainLayout->addWidget(dimensionZLineEdit, 2, 3);
  mainLayout->addWidget(spacingLabel, 3, 0, Qt::AlignRight);
  mainLayout->addWidget(spacingXLineEdit, 3, 1);
  mainLayout->addWidget(spacingYLineEdit, 3, 2);
  mainLayout->addWidget(spacingZLineEdit, 3, 3);
  mainLayout->addWidget(dataTypeLabel, 4, 0, Qt::AlignRight);
  mainLayout->addWidget(dataTypeBox, 4, 1);
  mainLayout->addWidget(orientationLabel, 4, 2, Qt::AlignRight);
  mainLayout->addWidget(orientationBox, 4, 3);
  mainLayout->addWidget(line, 5, 0, 1, 4);
  mainLayout->addWidget(extension, 6, 0, 1, 4);
  mainLayout->addWidget(sizeLabel, 7, 0, 1, 4);
  mainLayout->addWidget(buttonBox, 8, 0, 1, 5, Qt::AlignBottom);
  setLayout(mainLayout);

  connect(importButton, SIGNAL(clicked()), this, SLOT(import()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(extensionButton, SIGNAL(toggled(bool)), extension, SLOT(setVisible(bool)));

  setWindowTitle("Import Raw Image");
}

/***********************************************************************************//**
 * 
 */
int MSQOpenRawDialog::GetDataTypeSize(QString type)
{
  if (type.compare(tr("Float")) == 0)
    return sizeof(float);
  if (type.compare(tr("Double")) == 0)
    return sizeof(double);
  if (type.compare(tr("Int")) == 0)
    return sizeof(int);
  if (type.compare(tr("Unsigned int")) == 0)
    return sizeof(unsigned int);
  if (type.compare(tr("Short")) == 0)
    return sizeof(short);
  if (type.compare(tr("Unsigned short")) == 0)
    return sizeof(unsigned short);
  if (type.compare(tr("Char")) == 0)
    return sizeof(char);
  if (type.compare(tr("Unsigned char")) == 0)
    return sizeof(unsigned char);
  return sizeof(signed char);
}

/***********************************************************************************//**
 * 
 */
void MSQOpenRawDialog::updateFileSize()
{
  int dimX = dimensionXLineEdit->text().toInt();
  int dimY = dimensionYLineEdit->text().toInt();
  int dimZ = dimensionZLineEdit->text().toInt();
  int vols = volumeLineEdit->text().toInt();
  int size = GetDataTypeSize(dataTypeBox->currentText());
  int offset = offsetLineEdit->text().toInt();
  int userSize = dimX * dimY * dimZ * vols * size + offset;
  sizeLabel->setText(
      tr("Estimated file size: %1 bytes (%2 bytes)").arg(userSize).arg(fileSize));

  if (userSize <= fileSize)
    importButton->setEnabled(true);
  else
    importButton->setEnabled(false);
}

/***********************************************************************************//**
 * 
 */
void MSQOpenRawDialog::import()
{
  this->accept();
}

/***********************************************************************************//**
 * 
 */
vtkSmartPointer<vtkmsqRawHeader> MSQOpenRawDialog::loadHeader(const QString &fileName,
    QWidget *parent)
{
  vtkSmartPointer<vtkmsqRawHeader> header = vtkSmartPointer<vtkmsqRawHeader>::New();

  MSQOpenRawDialog dialog(fileName, header, parent);

  if (dialog.exec() == QDialog::Accepted)
  {
    header->SetDimensions(dialog.dimensionXLineEdit->text().toInt(),
        dialog.dimensionYLineEdit->text().toInt(),
        dialog.dimensionZLineEdit->text().toInt());
    header->SetOrigin(dialog.originXLineEdit->text().toDouble(),
        dialog.originYLineEdit->text().toDouble(),
        dialog.originZLineEdit->text().toDouble());
    header->SetSpacing(dialog.spacingXLineEdit->text().toDouble(),
        dialog.spacingYLineEdit->text().toDouble(),
        dialog.spacingZLineEdit->text().toDouble());
    header->SetOrientation(dialog.GetOrientation(dialog.orientationBox->currentText()));
    header->SetType(dialog.GetType(dialog.dataTypeBox->currentText()));
    header->SetLittleEndian(dialog.IsLittleEndian(dialog.endianBox->currentText()));
    header->SetVolume(dialog.volumeLineEdit->text().toInt());
    header->SetOffset(dialog.offsetLineEdit->text().toInt());

    return header;
  }

  return NULL;
}

/***********************************************************************************//**
 * 
 */
rawOrientation MSQOpenRawDialog::GetOrientation(QString orientation)
{
  if (orientation.compare(tr("Axial")) == 0)
    return RAWAXIAL;
  if (orientation.compare(tr("Coronal")) == 0)
    return RAWCORONAL;
  return RAWSAGITTAL;
}

/***********************************************************************************//**
 * 
 */
rawDataType MSQOpenRawDialog::GetType(QString type)
{
  if (type.compare(tr("Float")) == 0)
    return FLOAT;
  if (type.compare(tr("Double")) == 0)
    return DOUBLE;
  if (type.compare(tr("Int")) == 0)
    return INT;
  if (type.compare(tr("Unsigned int")) == 0)
    return UNSIGNEDINT;
  if (type.compare(tr("Short")) == 0)
    return SHORT;
  if (type.compare(tr("Unsigned short")) == 0)
    return UNSIGNEDSHORT;
  if (type.compare(tr("Char")) == 0)
    return CHAR;
  if (type.compare(tr("Unsigned char")) == 0)
    return UNSIGNEDCHAR;
  return SIGNEDCHAR;
}

/***********************************************************************************//**
 * 
 */
bool MSQOpenRawDialog::IsLittleEndian(QString endian)
{
  return endian.compare(tr("Little endian")) == 0;
}
