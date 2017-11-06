#ifndef PAGETITLEVIEW_H
#define PAGETITLEVIEW_H

#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QPainter>

#include "bookidxview.h"

class PageTitleView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PageTitleView(QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent * event);

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight,
                             const QVector<int> &roles = QVector<int>());
};

#endif // PAGETITLEVIEW_H
