/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomImageSorter.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomImageSorter.h"
#include "MSQDicomSearchLineEdit.h"
#include "MSQDicomTagModel.h"
#include "MSQDicomTagSortingModel.h"
#include "MSQSortOrderDelegate.h"
#include "MSQTagSortItem.h"

#include "gdcmPrinter.h"
#include "gdcmImageReader.h"

#include <qglobal.h>

#include <QtCore>
#include <QtGui>

#include <set>


/***********************************************************************************//**
 *
 */
MSQDicomImageSorter::MSQDicomImageSorter()
{
  currentFileName = "";
  currentPath = QDir::currentPath();
  createDictionary();
  createInterface();
  createDefaultTagList();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::createDictionary()
{
  const gdcm::Global& g = gdcm::Global::GetInstance(); // sum of all knowledge !
  const gdcm::Dicts &ds = g.GetDicts();
  const gdcm::Dict &pub = ds.GetPublicDict();
  const gdcm::CSAHeaderDict &csadict = ds.GetCSAHeaderDict();
  const gdcm::PrivateDict &priv = ds.GetPrivateDict();

  // relevant GE private tags
  //const gdcm::DictEntry GEDictBValue( 0x0043, 0x1039, "IS", "1", "B Value of diffusion weighting" );
  //const gdcm::DictEntry GEDictXGradient( 0x0019, 0x10bb, "DS", "1", "X component of gradient direction" );
  //const gdcm::DictEntry GEDictYGradient( 0x0019, 0x10bc, "DS", "1", "Y component of gradient direction" );
  //const gdcm::DictEntry GEDictZGradient( 0x0019, 0x10bd, "DS", "1", "Z component of gradient direction" );

  // relevant Siemens private tags
  const gdcm::DictEntry SiemensDictMatrixSize( "AcquisitionMatrixText", "SIEMENS MR HEADER",
    gdcm::VR::SH, gdcm::VM::VM1, "Acquisition matrix size" );

  const gdcm::DictEntry SiemensDictNMosaic( "NumberOfImagesInMosaic", "SIEMENS MR HEADER", 
    gdcm::VR::US, gdcm::VM::VM1, "Number of images in mosaic" );       
  
  const gdcm::DictEntry SiemensDictBValue( "B_value", "SIEMENS MR HEADER", 
    gdcm::VR::IS, gdcm::VM::VM1, "Diffusion effect in s/mm*mm of the ICE program for the performed acquisition" );

  const gdcm::DictEntry SiemensDictDiffusionDirection( "DiffusionGradientDirection", "SIEMENS MR HEADER",
    gdcm::VR::FD, gdcm::VM::VM3, "Diffusion gradient direction");
    
  const gdcm::DictEntry SiemensDictDiffusionBMatrix( "B_matrix", "SIEMENS MR HEADER",
    gdcm::VR::FD, gdcm::VM::VM6, "Diffusion B Matrix" );

  //const gdcm::DictEntry SiemensDictShadowInfo( 0x0029, 0x1010, "OB", "1", "Siemens DWI Info" );   

  gdcm::Dict::ConstIterator it = pub.Begin();
  std::set<std::string> names;

  tagDictionary = new MSQDicomTagModel();

  // scan public dictionary
  for( ; it != pub.End(); ++it)
  {
    const gdcm::Tag &t = it->first;
    const gdcm::DictEntry &de = it->second;

    // A couple of tests:
    //if( t.GetElement() == 0x0 )
      //{
        // Check group length
        //if( de.GetVR() != gdcm::VR::UL || de.GetVM() != gdcm::VM::VM1 )
          //{
            //std::cerr << "Group length issue: Problem with tag: " << t << " " << de << std::endl;
            //++ret;
          //}

      //} else {

        if (names.count(de.GetName()) == 0) 
        {
          tagDictionary->appendEntry(t, de);
          names.insert( de.GetName() );
        }
    //}
  }

  // append selected tags from Siemens private header
  tagDictionary->appendEntry(gdcm::Tag(0x0051, 0x100b), SiemensDictMatrixSize);
  tagDictionary->appendEntry(gdcm::Tag(0x0019, 0x100a), SiemensDictNMosaic);
  tagDictionary->appendEntry(gdcm::Tag(0x0019, 0x100c), SiemensDictBValue);
  tagDictionary->appendEntry(gdcm::Tag(0x0019, 0x100e), SiemensDictDiffusionDirection);
  tagDictionary->appendEntry(gdcm::Tag(0x0019, 0x1027), SiemensDictDiffusionBMatrix);

}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::createDefaultTagList()
{
  mDefaultTagList << "0020,0010" // Study ID
                  << "0008,103e" // Series Description
                  << "0020,0011"  // Series Number
                  << "0020,1041";  // Slice Location
  insertTag();
  insertTag();
  insertTag(); 
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::insertTagAt(MSQTagSortItem *item)
{
  MSQTagSortItem *tag = new MSQTagSortItem(this);

  tag->setModel(tagDictionary);
  tag->setTag(mDefaultTagList.at(mTagSortLayout->count() % mDefaultTagList.count()));
  QObject::connect(tag, SIGNAL(addTriggered(MSQTagSortItem *)), this, SLOT(insertTagAt(MSQTagSortItem *)));
  QObject::connect(tag, SIGNAL(removeTriggered(MSQTagSortItem *)), this, SLOT(removeTag(MSQTagSortItem *)));
  mTagSortLayout->insertWidget(mTagSortLayout->indexOf(item) + 1, tag);

  mActionRemove->setEnabled(true);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::removeTag(MSQTagSortItem *item)
{
  mTagSortLayout->removeWidget(item);
  delete item;
  if (mTagSortLayout->count() == 0)
    mActionRemove->setEnabled(false);
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::removeTags()
{
  QLayoutItem *child;
  while ((child = mTagSortLayout->takeAt(0)) != 0) {
     delete child->widget();
  }
  mActionRemove->setEnabled(false);
}

/***********************************************************************************//**
 *
 */
MSQTagSortItem* MSQDicomImageSorter::insertTag()
{
  MSQTagSortItem *tag = new MSQTagSortItem(this);
  tag->setModel(tagDictionary);
  tag->setTag(mDefaultTagList.at(mTagSortLayout->count() % mDefaultTagList.count()));
  QObject::connect(tag, SIGNAL(addTriggered(MSQTagSortItem *)), this, SLOT(insertTagAt(MSQTagSortItem *)));
  QObject::connect(tag, SIGNAL(removeTriggered(MSQTagSortItem *)), this, SLOT(removeTag(MSQTagSortItem *)));
  mTagSortLayout->addWidget(tag);
  mActionRemove->setEnabled(true);

  return tag;
}

/***********************************************************************************//**
 *
 */
QVector<gdcm::Tag> MSQDicomImageSorter::tags()
{
  QVector<gdcm::Tag> list;
  QLayoutItem *child;
  MSQTagSortItem *item;
  for(int i=0; i<mTagSortLayout->count(); i++) {
    child = mTagSortLayout->itemAt(i);
    item = qobject_cast<MSQTagSortItem *>(child->widget());
    if (!item->isValid())
        item->reset();
    list.append(item->tag());
  }
  return list;
}

/***********************************************************************************//**
 *
 */
QVector<int> MSQDicomImageSorter::orders()
{
  QVector<int> list;
  QLayoutItem *child;
  MSQTagSortItem *item;
  for(int i=0; i<mTagSortLayout->count(); i++) {
    child = mTagSortLayout->itemAt(i);
    item = qobject_cast<MSQTagSortItem *>(child->widget());
    if (!item->isValid())
        item->reset();
    list.append(item->order());
  }
  return list;
}

/***********************************************************************************//**
 *
 */
 QStringList MSQDicomImageSorter::descriptions()
 {
  QStringList list;
  QLayoutItem *child;
  MSQTagSortItem *item;
  for(int i=0; i<mTagSortLayout->count(); i++) {
    child = mTagSortLayout->itemAt(i);
    item = qobject_cast<MSQTagSortItem *>(child->widget());
    if (!item->isValid())
        item->reset();
    list.append(item->description());
  }
  return list;
 }

/***********************************************************************************//**
 *
 */
 QStringList MSQDicomImageSorter::aliases()
 {
  QStringList list;
  QLayoutItem *child;
  MSQTagSortItem *item;
  for(int i=0; i<mTagSortLayout->count(); i++) {
    child = mTagSortLayout->itemAt(i);
    item = qobject_cast<MSQTagSortItem *>(child->widget());
    if (!item->isValid())
        item->reset();
    list.append(item->alias());
  }
  return list;
 }

/***********************************************************************************//**
 *
 */
QVector<bool> MSQDicomImageSorter::groups()
{
  QVector<bool> list;
  QLayoutItem *child;
  MSQTagSortItem *item;
  for(int i=0; i<mTagSortLayout->count(); i++) {
    child = mTagSortLayout->itemAt(i);
    item = qobject_cast<MSQTagSortItem *>(child->widget());
    if (!item->isValid())
        item->reset();
    list.append(item->isGrouped());
  }
  return list;
}

/***********************************************************************************//**
 *
 */
 void MSQDicomImageSorter::sortImages()
 {
  QLayoutItem *child;
  MSQTagSortItem *item;
  for(int i=0; i<mTagSortLayout->count(); i++) {
    child = mTagSortLayout->itemAt(i);
    item = qobject_cast<MSQTagSortItem *>(child->widget());
    if (!item->isValid())
        item->reset();
    gdcm::Tag t = item->tag();
  }
  printf("\n");
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::resetCriteria()
{
  this->removeTags();
  this->createDefaultTagList(); 
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::sortByCriteria()
{
  emit sortButtonClicked();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::loadCriteria()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), currentPath, tr("Sorting criteria (*.crit)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);

    // remove tags
    this->removeTags();

    int count, grouped, order;
    QString strtag, alias;
    gdcm::Tag tag;

    // read count
    in >> count;
    //printf("%d\n", count);

    // read criterium
    for(int i=0; i<count; i++) {
      
      in >> grouped;
      in >> strtag;
      tag.ReadFromPipeSeparatedString(strtag.toLocal8Bit().data());
      in >> alias;
      in >> order;
      //printf("grouped=%d, %s, %d\n", grouped, strtag.toLocal8Bit().data(), order);

      MSQTagSortItem *item = this->insertTag();
      item->setTag(tag);
      item->setGrouped(grouped ? true : false);
      item->setAlias(alias);
      item->setOrder(order);
    }

    currentFileName = fileName;
    currentPath = QFileInfo(fileName).path();
  }
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::saveCriteria()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save sorting criteria"),
      currentPath, tr("Sorting criteria (*.crit)"));

  if (!fileName.isEmpty())
  {
    gdcm::StringFilter filter;

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    QLayoutItem *child;
    MSQTagSortItem *item;

    out << mTagSortLayout->count() << "\n";

    for(int i=0; i<mTagSortLayout->count(); i++) {
      child = mTagSortLayout->itemAt(i);
      item = qobject_cast<MSQTagSortItem *>(child->widget());
      if (item->isValid()) {
        out << (item->isGrouped() ? "1 " : "0 ");
        out << QString::fromStdString(item->tag().PrintAsPipeSeparatedString()) << " ";
        out << item->alias() << " ";
        out << item->order() << "\n";
      } 
    }

    currentPath = QFileInfo(fileName).path();
    currentFileName = fileName;
  }

}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::togglePrecision()
{
    mPrecision->setEnabled(mEnablePrecision->isChecked());
}

/***********************************************************************************//**
 *
 */
int MSQDicomImageSorter::precision()
{
  return mPrecision->text().toInt();
}

/***********************************************************************************//**
 *
 */
bool MSQDicomImageSorter::precisionEnabled()
{
  return mEnablePrecision->isChecked();
}

/***********************************************************************************//**
 *
 */
void MSQDicomImageSorter::createInterface()
{
  QFont font;
  font.setPointSize(11);

  QFont font12;
  font12.setPointSize(12);
  font12.setBold(true); 

  QFont boldFont;
  boldFont.setPointSize(11);
  boldFont.setBold(true);  

  // create main layout
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setContentsMargins(QMargins(8,8,8,8));
  mainLayout->setSpacing(0);

  // create layout for header
  QHBoxLayout *headerLayout = new QHBoxLayout;
  headerLayout->setContentsMargins(QMargins(0,0,0,0));
  headerLayout->setSpacing(10);
  //QGridLayout *headerLayout = new QGridLayout;
  //headerLayout->setColumnStretch(0, 0);
  //headerLayout->setColumnStretch(4, 0);
  //headerLayout->setSpacing(0);
  //headerLayout->setContentsMargins(QMargins(0,0,0,0));

  // create layout for footer
  //QHBoxLayout *footerLayout = new QHBoxLayout;
  //footerLayout->setContentsMargins(QMargins(0,6,0,3));
  //footerLayout->setSpacing(0);

  //QLabel *label = new QLabel("Sorting criteria:", this);
  //label->setFont(font12);

  mResetButton = new QPushButton("Reset", this);
  mResetButton->setStyleSheet("min-width: 30px; max-height: 25px;");
  mResetButton->setFont(font);
  QObject::connect(mResetButton, SIGNAL(clicked()), this, SLOT(resetCriteria()));

  mOpenButton = new QPushButton("Open", this);
  mOpenButton->setStyleSheet("min-width: 30px; max-height: 25px;");
  mOpenButton->setFont(font);
  QObject::connect(mOpenButton, SIGNAL(clicked()), this, SLOT(loadCriteria()));

  mSaveButton = new QPushButton("Save", this);
  mSaveButton->setStyleSheet("min-width: 30px; max-height: 25px;");
  mSaveButton->setFont(font);
  QObject::connect(mSaveButton, SIGNAL(clicked()), this, SLOT(saveCriteria()));

  mSortButton = new QPushButton("Sort", this);
  mSortButton->setStyleSheet("min-width: 30px; max-height: 25px;");
  mSortButton->setFont(font);
  QObject::connect(mSortButton, SIGNAL(clicked()), this, SLOT(sortByCriteria()));

  mEnablePrecision = new QCheckBox("Decimal precision:", this);
  mEnablePrecision->setFont(font);
  QObject::connect(mEnablePrecision, SIGNAL(stateChanged(int)), this, SLOT(togglePrecision()));

  mPrecision = new QLineEdit("5", this);
  mPrecision->setFont(font);
  mPrecision->setPlaceholderText("5");
  mPrecision->setFixedWidth(50);
  mPrecision->setEnabled(false);
  QIntValidator *validator = new QIntValidator;
  validator->setBottom(1);
  mPrecision->setValidator(validator);

  //QLabel *label = new QLabel("Sorting criteria:");
  //label->setFont(boldFont);

  //QLabel *groupLabel = new QLabel("G");
  //groupLabel->setFont(boldFont);
  //QLabel *descLabel = new QLabel("Tag");
  //descLabel->setAlignment(Qt::AlignCenter);
  //descLabel->setFont(boldFont);
  //QLabel *aliasLabel = new QLabel("Alias");
  //aliasLabel->setAlignment(Qt::AlignCenter);
  //aliasLabel->setFont(boldFont);
  //QLabel *orderLabel = new QLabel("Order");
  //orderLabel->setAlignment(Qt::AlignCenter);
  //orderLabel->setContentsMargins(QMargins(0,0,0,0));
  //orderLabel->setFont(boldFont);

  // add button
  mActionAdd = new QAction(QIcon(":/images/add_normal.png"), tr("&Add..."), this);
  mActionAdd->setEnabled(true);
  QObject::connect(mActionAdd, SIGNAL(triggered()), this, SLOT(insertTag()));

  mActionRemove = new QAction(QIcon(":/images/remove_normal.png"), tr("&Remove"), this);
  mActionRemove->setEnabled(false);
  QObject::connect(mActionRemove, SIGNAL(triggered()), this, SLOT(removeTags()));

  // create add and remove item buttons
  QToolBar *toolBar = new QToolBar;
  toolBar->setIconSize(QSize(15,15));
  toolBar->setContentsMargins(QMargins(0,0,0,0));
  toolBar->setStyleSheet("spacing: 0px");
  //toolBar->addAction(mActionSort); 
  toolBar->addAction(mActionRemove);
  toolBar->addAction(mActionAdd);

  //headerLayout->addWidget(groupLabel, 0, Qt::AlignLeft);//, 0, 0);
  //headerLayout->addWidget(descLabel, 3, Qt::AlignHCenter);//, 0, 1);
  //headerLayout->addWidget(aliasLabel, 0, Qt::AlignHCenter);
  //headerLayout->addWidget(orderLabel, 0, Qt::AlignHCenter);//, 0, 2);
  //headerLayout->addWidget(toolBar, 0, Qt::AlignRight);//, 0, 3);
  //headerLayout->addWidget(groupLabel, 0, 0, Qt::AlignLeft);//, 0, 0);
  //headerLayout->addWidget(descLabel, 0, 1, Qt::AlignCenter);//, 0, 1);
  //headerLayout->addWidget(aliasLabel, 0, 2, Qt::AlignCenter);
  //headerLayout->addWidget(orderLabel, 0, 3, Qt::AlignCenter);//, 0, 2);
  //headerLayout->addWidget(toolBar, 0, 4, Qt::AlignRight);//, 0, 3);

  //headerLayout->addWidget(label);
  headerLayout->addWidget(mOpenButton);
  headerLayout->addWidget(mSaveButton);
  headerLayout->addWidget(mResetButton);
  headerLayout->addWidget(mSortButton);
  headerLayout->addStretch();
  headerLayout->addWidget(mEnablePrecision);
  headerLayout->addWidget(mPrecision);
  headerLayout->addWidget(toolBar);
 
  mTagSortLayout = new QVBoxLayout;
  mTagSortLayout->setContentsMargins(QMargins(0,4,0,0));
  mTagSortLayout->setSpacing(0);

  //mSortButton = new QToolButton(this);
  //mSortButton->setText("Sort Images");
  //mSortButton->setCursor(Qt::ArrowCursor);
  //mSortButton->setFont(boldFont);
  //mSortButton->setStyleSheet("QToolButton { border: 2px; } \
  //  QToolButton:hover { color: white; background: darkgray; border-radius: 8px; } \
  //  QToolButton:pressed { color: white; background: gray; border-radius: 8px; }");

  //footerLayout->addStretch();
  //footerLayout->addWidget(mSortButton);
  
  //QFrame* line = new QFrame(this);
  //line->setGeometry(QRect());
  //line->setFrameShape(QFrame::HLine); // Replace by VLine for vertical line
  //line->setFrameShadow(QFrame::Sunken);
  //line->setLineWidth(1);

  //mainLayout->addLayout(mTagSortLayout);
  mainLayout->addLayout(headerLayout);
  mainLayout->addLayout(mTagSortLayout);
  //mainLayout->addWidget(line);
  //mainLayout->addLayout(footerLayout);

  this->setLayout(mainLayout);
  //this->setContentsMargins(QMargins(0,0,0,0));
}


/***********************************************************************************//**
 *
 */
///void MSQDicomImageSorter::tagSelected(const QModelIndex & index)
//{
  // safe to add to tag list
  //this->actionAddTag->setEnabled(true);

  // get whole item information
  //QAbstractItemModel *completionModel = this->completer->completionModel();
  //QModelIndex indexGroupElement = completionModel->index(index.row(),set 0);
  //QModelIndex indexName = completionModel->index(index.row(), 1);

  // store item for delayed addition
  //selectionValid = true;
  //selectionGroupElement = indexGroupElement.data().toString();
  //selectionTagName = indexName.data().toString();
  //qDebug() << indexGroupElement.data().toString() << indexName.data().toString();
//}

/***********************************************************************************//**
 *
 */
//void MSQDicomImageSorter::searchLineEdited(const QString & text)
//{
//   this->actionAddTag->setEnabled(false);
//   selectionValid = false;
//}

/***********************************************************************************//**
 *
 */
//void MSQDicomImageSorter::insertTagIntoList()
//{
//    if (selectionValid) {
//      qDebug() << "selection is valid";

//      tagList->insertEntry(selectionGroupElement, selectionTagName, 
//        MSQDicomTagSortingModel::Ascending, tagView->currentIndex().row()+1);

 //     selectionValid = false;
 //     this->actionAddTag->setEnabled(false);
 //     if (!this->actionRemoveTag->isEnabled())
  //      this->actionRemoveTag->setEnabled(true);

   //   tagView->setCurrentIndex(tagList->index(tagView->currentIndex().row()+1, 0));
    //}
//}

/***********************************************************************************//**
 *
 */
//void MSQDicomImageSorter::removeTagFromList()
//{
 // const QModelIndex& current = tagView->currentIndex();
  //if (current.isValid()) {
   // tagList->removeRows(current.row(), 1);
    //if (tagList->rowCount() == 1)
     //   this->actionRemoveTag->setEnabled(false);
    //this->tagLabels->setTags(this->getSortingCriteria());
  //}
//}

/***********************************************************************************//**
 *
 */
//void MSQDicomImageSorter::currentTagChanged(const QModelIndex& current, const QModelIndex& previous)
//{
  //if (previous.row() != current.row()) {

    //  QModelIndex prev = tagList->index(previous.row(), 3);
      //tagView->setIndexWidget(prev, new QWidget());
    
      //QModelIndex next = tagList->index(current.row(), 3);
      //appendWidget(next, current.row() == 0);
  //}
//}

/***********************************************************************************//**
 *
 */
//void MSQDicomImageSorter::appendWidget(const QModelIndex& index, bool isFirst)
//{
  //  QToolBar *toolBar = new QToolBar;
   // toolBar->setIconSize(QSize(15,15));
  //  toolBar->setContentsMargins(QMargins(0,0,0,0));
  //  toolBar->setStyleSheet("spacing: 0px");
  //  toolBar->addAction(this->actionAddTag);
  //  if (!isFirst)
    //  toolBar->addAction(this->actionRemoveTag);
    //toolBar->setAutoFillBackground(true);

    //tagView->setIndexWidget(index, toolBar);
    //this->tagLabels->setTags(this->getSortingCriteria());
//}

/***********************************************************************************//**
 *
 */
MSQDicomImageSorter::~MSQDicomImageSorter()
{
}

