#ifndef LINEDRULERVIEW_H
#define LINEDRULERVIEW_H

#include <QPainter>

#include "bookidxview.h"
#include "layoutitem_plottics.h"

class LinedRulerView : public BookIdxView
{
    Q_OBJECT
public:
    explicit LinedRulerView(Qt::Alignment alignment, QWidget *parent = 0);

protected:
    virtual void setModel(QAbstractItemModel* model);
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);


};

#endif // LINEDRULERVIEW_H
