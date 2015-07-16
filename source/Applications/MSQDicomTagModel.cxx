/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomTagModel.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomTagModel.h"

/***********************************************************************************//**
 *
 */
MSQDicomTagModel::MSQDicomTagModel(QObject *parent) : QStandardItemModel(parent)
{
	setColumnCount(2);  // Tag, Name

	QStringList header;
	header << "Tag" << "Decription";
	setHorizontalHeaderLabels(header);
}

/***********************************************************************************//**
 *
 */
void MSQDicomTagModel::insertEntry(const QString &t, const QString &de, int row)
{
	// Create tag item
	QStandardItem *tag = new QStandardItem(t);
	//tag->setData(t, Qt::EditRole);

	// Create tag name entry
	QStandardItem *name = new QStandardItem(de);

	// Create list
	QList<QStandardItem *> entry;
	entry << tag << name;

	if (row == -1)
		appendRow(entry);
	else
		insertRow(row, entry);
}

void MSQDicomTagModel::insertEntry(const gdcm::Tag &t, const gdcm::DictEntry &de, int row)
{
	QString groupElement = QString("%1,%2").
          arg(t.GetGroup(), 4, 16, QChar('0')).
          arg(t.GetElement(), 4, 16, QChar('0'));

    insertEntry(groupElement, de.GetName(), row);
}

/***********************************************************************************//**
 *
 */
void MSQDicomTagModel::appendEntry(const QString &t, const QString &de)
{
	insertEntry(t, de);
}

/***********************************************************************************//**
 *
 */
void MSQDicomTagModel::appendEntry(const gdcm::Tag &t, const gdcm::DictEntry &de)
{
	QString groupElement = QString("%1,%2").
          arg(t.GetGroup(), 4, 16, QChar('0')).
          arg(t.GetElement(), 4, 16, QChar('0'));

    insertEntry(groupElement, de.GetName());
}