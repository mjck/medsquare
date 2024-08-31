#ifndef MSQ_IMAGE_MANAGER_WIDGET_H
#define MSQ_IMAGE_MANAGER_WIDGET_H

#include <QtGui>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>

class MedSquare;

class MSQImageManagerWidget : public QWidget
{
Q_OBJECT

public:
  MSQImageManagerWidget(MedSquare *medsquare);
  ~MSQImageManagerWidget();

  // sets image input to widget
  void setInput();

public slots:
  void showContextMenu(const QPoint& pos);
  void itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
  QTreeWidget *infoTree;

  MedSquare* medsquare;

  void buildWidget();
  void updateWidget();
  int findImageIndex(const QPoint& pos);
  QMenu *createColormapMenu(const QPoint& pos);
};

#endif
