#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include <QPainter>
#include <QHash>
#include "bookidxview.h"

class CurvesView : public BookIdxView
{
    Q_OBJECT

public:
    explicit CurvesView(QWidget *parent = 0);

protected:
    virtual void _update();

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

private:
    QHash<TrickCurveModel*,QPainterPath*> _curve2path;
    QPainterPath *_createPainterPath(TrickCurveModel *curveModel);
    QRectF _bbox();
    void _setPlotMathRect(const QRectF& M);
    QPainterPath _sinPath();
    QPainterPath _stepPath();

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);


};

#endif // CURVESVIEW_H
