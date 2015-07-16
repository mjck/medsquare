/*=========================================================================

 Program:   MedSquare
 Module:    MSQSortOrderDelegate.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQSortOrderDelegate.h"
#include "MSQDicomTagSortingModel.h"

/***********************************************************************************//**
 *
 */
MSQSortOrderDelegate::MSQSortOrderDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

/***********************************************************************************//**
 *
 */
 QWidget *MSQSortOrderDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
 {
 	QFont font;
 	font.setPointSize(11);
 	QListWidget *editor = new QListWidget(parent);
 	editor->setFont(font);
 	editor->addItem("Ascending");
 	editor->addItem("Descending");
 	editor->adjustSize();
 	connect(editor, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(commitAndCloseEditor()));
 	return editor;
 }

/***********************************************************************************//**
 *
 */
 void MSQSortOrderDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
 	QString order = index.model()->data(index, Qt::EditRole).toString(); 	 
 	QListWidget *listWidget = qobject_cast<QListWidget *>(editor);
 	listWidget->setCurrentRow(order == "Ascending" ? 0 : 1);
 }

/***********************************************************************************//**
 *
 */
void MSQSortOrderDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QListWidget *listWidget = qobject_cast<QListWidget *>(editor);
    model->setData(index, listWidget->currentItem()->data(Qt::EditRole).toString(), Qt::EditRole);
}

/***********************************************************************************//**
 *
 */
void MSQSortOrderDelegate::commitAndCloseEditor()
{
	QListWidget *editor = qobject_cast<QListWidget *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

/***********************************************************************************//**
 *
 */
void MSQSortOrderDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QRect r = option.rect;
	QSize sz = editor->sizeHint();

	QFont font;
 	font.setPointSize(11);
	QFontMetrics *fm = new QFontMetrics(editor->font());
	QRect rect = fm->boundingRect("Descending");
	r.setSize(QSize(rect.width()*3,rect.height()*3));

	editor->setGeometry( r );
}

