/*=========================================================================

 Program:   MedSquare
 File:      MSQSortOrderDelegate.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_SORTORDER_DELEGATE_H
#define MSQ_SORTORDER_DELEGATE_H

#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QListWidget>
#include <QtGui>

class MSQSortOrderDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MSQSortOrderDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;

private slots:
     void commitAndCloseEditor();

private:
};

#endif // MSQ_SORTORDER_DELEGATE_H
