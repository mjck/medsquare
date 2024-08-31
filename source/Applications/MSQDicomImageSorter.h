/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomImageSorter.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_DICOMIMAGE_SORTER_H
#define MSQ_DICOMIMAGE_SORTER_H

#include <QtGui>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileDialog>

#include "gdcmDicts.h"
#include "gdcmGlobal.h"
#include "gdcmDict.h"

class MSQDicomTagModel;
//class MSQDicomTagSortingModel;
class MSQDicomSearchLineEdit;
//class MSQTagWidget;
class MSQTagSortItem;

class MSQDicomImageSorter : public QWidget
{
Q_OBJECT

public:
  MSQDicomImageSorter();
  virtual ~MSQDicomImageSorter();

  int precision();
  bool precisionEnabled();

  QVector<gdcm::Tag> tags();
  QVector<int> orders();
  QStringList descriptions();
  QStringList aliases();
  QVector<bool> groups();
  //QStringList getSortingCriteria();

public slots:
  //void showOrHideEditor();
  //virtual void setComponent(int component) = 0;
  //virtual void setLevel(double value) = 0;
  //virtual void setWindow(double value) = 0;
  //virtual void setOpacity(double value) = 0;

signals:
  void sortButtonClicked();

private slots:
  MSQTagSortItem *insertTag();
  void insertTagAt(MSQTagSortItem *);
  void removeTag(MSQTagSortItem *);
  void removeTags();
  void sortImages();
  void resetCriteria();
  void loadCriteria();
  void saveCriteria();
  void sortByCriteria();
  void togglePrecision();

  //void tagSelected(const QModelIndex & index);
  //void currentTagChanged(const QModelIndex& current, const QModelIndex& previous);
  //void searchLineEdited(const QString & text);

  //void insertTagIntoList();
  //void removeTagFromList();

protected:
  QString currentFileName;
  QString currentPath;
  MSQDicomTagModel *tagDictionary;
  QStringList mDefaultTagList;
  //MSQDicomTagSortingModel *tagList;
  //MSQTagWidget *tagLabels;
  //QTreeView *tagView;
  //QToolButton *mSortButton;
  //QToolButton *sortButton;


  QVBoxLayout *mTagSortLayout;
  //QWidget *mSortItemWidget;

  //MSQDicomSearchLineEdit *searchLineEdit;
  //QCompleter *completer;

  //bool selectionValid;
  //QString selectionTagName;
  //QString selectionGroupElement;

  QAction *mActionAdd;
  QAction *mActionRemove;
  QPushButton *mOpenButton;
  QPushButton *mSaveButton;
  QPushButton *mResetButton;
  QPushButton *mSortButton;
  QCheckBox *mEnablePrecision;
  QLineEdit *mPrecision;
  //QAction *mActionSort;
  //QAction *actionRemoveTag;
  
  //void createTagList();
  void createDictionary();
  void createInterface();
  void createDefaultTagList();
  //void appendWidget(const QModelIndex& index, bool isFirst = false);
  //void createDelegates();
  //void populateDefaultList();
  //void appendTag(QString &groupElement, QString &name);
};

#endif

