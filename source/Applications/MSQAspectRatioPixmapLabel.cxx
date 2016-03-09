/*=========================================================================

 Program:   MedSquare
 Module:    MSQAspectRatioPixmapLabel.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQAspectRatioPixmapLabel.h"
#include <QStyle>
//
#include <QDebug>

MSQAspectRatioPixmapLabel::MSQAspectRatioPixmapLabel(QWidget *parent) :
    QLabel(parent)
{
    this->setMinimumSize(1,1);

    this->viewport.setCoords(0, 0, 1, 1);
    this->screen.setCoords(0, 0, 1, 1);
    this->normalized.setRect(0.33, 0.33, 0.33, 0.33);

    this->tracking = false;
    this->panning = false;

    this->cursorEnabled = false;
    this->cursorType = 0;
    this->cursorFilled = true;

    this->arcAngle = 0;
    this->arcLength = 180;
    this->penSize = 1;

    this->highQuality = true;

    //this->overlay = 0;
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setHighQuality(bool quality)
{
    this->highQuality = quality;
    update();
    //emit changed();
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setPenSize(int size)
{
    this->penSize = size;
    update();
    emit changed();
}

/***********************************************************************************//**
 * 
 */
bool MSQAspectRatioPixmapLabel::isCursorEnabled()
{
    return this->cursorEnabled;
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setCursorEnabled(bool state)
{
    this->cursorEnabled = state;
    update();
    emit changed();
}

/***********************************************************************************//**
 * 
 */
bool MSQAspectRatioPixmapLabel::isCursorFilled()
{
    return this->cursorFilled;
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setCursorFilled(bool state)
{
    this->cursorFilled = state;
    update();
    emit changed();
}

/***********************************************************************************//**
 * 
 */
int MSQAspectRatioPixmapLabel::getCursorType()
{
    return this->cursorType;
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setCursorToRect()
{
    this->cursorType = 0;
    update();
    emit changed();
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setCursorToEllipse()
{
    this->cursorType = 1;
    update();
    emit changed();
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setCursorToArc()
{
    this->cursorType = 2;
    update();
    emit changed();
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::paintEvent(QPaintEvent * e)
{
    QLabel::paintEvent(e);
    //QPainter painter(this);

    //QStyle *style = QWidget::style();
    //int align = QStyle::visualAlignment(layoutDirection(), QFlag(Qt::AlignCenter));

    // draw background
    //painter.setOpacity(this->backgroundOpacity);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    //this->paintBackground(painter, style, align);
    //painter.fillRect(contentsRect(), Qt::transparent);

    //painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    //painter.fillRect(contentsRect(), QColor(0, 0, 0, 120));
    //if (foreground.width() > 0 && foreground.height() > 0) {
       //foreground.setOpacity(this->foregroundOpacity);
    //   this->paintForeground(painter, style, align);
    //}
    //painter.setCompositionMode(mode);
    //painter.drawImage(0, 0, sourceImage);
    //painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    //painter.fillRect(resultImage.rect(), Qt::white);
    //painter.end();

    //this->paintBackground(painter, style, align);

    // draw foreground
     //if (foreground.width() > 0 && foreground.height() > 0) {
     //   painter.setOpacity(1.0 - this->backgroundOpacity);
     //   this->paintForeground(painter, style, align);
   // }

    if (this->cursorEnabled) {

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); 
        //painter.setRenderHint(QPainter::HighQualityAntialiasing);

        if (pix.width() > 0 && pix.height() > 0 && screen.width() > 0 && screen.height() > 0) {

            QPen paintpen(this->highQuality ? Qt::green : Qt::red);
            QBrush paintbrush(this->highQuality ? QColor(0, 255, 0, 60) : QColor(255, 0, 0, 60), Qt::DiagCrossPattern);
            paintpen.setWidth(this->penSize);
            paintpen.setCapStyle(Qt::FlatCap);
            paintpen.setJoinStyle(Qt::MiterJoin);
            painter.setPen(paintpen);

            if (this->cursorFilled) {
                painter.setBrush(paintbrush);
            }
            
            if (!tracking && !panning) {

                screen.setRect(
                    viewport.left(), 
                    viewport.top(), 
                    normalized.width() * viewport.width(),
                    normalized.height() * viewport.height()
                );

            //printf("screen1: %f %f %f %f\n\n",screen.left(),screen.top(),screen.width(),screen.height());

                screen.translate(
                    normalized.left() * viewport.width(),
                    normalized.top() * viewport.height()
                );

            }

            //printf("viewport: %f %f %f %f\n",viewport.left(),viewport.top(),viewport.width(),viewport.height());
            //printf("normalized: %f %f %f %f\n",normalized.left(),normalized.top(),normalized.width(),normalized.height());               
            //printf("screen2: %f %f %f %f\n\n",screen.left(),screen.top(),screen.width(),screen.height());

            //painter.drawEllipse(viewport);
            if (this->highQuality || tracking || panning) {

                switch(this->cursorType)
                {
                    case 0: 
                        painter.drawRect(screen);
                    break;
                    case 1:
                        if ( this->cursorFilled )
                            painter.drawEllipse(screen);
                        else
                            painter.drawArc(screen, 0, 359 * 17);
                    break;
                    case 2:
                        if (this->arcLength < 360)
                            painter.drawArc(screen, this->arcAngle * 16, this->arcLength * 16);
                        else
                            painter.drawArc(screen, 0, 359 * 16);
                    break;
                }

            } else {
                
                drawOverlay( painter );
            }
        }
    }
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::drawOverlay( QPainter & p )
{
    QStyle *style = QWidget::style();
    int align = QStyle::visualAlignment(layoutDirection(), QFlag(Qt::AlignCenter));

    QPainter painter( &overlay );

    QPen paintpen(QColor(255, 0, 0, 60));

    paintpen.setWidth(this->penSize);
    paintpen.setCapStyle(Qt::FlatCap);
    paintpen.setJoinStyle(Qt::MiterJoin);

    painter.setPen( paintpen );

    if ( this->cursorFilled ) {
        painter.setBrush ( QColor(255, 0, 0, 60) );
        painter.setPen ( Qt::NoPen );
    }
    
    QRectF roi(
        pix.width() * normalized.left(),
        pix.height() * normalized.top(),
        pix.width() * normalized.width(),
        pix.height() * normalized.height() 
    ); 

    if ( this->cursorEnabled )
    {
        overlay.fill( Qt::transparent );
    
        switch(this->cursorType)
        {
            case 0:
                painter.drawRect(roi);
            break;
            case 1:
                if ( this->cursorFilled )
                    painter.drawEllipse(roi);
                else
                    painter.drawArc(roi, 0, 359 * 16);
            break;
            case 2:
                if (this->arcLength < 360)
                    painter.drawArc(roi, this->arcAngle * 16, this->arcLength * 16);
                else
                    painter.drawArc(roi, 0, 359 * 16);
            break;
        };
    
    } 

    style->drawItemPixmap(&p, contentsRect(), align, 
        overlay.scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::mousePressEvent(QMouseEvent * e)
{
    if (this->cursorEnabled) {
        if (e->button() == Qt::LeftButton)
        {
            tracking = true;
            this->screen.setCoords(e->x(), e->y(), e->x(), e->y());
        } else if (e->button() == Qt::RightButton) {
            s0 = s1 = e->x();
            t0 = t1 = e->y();
            panning = true;
        } 
    }
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::mouseMoveEvent(QMouseEvent * e)
{
    if (this->cursorEnabled) {
        if (this->tracking)
        {
            this->screen.setRight(e->x());
            this->screen.setBottom(e->y());
            if (this->screen.width() && this->screen.height())
                update();
        } else if (this->panning) {
            s1 = e->x();
            t1 = e->y();
            this->screen.translate(s1 - s0, t1 - t0);
            s0 = e->x();
            t0 = e->y();
            update();
        }
        //update();
        //emit changed();
    }
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::mouseReleaseEvent(QMouseEvent * e)
{
    if (this->cursorEnabled) {

        if (e->button() == Qt::LeftButton)
        {
            tracking = false;
            this->normalized.setRect(
                (qreal)(this->screen.left() - this->viewport.left()) / this->viewport.width(),
                (qreal)(this->screen.top() - this->viewport.top()) / this->viewport.height(),
                (qreal)this->screen.width() / this->viewport.width(),
                (qreal)this->screen.height() / this->viewport.height()
            );

        } else if (e->button() == Qt::RightButton) {
            panning = false;
            this->normalized.setRect(
                (qreal)(this->screen.left() - this->viewport.left()) / this->viewport.width(),
                (qreal)(this->screen.top() - this->viewport.top()) / this->viewport.height(),
                (qreal)this->screen.width() / this->viewport.width(),
                (qreal)this->screen.height() / this->viewport.height()
            );
        }
        
        update();
        emit changed();
    }
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    // rotate any shape
    if (this->cursorType == 2) {

        // draw arc
        if (event->modifiers() == Qt::ShiftModifier) {
            
            arcLength += numSteps * 5;
            
            if (arcLength > 360)
                arcLength = 360;
            else if (arcLength < 5)
                arcLength = 5;

        } else {

            if (arcLength < 360) {
                arcAngle += numSteps * 5;

                if (arcAngle > 360)
                    arcAngle -= 360;
                else if (arcAngle < 0)
                    arcAngle += 360;
            }
        
        }
    }

    update();
    emit changed();

    event->accept();
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::recalculateRect()
{
    qreal x, y, w, h;
    qreal ratio = (qreal)this->width() / this->height();
    qreal pixratio = (qreal)pix.width() / pix.height();

    if (ratio < pixratio)
    {
        x = 0;
        w = pix.width() * ((qreal)this->width() / pix.width()) - 1;
        h = pix.height() * ((qreal)this->width() / pix.width()) - 1;
        y = (this->height() - h) / 2;

    } else {

        w = pix.width() * ((qreal)this->height() / pix.height()) - 1;
        x = (this->width() - w) / 2;
        y = 0;
        h = this->height() - 1;
    }

    viewport.setRect(x, y, w, h);
}

/***********************************************************************************//**
 * 
 */
QImage MSQAspectRatioPixmapLabel::regionOfInterest() const
{
    if ( pix.isNull() )
        return QImage();

    QImage pixroi ( pix.width(), pix.height(), QImage::Format_RGB32 );
    QPainter painter( &pixroi );

    QPen paintpen( Qt::white );
    paintpen.setWidth(this->penSize);
    painter.setPen( paintpen );
    paintpen.setCapStyle(Qt::FlatCap);
    paintpen.setJoinStyle(Qt::MiterJoin);

    if ( this->cursorFilled ) {
        painter.setBrush ( QBrush ( Qt::white ) );
        painter.setPen ( Qt::NoPen );
    }
    
    QRectF roi(
        pix.width() * normalized.left(),
        pix.height() * normalized.top(),
        pix.width() * normalized.width(),
        pix.height() * normalized.height() 
    ); 

    if ( this->cursorEnabled )
    {
        pixroi.fill( Qt::black );

        switch(this->cursorType)
        {
            case 0:
                painter.drawRect(roi);
            break;
            case 1:
                if ( this->cursorFilled )
                    painter.drawEllipse(roi);
                else
                    painter.drawArc(roi, 0, 359.9 * 16);
            break;
            case 2:
                if (this->arcLength < 360)
                    painter.drawArc(roi, this->arcAngle * 16, this->arcLength * 16);
                else
                    painter.drawArc(roi, 0, 359.9 * 16);
            break;
        };
    
    } else {

        pixroi.fill( Qt::white );
    }

    printf("saving\n");
    pixroi.save( "region_of_interest.png" );

    //return pixroi.toImage();
    return pixroi.copy();
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::setPixmap ( const QPixmap & p )
{
    pix = p;
    overlay = p;

    QLabel::setPixmap(pix.scaled(this->size(),
        Qt::KeepAspectRatio, Qt::SmoothTransformation));

    recalculateRect();
}

/***********************************************************************************//**
 * 
 */
int MSQAspectRatioPixmapLabel::heightForWidth( int width ) const
{
    return ((qreal)pix.height()*width)/pix.width();
}

/***********************************************************************************//**
 * 
 */
QSize MSQAspectRatioPixmapLabel::sizeHint() const
{
    int w = this->width();
    return QSize( w, heightForWidth(w) );
}

/***********************************************************************************//**
 * 
 */
void MSQAspectRatioPixmapLabel::resizeEvent(QResizeEvent * e)
{
    if ( !pix.isNull() )
    {
        QLabel::setPixmap(pix.scaled(this->size(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    recalculateRect();
}