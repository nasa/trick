#ifndef LINEDRULERVIEW_H
#define LINEDRULERVIEW_H

#include <QPainter>

#include "bookidxview.h"

class LinedRulerView : public BookIdxView
{
    Q_OBJECT
public:
    explicit LinedRulerView(Qt::Alignment alignment, QWidget *parent = 0);

protected:
    virtual void _update();

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

private:
    Qt::Alignment _alignment;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);


};

#endif // LINEDRULERVIEW_H
