/*=========================================================================

 Program:   MedSquare
 Module:    MSQDicomHeaderViewer.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQDicomHeaderViewer.h"
#include "MSQSearchLineEdit.h"

#include "gdcmPrinter.h"
#include "gdcmImageReader.h"


/***********************************************************************************//**
 *
 */
MSQDicomHeaderViewer::MSQDicomHeaderViewer()
{
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(QMargins(0,0,0,0));
  layout->setSpacing(4);

  //MSQSearchLineEdit *searchTag = new MSQSearchLineEdit;
  //searchTag->setPlaceholderText(tr("Search"));
  //searchTag->setContentsMargins(QMargins(0,0,0,0));
  //searchTag->setTextMargins(QMargins(2,2,2,2));
  //tagEdit->setStyleSheet("border-width: 2px;"
  //    "border-radius: 7px; "
  //    "border-color: darkgray;");
      //"min-width: 6em;");
  //findTagLayout->addWidget(searchTag);

  QStringList tagLabels; 
  tagLabels << "Tag" << "Name" << "Value";
  treeTag = new QTreeWidget;
  treeTag->setContentsMargins(QMargins(0,0,0,0));
  treeTag->setColumnCount(3);
  treeTag->setHeaderLabels(tagLabels);
  treeTag->setAlternatingRowColors(true);
  //treeTag->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  treeTag->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  treeTag->header()->setResizeMode(QHeaderView::ResizeToContents);

  //layout->addWidget(searchTag);
  layout->addWidget(treeTag); 

  this->setLayout(layout);
  //this->setContentsMargins(QMargins(0,0,0,0));
}

/***********************************************************************************//**
 *
 */
void MSQDicomHeaderViewer::setInput(const QString& fileName)
{
  QFont font;
  font.setPointSize(11);

  gdcm::ImageReader reader;
  reader.SetFileName(fileName.toLocal8Bit().constData());
  if (!reader.Read())
  {
    treeTag->clear();

    QTreeWidgetItem *item = new QTreeWidgetItem(treeTag);
    item->setText(0, QString("No tags to show!"));
    item->setFont(0, font);
    return;
  }

  std::ostringstream oss(std::ostringstream::out);

  // let's use GDCM's printer to retrieve tags appropriately
  gdcm::Printer printer;
  printer.SetFile(reader.GetFile());
  printer.Print(oss);

  // get tags as string list
  QStringList tagList = QString::fromStdString(oss.str()).split("\n");

  // clear tag tree
  treeTag->clear();

  QTreeWidgetItem *parentItem = treeTag->invisibleRootItem();

  // let's populate the tag tree
  QStringListIterator itemIterator(tagList);
  while (itemIterator.hasNext())
  {
    // get item
    const QString& item = itemIterator.next().trimmed();

    // handle meta information
    if (item == "# Dicom-Meta-Information-Header")
    {
      parentItem = new QTreeWidgetItem(treeTag);
      parentItem->setText(0, "Metadata");
      parentItem->setFont(0, font);
    }
    else if (item == "# Dicom-Data-Set")
    {
      parentItem = new QTreeWidgetItem(treeTag);
      parentItem->setText(0, "Dataset");
      parentItem->setFont(0, font);
    }
    else if (item.startsWith("("))
    {
      QString tag = item.left(11);
      QString name, value;

      // Only add to list if there is a description for the tag
      // otherwise is not worth the trouble.
      int index = item.lastIndexOf(QChar('#'));

      if (index > 0)
      {
        name = item.mid(index).mid(2);
        name = name.mid(name.indexOf(' ') + 1).trimmed();
        value = item.mid(15, index - 15).trimmed();

        QTreeWidgetItem *newItem = new QTreeWidgetItem(parentItem);
        newItem->setText(0, tag);
        newItem->setText(1, name);
        newItem->setText(2, value);
        newItem->setFont(0, font);
        newItem->setFont(1, font);
        newItem->setFont(2, font);
      }
    }
  }

  treeTag->expandToDepth(0);
}

/***********************************************************************************//**
 *
 */
MSQDicomHeaderViewer::~MSQDicomHeaderViewer()
{
  //this->interStyle->Delete();
  //this->renderer->Delete();

  //delete this->treeTag;
}

