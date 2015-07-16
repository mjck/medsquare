/*=========================================================================

 Program:   MedSquare
 Module:    MSQGeometryDialog.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_GEOMETRY_DIALOG_H
#define MSQ_GEOMETRY_DIALOG_H

#include <QtGui>

QT_MODULE(Gui)

class MSQGeometryItem;
class MSQGeometryDialogPrivate;

class Q_GUI_EXPORT MSQGeometryDialog : public QDialog
{
Q_OBJECT

public:
  MSQGeometryDialog(QWidget *parent, const QString &lbl, const QList<MSQGeometryItem*> geometries);
  virtual ~MSQGeometryDialog();

  MSQGeometryItem *getSelectedItem();

  static MSQGeometryItem *getGeometryItem(QWidget *parent, const QString &title, const QString &label, const QList<MSQGeometryItem*> geometries);

private:
  QComboBox *combo;
  QList<MSQGeometryItem*> geometries;

};

#endif
