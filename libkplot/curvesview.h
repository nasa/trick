#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include <QPainter>
#include <QHash>
#include <QList>
#include <QColor>
#include <QMouseEvent>
#include <QRubberBand>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QSizeF>
#include <stdlib.h>
#include "bookidxview.h"
#include "libkplot/unit.h"

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
    virtual void keyPressEvent(QKeyEvent *event);

private:
    QHash<TrickCurveModel*,QPainterPath*> _curve2path;
    QPainterPath* _errorPath;
    QString _viewType; // error,coplot,error+coplot
    QPainterPath *_createPainterPath(TrickCurveModel *curveModel);
    QPainterPath* _createErrorPath(const QModelIndex& curve0Idx,
                                   const QModelIndex &curve1Idx);
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

    double _xScale(TrickCurveModel *curveModel,
                   const QModelIndex& curveIdx=QModelIndex()) const;
    double _yScale(TrickCurveModel *curveModel,
                   const QModelIndex& curveIdx=QModelIndex()) const;

    QRectF _curveBBox(TrickCurveModel *curveModel,
                      const QModelIndex &curveIdx) const ;
    QRectF _calcBBox() const ;

    void _paintCoplot(const QTransform& T,QPainter& painter,QPen& pen);
    void _paintErrorplot(const QTransform& T,QPainter& painter,QPen& pen);

    // Key Events
    void _keyPressSpace();

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);


};

#endif // CURVESVIEW_H
