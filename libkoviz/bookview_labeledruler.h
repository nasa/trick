#ifndef LABELEDRULERVIEW_H
#define LABELEDRULERVIEW_H

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QFontInfo>
#include "bookidxview.h"

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

    QString _format(double tic) const;

    QSize _sizeHintLeft() const;
    QSize _sizeHintBottom() const;

    QRect _boundingRect(const QString& strVal) const;
    void _paint10Exponent(const LabelBox &box) const;

signals:

public slots:

};

#endif // LABELEDRULERVIEW_H
