/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomTagSortingModel.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomTagSortingModel.h"

/***********************************************************************************//**
 *
 */
MSQDicomTagSortingModel::MSQDicomTagSortingModel(QObject *parent) : QStandardItemModel(parent)
{
	setColumnCount(4);  // Tag, Name, Order, Extra

	QStringList header;
	header << "Tag" << "Decription" << "Order" << "";
	setHorizontalHeaderLabels(header);
}

/***********************************************************************************//**
 *
 */
void MSQDicomTagSortingModel::insertEntry(const QString &t, const QString &de, 
	MSQDicomTagSortingModel::SortingOrder order, int row)
{
	// Create tag item
	QStandardItem *tag = new QStandardItem(t);
	tag->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	// Create tag name entry
	QStandardItem *name = new QStandardItem(de);
	name->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	QStandardItem *sorting = new QStandardItem(order == Ascending ? 
		QString("Ascending") : QString("Descending"));
	sorting->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

	// Create list
	QList<QStandardItem *> entry;
	entry << tag << name << sorting;

	if (row == -1)
		appendRow(entry);
	else
		insertRow(row, entry);
}

void MSQDicomTagSortingModel::insertEntry(const gdcm::Tag &t, const gdcm::DictEntry &de, 
	MSQDicomTagSortingModel::SortingOrder order, int row)
{
	QString groupElement = QString("%1,%2").
          arg(t.GetGroup(), 4, 16, QChar('0')).
          arg(t.GetElement(), 4, 16, QChar('0'));

    insertEntry(groupElement, de.GetName(), order, row);
}

/***********************************************************************************//**
 *
 */
void MSQDicomTagSortingModel::appendEntry(const QString &t, const QString &de, 
	MSQDicomTagSortingModel::SortingOrder order)
{
	insertEntry(t, de, order);
}

/***********************************************************************************//**
 *
 */
void MSQDicomTagSortingModel::appendEntry(const gdcm::Tag &t, const gdcm::DictEntry &de,
		MSQDicomTagSortingModel::SortingOrder order)
{
	QString groupElement = QString("%1,%2").
          arg(t.GetGroup(), 4, 16, QChar('0')).
          arg(t.GetElement(), 4, 16, QChar('0'));

    insertEntry(groupElement, de.GetName(), order);
}