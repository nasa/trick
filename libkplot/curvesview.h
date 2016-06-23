#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include <QPainter>
#include <QHash>
#include <QList>
#include <QColor>
#include <QMouseEvent>
#include <QRubberBand>
#include <QFocusEvent>
#include <QSizeF>
#include <stdlib.h>
#include "bookidxview.h"
#include "libsnapdata/unit.h"

class CurvesView : public BookIdxView
{
    Q_OBJECT

public:
    explicit CurvesView(QWidget *parent = 0);
    ~CurvesView();

protected:
    virtual void _update();

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* mouseEvent);

private:
    QHash<TrickCurveModel*,QPainterPath*> _curve2path;
    QPainterPath *_createPainterPath(TrickCurveModel *curveModel);
    QRectF _bbox();
    QPainterPath _sinPath();
    QPainterPath _stepPath();

    QList<QColor> _colorBandsNormal;
    QList<QColor> _colorBandsRainbow;
    QList<QColor> _createColorBands(int nBands, bool isRainbow);

    QRectF _mousePressMathRect;
    QPoint _mousePressPos;
    QPointF _mousePressMathTopLeft;

    void _updateUnits(const QModelIndex &curveIdx,QChar axis) const;

    QRectF _currBBox;

    void _insertPath(QPainterPath* path,
                     TrickCurveModel* curveModel,
                     const QModelIndex& curveIdx);

    double _xScale(TrickCurveModel *curveModel,
                   const QModelIndex& curveIdx=QModelIndex()) const;
    double _yScale(TrickCurveModel *curveModel,
                   const QModelIndex& curveIdx=QModelIndex()) const;

    QRectF _curveBBox(TrickCurveModel *curveModel,
                      const QModelIndex &curveIdx) const ;
    QRectF _calcBBox() const ;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);


};

#endif // CURVESVIEW_H
