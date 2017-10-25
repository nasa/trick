#ifndef PAGETITLEVIEW_H
#define PAGETITLEVIEW_H

#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QFontMetrics>
#include <QPainter>

#include "bookidxview.h"

class PageTitleView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PageTitleView(QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent * event);

};

#endif // PAGETITLEVIEW_H
