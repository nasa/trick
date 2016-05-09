#ifndef CORNERVIEW_H
#define CORNERVIEW_H

#include <QPainter>
#include "bookidxview.h"

class CornerView : public BookIdxView
{
    Q_OBJECT
public:
    explicit CornerView(Qt::Corner corner, QWidget *parent = 0);

protected:
    virtual void _update();

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

private:
    Qt::Corner _corner;

protected slots:
    /*
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    */

signals:
    
public slots:
    
};

#endif // CORNERVIEW_H
