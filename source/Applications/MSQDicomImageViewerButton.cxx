/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomImageViewerButton.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomImageViewerButton.h"

#include <QPainter>
#include <QPainterPath>
//
//#include <QDebug>

MSQDicomImageViewerButton::MSQDicomImageViewerButton(QWidget *parent) :
    QAbstractButton(parent)
{
    this->setMinimumSize(1,1);
    this->entering = false;
    this->type = 0;
    this->filled = false;
}

/***********************************************************************************//**
 * 
 */
void MSQDicomImageViewerButton::setColor(const QColor & color)
{
    this->color = color;
    update();
}

/***********************************************************************************//**
 * 
 */
void MSQDicomImageViewerButton::paintEvent(QPaintEvent * e)
{
    QPainter painter(this);
    
    if (this->type > 0 && this->type < 3) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
    }

    qreal x = this->width() / 7.0;
    qreal y = this->height() / 4.0;
    QRectF screen(x, y, x * 5.0, y * 2.0);

    if (this->isChecked()) {
        
        QPen paintpen(this->color);
        paintpen.setWidth(1);
        painter.setPen(paintpen);
        
        QBrush paintbrush(QColor(this->color.red(),this->color.green(), this->color.blue(), 80), Qt::DiagCrossPattern);
        
        if (this->filled) 
            painter.setBrush(paintbrush);
        
        switch(this->type)
        {
            case 0: painter.drawRect(screen); break;
            case 1: painter.drawEllipse(screen); break;
            case 2: painter.drawArc(screen, 0, 180 * 16); break;
            case 3:
                {
                    painter.drawRect(screen);
                    painter.drawRect(screen.left()+2,screen.top()+2,screen.width()-3,screen.height()-4);
                    QPainterPath path;
                    path.moveTo(x + 2, this->height() - y - 2);
                    path.cubicTo(
                        screen.width() / 2, this->height() - y - 5, 
                        screen.width() / 2, screen.height() / 2, 
                        this->width() - x - 2, this->height() - y - 2);
                    QBrush brush(this->color, Qt::Dense4Pattern);
                    painter.setBrush(brush);
                    painter.drawPath(path);
                    painter.drawEllipse(this->width() - x - 10, y + 4, 5, 5);
                }
                break;
            case 4:
                {
                    QRectF left(x, y, x * 2.5, y * 2.0);
                    QRectF right(x + x * 2.5, y, x * 2.5, y * 2.0);
                    painter.drawLine(x, y + y, x + x * 5.0, y + y);
                    painter.drawArc(right, 180 * 16, 180 * 16);
                    painter.setBrush(paintbrush);
                    painter.drawChord(left, 0, 180 * 16);
                }
                break;
            default:
                break;
        }
        
    } else {

        QPen paintpen(this->entering && this->isEnabled() ? this->color : Qt::gray );
        paintpen.setWidth(1);
        painter.setPen(paintpen);

        QBrush paintbrush(this->entering && this->isEnabled() ? 
            QColor(this->color.red(),this->color.green(), this->color.blue(), 80) : QColor(255, 255, 255, 80), Qt::DiagCrossPattern);

        if (this->filled)
            painter.setBrush(paintbrush);

        switch(this->type)
        {
            case 0: painter.drawRect(screen); break;
            case 1: painter.drawEllipse(screen); break;
            case 2: painter.drawArc(screen, 0, 180 * 16); break;
            case 3:
                {
                     painter.drawRect(screen);
                     painter.drawRect(screen.left()+2,screen.top()+2,screen.width()-3,screen.height()-4);
                     QPainterPath path;
                     path.moveTo(x + 2, this->height() - y - 2);
                     path.cubicTo(
                        screen.width() / 2, this->height() - y - 5, 
                        screen.width() / 2, screen.height() / 2, 
                        this->width() - x - 2, this->height() - y - 2);
                     QBrush brush(this->entering ? this->color : QColor(255, 255, 255, 80), Qt::Dense4Pattern);
                     painter.setBrush(brush);
                     painter.drawPath(path);
                     painter.drawEllipse(this->width() - x - 10, y + 4, 5, 5);
                }
                break;
            case 4:
                {
                    QRectF left(x, y, x * 2.5, y * 2.0);
                    QRectF right(x + x * 2.5, y, x * 2.5, y * 2.0);
                    painter.drawLine(x, y + y, x + x * 5.0, y + y);
                    painter.drawArc(right, 180 * 16, 180 * 16);
                    painter.setBrush(paintbrush);
                    painter.drawChord(left, 0, 180 * 16);
                }
                break;
            default:
                break;
        }
        
    }
             
}

/***********************************************************************************//**
 * 
 */
void MSQDicomImageViewerButton::setFilled( bool state )
{
    this->filled = state;
    update();
}

/***********************************************************************************//**
 * 
 */
void MSQDicomImageViewerButton::setType( int type)
{
    this->type = type;
    update();
}

/***********************************************************************************//**
 * 
 */
QSize MSQDicomImageViewerButton::sizeHint() const
{
    return QSize( this->width(), this->height() );
}

/***********************************************************************************//**
 * 
 */
//void MSQDicomImageViewerButton::mousePressEvent(QMouseEvent * e)
//{
//    this->setSelected( true );
//}

/***********************************************************************************//**
 * 
 */
void MSQDicomImageViewerButton::enterEvent(QEvent * e) 
{
    QAbstractButton::enterEvent(e);
    
    this->entering = true;
    update();
}
  
/***********************************************************************************//**
 * 
 */  
void MSQDicomImageViewerButton::leaveEvent(QEvent * e)
{
    QAbstractButton::leaveEvent(e);

    this->entering = false;
    update();
}

/***********************************************************************************//**
 * 
 */
/*void MSQDicomImageViewerButton::resizeEvent(QResizeEvent * e)
{
    QLabel::setPixmap(pix.scaled(this->size(),
        Qt::KeepAspectRatio, Qt::SmoothTransformation));

    recalculateRect();
}*/