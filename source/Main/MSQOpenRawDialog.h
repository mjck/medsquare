#ifndef MSQ_OPENRAW_DIALOG_H
#define MSQ_OPENRAW_DIALOG_H

#include <QtGui>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QDialogButtonBox>

#include "vtkSmartPointer.h"
#include "vtkmsqRawHeader.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

class MSQOpenRawDialog: public QDialog
{
Q_OBJECT

public:
  MSQOpenRawDialog(const QString &fileName, vtkSmartPointer<vtkmsqRawHeader> header,
      QWidget *parent = 0);
  static vtkSmartPointer<vtkmsqRawHeader> loadHeader(const QString &fileName,
      QWidget *parent = 0);

private slots:
  void import();
  void updateFileSize();

private:
  QString currentFilter;

  QLabel *sizeLabel;
  QLabel *fileLabel, *dimensionLabel, *originLabel;
  QLabel *spacingLabel, *orientationLabel;
  QLabel *dataTypeLabel, *endianLabel;
  QLabel *volumeLabel, *offsetLabel;
  QLineEdit *fileLineEdit, *volumeLineEdit;
  QLineEdit *dimensionXLineEdit, *dimensionYLineEdit, *dimensionZLineEdit;
  QLineEdit *originXLineEdit, *originYLineEdit, *originZLineEdit;
  QLineEdit *spacingXLineEdit, *spacingYLineEdit, *spacingZLineEdit;
  QLineEdit *offsetLineEdit;
  QComboBox *orientationBox, *dataTypeBox, *endianBox;
  QPushButton *importButton, *extensionButton;
  QDialogButtonBox *buttonBox;
  QWidget *extension;
  qint64 fileSize;

  rawOrientation GetOrientation(QString orientation);
  rawDataType GetType(QString type);
  bool IsLittleEndian(QString endian);
  int GetDataTypeSize(QString type);
};

#endif
