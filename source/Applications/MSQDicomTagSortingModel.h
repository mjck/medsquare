/*=========================================================================

 Program:   MedSquare
 File:      MSQDicomTagSortingModel.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOM_TAGSORTING_MODEL_H
#define MSQ_DICOM_TAGSORTING_MODEL_H

#include <QStandardItemModel>

#include "gdcmGlobal.h"
#include "gdcmDict.h"

class MSQDicomTagSortingModel : public QStandardItemModel
{
    Q_OBJECT

public:
	enum SortingOrder {
		Ascending,
		Descending
	};

    MSQDicomTagSortingModel(QObject *parent = 0);

    void insertEntry(const QString &t, const QString &de, 
    	MSQDicomTagSortingModel::SortingOrder order = Ascending, int row = -1);
    void insertEntry(const gdcm::Tag &t, const gdcm::DictEntry &de, 
    	MSQDicomTagSortingModel::SortingOrder order, int row);
    void appendEntry(const gdcm::Tag &t, const gdcm::DictEntry &de, 
    	MSQDicomTagSortingModel::SortingOrder order = Ascending);
    void appendEntry(const QString &t, const QString &de, 
    	MSQDicomTagSortingModel::SortingOrder order = Ascending);

private slots:
//    void updateCloseButton(const QString &text);

private:
};

#endif // MSQ_DICOM_TAGSORTING_MODEL_H
