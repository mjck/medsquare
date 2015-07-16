/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef MSQ_SEARCHLINEEDIT_H
#define MSQ_SEARCHLINEEDIT_H

#include <QLineEdit>

class QToolButton;
class QLabel;

class MSQSearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    MSQSearchLineEdit(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void updateCloseButton(const QString &text);

private:
    QToolButton *clearButton;
    QLabel *lensLabel;
};

#endif // MSQ_SEARCHLINEDIT_H
