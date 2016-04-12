/*=========================================================================

 Program:   MedSquare
 Module:    MSQAspectRatioPixmapLabel.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_ASPECTRATIOPIXMAPLABEL_H
#define MSQ_ASPECTRATIOPIXMAPLABEL_H

#include <QDir>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QResizeEvent>
#include <QFileDialog>

class MSQAspectRatioPixmapLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MSQAspectRatioPixmapLabel(QWidget *parent = 0);
    virtual int heightForWidth( int width ) const;
    virtual QSize sizeHint() const;

    void setHighQuality(bool quality);
    void setPenSize(int size);
    void setCursorEnabled(bool state);
    void setCursorFilled(bool state);
    bool isCursorEnabled();
    bool isCursorFilled();
    void setCursorToRect();
    void setCursorToEllipse();
    void setCursorToArc();
    void setCursorToThreshold();
    void setThresholdPercentage(int perc);
    int getThresholdPercentage();
    int getCursorType();
    QRect getRect();
    QImage regionOfInterest();
    QImage rectangularRegionOfInterest();
    void saveSettings();
    void loadSettings();

private:
    void recalculateRect();
    void drawOverlay ( QPainter & p );
    void threshold( QPainter & p );

signals:
    void changed();

public slots:
    void setPixmap ( const QPixmap &, std::vector<short> & im);
    void resizeEvent(QResizeEvent *);

private slots:
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void wheelEvent(QWheelEvent *e);
    void paintEvent(QPaintEvent *);

private:
    QString currentFileName;
    QString currentPath;

    QPixmap pix;
    QPixmap overlay;
    std::vector<short> image;

    bool tracking;
    bool panning;

    int arcAngle;
    int arcLength;
    int perc;
    
    int s0, s1, t0, t1;
    
    QRectF viewport;
    QRectF screen;
    QRectF normalized;

    bool highQuality;
    bool cursorEnabled;
    bool cursorFilled;
    int cursorType;
    int penSize;
};

#endif // MSQ_ASPECTRATIOPIXMAPLABEL_H