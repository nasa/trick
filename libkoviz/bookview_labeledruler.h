#ifndef LABELEDRULERVIEW_H
#define LABELEDRULERVIEW_H

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QFontInfo>
#include "bookidxview.h"
#include "layoutitem_ticlabels.h"


class LabeledRulerView : public BookIdxView
{
    Q_OBJECT
public:
    explicit LabeledRulerView(Qt::Alignment alignment, QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void wheelEvent(QWheelEvent *e);
};

#endif // LABELEDRULERVIEW_H
