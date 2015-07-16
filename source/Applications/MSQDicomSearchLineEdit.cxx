/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomSearchLineEdit.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomSearchLineEdit.h"

#include <QToolButton>
#include <QTreeView>
#include <QHeaderView>
#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QStyle>
#include <QDebug>

/***********************************************************************************//**
 *
 */
MSQDicomSearchLineEdit::MSQDicomSearchLineEdit(QWidget *parent)
    : QLineEdit(parent), completionValid(false)
{
    QFont font;
    font.setPointSize(11);

    mAlias = NULL;

    mPopupView = new QTreeView(this);
    mPopupView->setFont(font);
    mPopupView->setAlternatingRowColors(true);
    mPopupView->setRootIsDecorated(false);
    mPopupView->header()->hide();
    mPopupView->header()->setStretchLastSection(true);
    mPopupView->header()->setResizeMode(1, QHeaderView::ResizeToContents);

    mCompleter = new QCompleter(this);
    mCompleter->setPopup(mPopupView);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setWidget(this);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(mCompleter, SIGNAL(activated(const QModelIndex &)), this, SLOT(selectedItem(const QModelIndex &)));

    mActionGroup = new QActionGroup(this);

    QAction *actionSearchByGroupElement = new QAction(tr("Search By Group, Element"), this);
    actionSearchByGroupElement->setStatusTip(tr("Search by tag's group and element"));
    QObject::connect(actionSearchByGroupElement, SIGNAL(triggered()), this, SLOT(enableSearchByGroupElement()));
    actionSearchByGroupElement->setCheckable(true);

    QAction *actionSearchByName = new QAction(tr("Search By Name"), this);
    actionSearchByName->setStatusTip(tr("Search by tag's name"));
    QObject::connect(actionSearchByName, SIGNAL(triggered()), this, SLOT(enableSearchByName()));
    actionSearchByName->setCheckable(true);

    mActionGroup->addAction(actionSearchByGroupElement);
    mActionGroup->addAction(actionSearchByName);
    actionSearchByGroupElement->setChecked(true);

    searchMenu = new QMenu(this);
    searchMenu->setFont(font);
    searchMenu->addAction(actionSearchByGroupElement);
    searchMenu->addAction(actionSearchByName);

    lensButton = new QToolButton(this);
    QPixmap lens(":/images/search.png");
    lensButton->setIcon(QIcon(lens));
    lensButton->setCursor(Qt::ArrowCursor);
    lensButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    lensButton->hide();

    QObject::connect(lensButton, SIGNAL(clicked()), this, SLOT(showPopupMenu()));

    clearButton = new QToolButton(this);
    QPixmap pixmap(":/images/search_cancel.png");
    clearButton->setIcon(QIcon(pixmap));
    clearButton->setIconSize(QSize(11,11));//pixmap.size());
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->hide();
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    QObject::connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateClearButton(const QString&)));
    //QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(editingFinished()));

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-left: %1px; padding-right: %2px; } ").arg(lensButton->sizeHint().width() + frameWidth).arg(clearButton->sizeHint().width() + frameWidth + 1));
    QSize msz = minimumSizeHint();

    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().width() + frameWidth * 2 + 2),
                   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

/***********************************************************************************//**
 *
 */
bool MSQDicomSearchLineEdit::isValid()
{
    return completionValid;
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::setText(const QString& text)
{
    searchByGroupElement();
    mCompleter->setCompletionPrefix(text);

    QAbstractItemModel *completionModel = mCompleter->completionModel();
    QModelIndex indexGroupElement = completionModel->index(0, 0);
    QModelIndex indexName = completionModel->index(0, 1);

    completionValid = true;
    QLineEdit::setText(QString("%1: %2").arg(indexGroupElement.data().toString()).arg(indexName.data().toString()));

    //printf("setText\n");

    if (mAlias)
        mAlias->setText(indexName.data().toString().replace(QRegExp("\\([^\\(]*\\)"), "").remove(QChar(' '), Qt::CaseInsensitive));
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::setAlias(QLineEdit *alias)
{
    mAlias = alias;
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::ensureCompletion()
{
    mCompleter->setCompletionPrefix(QLineEdit::text());

    QAbstractItemModel *completionModel = mCompleter->completionModel();
    QModelIndex indexGroupElement = completionModel->index(0, 0);

    if (!indexGroupElement.isValid()) {
        mCompleter->setCompletionPrefix("");
        completionModel = mCompleter->completionModel();
        indexGroupElement = completionModel->index(0, 0);
    }

    QModelIndex indexName = completionModel->index(0, 1);
    completionValid = true;

    QLineEdit::setText(QString("%1: %2").arg(indexGroupElement.data().toString()).arg(indexName.data().toString()));

    //printf("ensureCompletion\n");

     if (mAlias)
        mAlias->setText(indexName.data().toString().replace(QRegExp("\\([^\\(]*\\)"), "").remove(QChar(' '), Qt::CaseInsensitive));
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    lensButton->move(rect().left() + 2, (rect().bottom() - sz.height())/2 - 2);

    clearButton->move(rect().right() - frameWidth - sz.width() - 3,
                      (rect().bottom() + 1 - sz.height())/2 - 1);
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (mCompleter && mCompleter->popup()->isVisible()) {

        QAbstractItemModel *completionModel;
        QModelIndex indexGroupElement;
        QModelIndex indexName;

        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
            case Qt::Key_Escape:  // if ESC, then choose the first item as default
                    mCompleter->setCompletionPrefix(QLineEdit::text());
                    mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0, 0));
                    mCompleter->popup()->hide();
                    completionModel = mCompleter->completionModel();
                    indexGroupElement = completionModel->index(0, 0);
                    indexName = completionModel->index(0, 1);
                    completionValid = true;
                    QLineEdit::setText(QString("%1: %2").arg(indexGroupElement.data().toString()).arg(indexName.data().toString()));
                    return; 
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                 e->ignore();
                 return; // let the completer do default behavior

            default:
                break;
        }
    }

     bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
     if (!mCompleter || !isShortcut) // do not process the shortcut when we have a completer
         QLineEdit::keyPressEvent(e);

     const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
     if (!mCompleter || (ctrlOrShift && e->text().isEmpty()))
         return;

     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = QLineEdit::text();

     if (!isShortcut && (hasModifier || e->text().isEmpty())) { 
         mCompleter->popup()->hide();
         completionValid = false;
         return;
     }

     if (completionPrefix != mCompleter->completionPrefix()) {
         mCompleter->setCompletionPrefix(completionPrefix);
         mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0, 0));
     }  

     completionValid = false;

     mCompleter->complete();
}

/***********************************************************************************//**
 *
 */
 void MSQDicomSearchLineEdit::setModel(QAbstractItemModel *model)
 {
    mCompleter->setModel(model);
 }

/***********************************************************************************//**
 *
 */
/*void MSQDicomSearchLineEdit::setCompleter(QCompleter *completer)
{
    if (c)
        QObject::disconnect(c, 0, this, 0);

    c = completer;
    
    if (!c)
        return;

    c->setPopup(mPopupView);
    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);

    //QObject::connect(c, SIGNAL(activated(const QString &)), this, SLOT(selectedItem(const QString &)));
    QObject::connect(c, SIGNAL(activated(const QModelIndex &)), this, SLOT(selectedItem(const QModelIndex &)));
}*/

/***********************************************************************************//**
 *
 */
/*QCompleter *MSQDicomSearchLineEdit::completer() const
{
    return c;
}*/

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::focusOutEvent(QFocusEvent * e)
{
    lensButton->hide();
    clearButton->hide();

    if (mCompleter) {
        mCompleter->setWidget(this);
        if (!completionValid || QLineEdit::text().isEmpty()) {
            ensureCompletion();
        }
    }

    QLineEdit::focusOutEvent(e);
}
    
/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::focusInEvent(QFocusEvent * e)
{
     QLineEdit::focusInEvent(e);
     lensButton->show();
     clearButton->setVisible(!text().isEmpty());
}  

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::selectedItem(const QModelIndex& index)
{
    completionValid = true;
    QAbstractItemModel *completionModel = mCompleter->completionModel();
    QModelIndex indexGroupElement = completionModel->index(index.row(), 0);
    QModelIndex indexName = completionModel->index(index.row(), 1);
    QLineEdit::setText(QString("%1: %2").arg(indexGroupElement.data().toString()).arg(indexName.data().toString()));
    if (mAlias)
        mAlias->setText(indexName.data().toString().replace(QRegExp("\\([^\\(]*\\)"), "").remove(QChar(' '), Qt::CaseInsensitive));

}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::updateClearButton(const QString& text)
{
    clearButton->setVisible(!text.isEmpty() && hasFocus());
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::showPopupMenu()
{
    searchMenu->exec(lensButton->mapToGlobal(QPoint(0,lensButton->sizeHint().height())));
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::searchByGroupElement()
{
    QList<QAction *> actions = mActionGroup->actions();
    actions[0]->trigger();
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::searchByName()
{
    QList<QAction *> actions = mActionGroup->actions();
    actions[1]->trigger();
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::enableSearchByName()
{
    mCompleter->setCompletionColumn(1);
}

/***********************************************************************************//**
 *
 */
void MSQDicomSearchLineEdit::enableSearchByGroupElement()
{
    mCompleter->setCompletionColumn(0);
}

/***********************************************************************************//**
 *
 */
MSQDicomSearchLineEdit::~MSQDicomSearchLineEdit()
{
}


