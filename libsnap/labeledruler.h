#ifndef LABELEDRULER_H
#define LABELEDRULER_H

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QList>
#include <QTransform>
#include <QStandardItemModel>
#include "kplotmodel.h"

struct LabelBox1
{
    QPointF center; // preferred center
    QString strVal;
    QRectF  bb;     // actual center is center of bb
};

class LabeledRuler : public QWidget
{
    Q_OBJECT
public:
    explicit LabeledRuler(KPlotModel *plotModel,
                          Qt::Alignment alignment,
                          QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

private:
    KPlotModel* _plotModel;
    Qt::Alignment _alignment;

    bool _isFits(const QList<LabelBox1>& boxes,
                 const QRectF& W,
                 double minGap, Qt::Alignment alignment);

    QString _format(double tic) const;

    QList<LabelBox1> _moveCurrBoxToFit(const QList<LabelBox1>& boxes,
                         int currBoxIdx,
                         int lastBoxIdx,
                         const QRectF& W,
                         double minGap);

    QList<LabelBox1> _halfBoxSet(const QList<LabelBox1>& boxes,
                                Qt::Alignment alignment);

signals:
    
public slots:
    
};

#endif // LABELEDRULER_H
