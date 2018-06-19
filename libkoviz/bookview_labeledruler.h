#ifndef LABELEDRULERVIEW_H
#define LABELEDRULERVIEW_H

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QFontInfo>
#include "bookidxview.h"

struct LabelBox
{
    QPointF center; // preferred center
    QString strVal;
    QRectF  bb;     // actual center is center of bb
};

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

private:
    Qt::Alignment _alignment;

    const int _margin5;

    bool _isFits(const QList<LabelBox>& boxes,
                 const QRectF& W,
                 double minGap, Qt::Alignment alignment);

    QString _format(double tic) const;

    QList<LabelBox> _moveCurrBoxToFit(const QList<LabelBox>& boxes,
                         int currBoxIdx,
                         int lastBoxIdx,
                         const QRectF& W,
                         double minGap);

    QList<LabelBox> _halfBoxSet(const QList<LabelBox>& boxes,
                                Qt::Alignment alignment);

    QSize _sizeHintLeft() const;
    QSize _sizeHintBottom() const;

signals:

public slots:

};

#endif // LABELEDRULERVIEW_H
