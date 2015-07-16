/*=========================================================================

 Program:   MedSquare
 Module:    MSQFileExportDialog.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_FILEXPORTDIALOG_H
#define MSQ_FILEXPORTDIALOG_H

#include <QFileDialog>
#include <QCheckBox>
#include <QBoxLayout>

class MSQFileExportDialog : public QFileDialog
{
    Q_OBJECT
public:
    MSQFileExportDialog(QWidget *parent = 0);
    //virtual int heightForWidth( int width ) const;
    //virtual QSize sizeHint() const;
signals:

public slots:
    //void setPixmap ( const QPixmap & );
    //void resizeEvent(QResizeEvent *);
private:
	QCheckBox *m_cb;
    //QPixmap pix;
};

#endif // MSQ_FILEXPORTDIALOG_H