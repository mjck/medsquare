/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomImageViewerButton.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOMIMAGEVIEWERBUTTON_H
#define MSQ_DICOMIMAGEVIEWERBUTTON_H

#include <QAbstractButton>
#include <QColor>
#include <QResizeEvent>

class MSQDicomImageViewerButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit MSQDicomImageViewerButton(QWidget *parent = 0);
    virtual QSize sizeHint() const;
    void setColor(const QColor & color);

private:
    //void recalculateRect();

signals:
    //void changed();

public slots:

    void setFilled ( bool state );
    void setType ( int type );

private slots:

    void paintEvent(QPaintEvent *);
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);

private:
    bool filled;
    int type;
    bool entering;
    QColor color;
};

#endif // MSQ_DICOMIMAGEVIEWERBUTTON_H