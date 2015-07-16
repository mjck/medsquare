#ifndef MSQ_EXPORTSLICE_DIALOG_H
#define MSQ_EXPORTSLICE_DIALOG_H

#include <QtGui/QDialog>
#include <QtGui>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class MedSquare;

class MSQExportSliceDialog: public QDialog
{
Q_OBJECT

public:
  MSQExportSliceDialog(MedSquare *parent = 0);

private slots:
  void exportSlices();  
  void selectExportPath();

private:  
  MedSquare *medsquare;
  QLabel *infoLabel, *pathLabel, *prefLabel, *formatLabel;
  QLineEdit *pathLineEdit, *prefLineEdit;
  QPushButton *exportButton;
  QDialogButtonBox *buttonBox;  
  QComboBox *formatCombo;
};

#endif
