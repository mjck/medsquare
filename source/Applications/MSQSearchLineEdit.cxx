/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "MSQSearchLineEdit.h"
#include <QToolButton>
#include <QLabel>
#include <QStyle>

MSQSearchLineEdit::MSQSearchLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    lensLabel = new QLabel(this);
    QPixmap lens(":/images/search.png");
    lensLabel->setPixmap(lens);
    lensLabel->setCursor(Qt::ArrowCursor);
    lensLabel->setStyleSheet("QToolButton { border: none; padding: 0px; }");

    clearButton = new QToolButton(this);
    QPixmap pixmap(":/images/search_cancel.png");
    clearButton->setIcon(QIcon(pixmap));
    clearButton->setIconSize(QSize(11,11));//pixmap.size());
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->hide();
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateCloseButton(const QString&)));

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-left: %1px; padding-right: %2px; } ").arg(lensLabel->sizeHint().width() + frameWidth).arg(clearButton->sizeHint().width() + frameWidth + 1));
    QSize msz = minimumSizeHint();

    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().width() + frameWidth * 2 + 2),
                   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void MSQSearchLineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    lensLabel->move(rect().left() + 3, (rect().bottom() - sz.height())/2);

    clearButton->move(rect().right() - frameWidth - sz.width() - 3,
                      (rect().bottom() + 1 - sz.height())/2);
}

void MSQSearchLineEdit::updateCloseButton(const QString& text)
{
    clearButton->setVisible(!text.isEmpty());
}

