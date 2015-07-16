/*=========================================================================

 Program:   MedSquare
 File:      MSQDicomTagModel.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOM_TAGMODEL_H
#define MSQ_DICOM_TAGMODEL_H

#include <QStandardItemModel>

#include "gdcmGlobal.h"
#include "gdcmDict.h"

class MSQDicomTagModel : public QStandardItemModel
{
    Q_OBJECT

public:
    MSQDicomTagModel(QObject *parent = 0);

    void insertEntry(const QString &t, const QString &de, int row = -1);
    void insertEntry(const gdcm::Tag &t, const gdcm::DictEntry &de, int row);
    void appendEntry(const gdcm::Tag &t, const gdcm::DictEntry &de);
    void appendEntry(const QString &t, const QString &de);

private slots:
//    void updateCloseButton(const QString &text);

private:
};

#endif // MSQ_DICOM_TAGMODEL_H
