#ifndef CORNERVIEW_H
#define CORNERVIEW_H

#include <QPainter>
#include "bookidxview.h"
#include "layoutitem_plotcorner.h"

class CornerView : public BookIdxView
{
    Q_OBJECT
public:
    explicit CornerView(Qt::Corner corner, QWidget *parent = 0);
    virtual void setModel(QAbstractItemModel *model);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

private:
    Qt::Corner _corner;
};

#endif // CORNERVIEW_H
