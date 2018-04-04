#ifndef PAGETITLEVIEW_H
#define PAGETITLEVIEW_H

#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QPainter>
#include <QSize>
#include <QSizePolicy>

#include "bookidxview.h"

class PageTitleView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PageTitleView(QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize sizeHint() const;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight,
                             const QVector<int> &roles = QVector<int>());
private:
    QSize _sizeLegend() const;
};

#endif // PAGETITLEVIEW_H
