#ifndef XAXISLABELVIEW_H
#define XAXISLABELVIEW_H

#include <QPainter>
#include <QString>
#include <QWheelEvent>
#include <QList>
#include "bookidxview.h"

class XAxisLabelView : public BookIdxView
{
    Q_OBJECT
public:
    explicit XAxisLabelView(QWidget *parent = 0);

protected:
    virtual void _update();

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void wheelEvent(QWheelEvent* e);

private:
    QString _xAxisLabelText() const;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);
};

#endif // XAXISLABELVIEW_H
