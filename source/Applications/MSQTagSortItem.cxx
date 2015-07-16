/*=========================================================================

 Program:   MedSquare
 Module:    MSQTagSortItem.cxx

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "MSQTagSortItem.h"
#include "MSQDicomSearchLineEdit.h"

/***********************************************************************************//**
 *
 */
MSQTagSortItem::MSQTagSortItem(QWidget *parent)
{
   createInterface();
}

/***********************************************************************************//**
 *
 */
 void MSQTagSortItem::setTag(const gdcm::Tag& tag)
 {
    QString tagnumber = QString("%1,%2").
    arg(tag.GetGroup(), 4, 16, QChar('0')).
    arg(tag.GetElement(), 4, 16, QChar('0'));
    //printf("%s\n",tagnumber.toLocal8Bit().data());
    //printf("tag=%d,%d\n",tag.GetGroup(),tag.GetElement());
    mLineEdit->setText(tagnumber);
 }

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::setTag(const QString& tag)
{
    mLineEdit->setText(tag);
    //mAlias->setText(mLineEdit->text().mid(11).replace(QRegExp("\\([^\\(]*\\)"), "").remove(QChar(' '), Qt::CaseInsensitive));
}

/***********************************************************************************//**
 *
 */
bool MSQTagSortItem::isValid()
{
    return mLineEdit->isValid();
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::reset()
{
    mLineEdit->ensureCompletion();
}

/***********************************************************************************//**
 *
 */
gdcm::Tag MSQTagSortItem::tag()
{
    gdcm::Tag t(0x0020, 0x000d); // default Study UID
    QString str = mLineEdit->text().left(9);
    if (!str.isEmpty()) {
        const char *groupElement = str.toStdString().c_str();
        //printf("%s\n",groupElement);
        t.ReadFromCommaSeparatedString(groupElement);
    }
    return t;
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::setAlias(const QString& alias)
{
    mAlias->setText(alias);
}

/***********************************************************************************//**
 *
 */
const QString MSQTagSortItem::alias()
{
    QString str = mAlias->text();
    return str;
}

/***********************************************************************************//**
 *
 */
const QString MSQTagSortItem::description()
{
    QString str = mLineEdit->text().mid(11);
    //if (!str.isEmpty()) {
    //    const char *groupElement = str.toStdString().c_str();
        //printf("%s\n",groupElement);
    //    t.ReadFromCommaSeparatedString(groupElement);
    //}
    return str;
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::setGrouped(bool group)
{
    mGroup->setChecked(group);
}

/***********************************************************************************//**
 *
 */
bool MSQTagSortItem::isGrouped()
{
    return mGroup->isChecked();
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::setOrder(int order)
{
    if (order == 0)
        this->triggerAscendingOrder();
    else
        this->triggerDescendingOrder();
}

/***********************************************************************************//**
 *
 */
int MSQTagSortItem::order()
{
    if (mToolButton->text() == "Ascending")
        return 0;
    else
        return 1;
}

/***********************************************************************************//**
 *
 */
 void MSQTagSortItem::setModel(QAbstractItemModel *model)
{ 
   mLineEdit->setModel(model);
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::triggerAddTag()
{
    // emit add tag at index index
    emit addTriggered(this);
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::triggerRemoveTag()
{
    // emit remove tag at index
    emit removeTriggered(this);
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::triggerAscendingOrder()
{
    mToolButton->setText("Ascending");
}

/***********************************************************************************//**
 *
 */
void MSQTagSortItem::triggerDescendingOrder()
{
    mToolButton->setText("Descending");
}

/***********************************************************************************//**
 *
 */
const QString MSQTagSortItem::toString()
{
    QString string;
    gdcm::StringFilter filter;

    if (this->isGrouped())
        string.append("1 ");
    else
        string.append("0 ");

    string.append(QString::fromStdString(filter.ToString(this->tag())));
    string.append(QString("%1 %2\n").arg(this->alias()).arg(this->order()));

    return string;
}

/***********************************************************************************//**
 *
 */
 void MSQTagSortItem::createInterface()
 {
    QFont font;
    font.setPointSize(11);

    QGridLayout *layout = new QGridLayout(this);

    //QHBoxLayout *horizLayout = new QHBoxLayout;
    layout->setSpacing(2);
    layout->setContentsMargins(QMargins(0,0,0,0));
    //layout->setColumnMinimumWidth(2, 50);
    layout->setColumnStretch(2, 2);

    // set up completer
    //mCompleter = new QCompleter(this);
    //mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QLabel *descLabel = new QLabel(" Tag:");
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setFont(font);

    // line editor
    mLineEdit = new MSQDicomSearchLineEdit(this);
    mLineEdit->setPlaceholderText(tr("Search Tag"));
    mLineEdit->setContentsMargins(QMargins(0,0,2,2));
    //mLineEdit->setCompleter(mCompleter);
    mLineEdit->setFont(font);
    mLineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    //QObject::connect(mLineEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));

    QLabel *aliasLabel = new QLabel(" Alias:");
    aliasLabel->setAlignment(Qt::AlignCenter);
    aliasLabel->setFont(font);

    // alias
    mAlias = new QLineEdit(this);
    mAlias->setContentsMargins(QMargins(0,0,2,2));
    //mLineEdit->setCompleter(mCompleter);
    mAlias->setFont(font);
    mAlias->setAttribute(Qt::WA_MacShowFocusRect, false);

    // set alias 
    mLineEdit->setAlias(mAlias);

    //grouping
    mGroup = new QCheckBox(" ", this);
    mGroup->setChecked(true);

    // menu for ordering
    QMenu *menu = new QMenu(this);
    menu->setFont(font);
    QAction *actionAscending = new QAction("Ascending", this);
    QObject::connect(actionAscending, SIGNAL(triggered()), this, SLOT(triggerAscendingOrder()));
    QAction *actionDescending = new QAction("Descending", this);
    QObject::connect(actionDescending, SIGNAL(triggered()), this, SLOT(triggerDescendingOrder()));
    menu->addAction(actionAscending);
    menu->addAction(actionDescending);

     //add buttons
    QAction *actionAdd = new QAction(QIcon(":/images/add_normal.png"), tr("&Add..."), this);
    QObject::connect(actionAdd, SIGNAL(triggered()), this, SLOT(triggerAddTag()));

    QAction *actionRemove = new QAction(QIcon(":/images/remove_normal.png"), tr("&Remove"), this);
    QObject::connect(actionRemove, SIGNAL(triggered()), this, SLOT(triggerRemoveTag()));

    mToolButton = new QToolButton(this);
    mToolButton->setFont(font);
    mToolButton->setText("Ascending");
    mToolButton->setMenu(menu);
    mToolButton->setPopupMode(QToolButton::InstantPopup);

    // create add and remove item buttons
    mToolBar = new QToolBar(this);
    mToolBar->setIconSize(QSize(15,15));
    mToolBar->setContentsMargins(QMargins(0,0,0,0));
    mToolBar->setStyleSheet("spacing: 0px");
    
    mToolBar->addWidget(mToolButton);
    mToolBar->addAction(actionRemove);
    mToolBar->addAction(actionAdd);

    QFrame* line = new QFrame(this);
    line->setGeometry(QRect());
    line->setFrameShape(QFrame::HLine); // Replace by VLine for vertical line
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);

    layout->addWidget(line, 0, 0, 1, 6);
    layout->addWidget(mGroup, 1, 0);
    layout->addWidget(descLabel, 1, 1);
    layout->addWidget(mLineEdit, 1, 2);
    layout->addWidget(aliasLabel, 1, 3);
    layout->addWidget(mAlias, 1, 4);
    layout->addWidget(mToolBar, 1, 5);
    //layout->addWidget(line, 1, 0, 1, 3);
   
    this->setLayout(layout);
 }

