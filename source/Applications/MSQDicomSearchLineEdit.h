/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomSearchLineEdit.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOM_SEARCHLINEEDIT_H
#define MSQ_DICOM_SEARCHLINEEDIT_H

#include <QLineEdit>
#include <QModelIndex>

class QToolButton;
class QMenu;
class QActionGroup;
class QTreeView;
class QAbstractItemModel;

class MSQDicomSearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    MSQDicomSearchLineEdit(QWidget *parent = 0);
    virtual ~MSQDicomSearchLineEdit();

    void searchByGroupElement();
    void searchByName();

    void setText(const QString& text);
    void setAlias(QLineEdit *alias);

    //void setCompleter(QCompleter *c);
    void setModel(QAbstractItemModel *model);
    //QCompleter *completer() const;

    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void focusInEvent(QFocusEvent *event);

    void ensureCompletion();
    bool isValid();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void updateClearButton(const QString &text);
    void showPopupMenu();
    void enableSearchByName();
    void enableSearchByGroupElement();
    void selectedItem(const QModelIndex& index);
    //void editingFinished();
    //void selectedItem(const QString& text);

private:
    QLineEdit *mAlias;
    QToolButton *clearButton;
    QToolButton *lensButton;
    QMenu *searchMenu;
    QActionGroup *mActionGroup;
    QCompleter *mCompleter;
    QTreeView *mPopupView;
    bool completionValid;
};

#endif // MSQ_DICOM_SEARCHLINEDIT_H
