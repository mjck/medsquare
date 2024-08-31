/*=========================================================================

 Program:   MedSquare
 Module:    MSQTagSortItem.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_TAGSORTITEM_H
#define MSQ_TAGSORTITEM_H

#include <QtGui>
#include <QList>
#include <QMenu>
#include <QLabel>
#include <QWidget>
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QStringList>
#include <QGridLayout>

#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmStringFilter.h"

class MSQDicomSearchLineEdit;

class MSQTagSortItem : public QWidget
{
    Q_OBJECT

public:
	MSQTagSortItem(QWidget *parent = 0);
	void setModel(QAbstractItemModel *model);
    void setTag(const QString& tag);
    void setTag(const gdcm::Tag& tag);

    bool isValid();
    gdcm::Tag tag();
    const QString alias();
    const QString description();
    const QString toString();
    void setAlias(const QString& alias); 
    void setOrder(int order);
    int order();
    void setGrouped(bool group);
    bool isGrouped();

    void reset();

signals:
  void addTriggered(MSQTagSortItem *item);
  void removeTriggered(MSQTagSortItem *item);

protected:
	void createInterface();

private slots:
    void triggerAddTag();
    void triggerRemoveTag();
    void triggerAscendingOrder();
    void triggerDescendingOrder();
    //void editingFinished();
    //void itemSelected(const QModelIndex& index);

private:
	//QCompleter *mCompleter;
	QToolBar *mToolBar;
    QToolButton *mToolButton;
    MSQDicomSearchLineEdit *mLineEdit;
    QLineEdit *mAlias;
    QCheckBox *mGroup;
};

#endif // MSQ_TAGSORTITEM_H
