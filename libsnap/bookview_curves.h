#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include <QtGlobal>
#include <QVector>
#include <QPolygonF>
#include <QPainter>
#include <QHash>
#include <QList>
#include <QColor>
#include <QMouseEvent>
#include <QRubberBand>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QSizeF>
#include <QLineF>
#include <QItemSelectionModel>
#include <stdlib.h>
#include <float.h>
#include "bookidxview.h"
#include "libsnap/unit.h"

class CurvesView : public BookIdxView
{
    Q_OBJECT

public:
    explicit CurvesView(QWidget *parent = 0);
    ~CurvesView();

public:
    virtual void setCurrentCurveRunID(int runID);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* mouseEvent);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void currentChanged(const QModelIndex& current,
                                const QModelIndex& previous);


private:
    QHash<TrickCurveModel*,QPainterPath*> _curve2path;
    QPainterPath* _errorPath;
    QPainterPath* _createPainterPath(TrickCurveModel *curveModel);
    QPainterPath* _createErrorPath(const QModelIndex& curveIdx0,
                                   const QModelIndex &curveIdx1);
    QRectF _bbox();
    QPainterPath _sinPath();
    QPainterPath _stepPath();

    QList<QColor> _colorBandsNormal;
    QList<QColor> _colorBandsRainbow;
    QList<QColor> _createColorBands(int nBands, bool isRainbow);

    QRectF _mousePressMathRect;
    QPoint _mousePressPos;
    QPointF _mousePressMathTopLeft;
    QModelIndex _mousePressCurrentIndex;
    bool _isMouseDoubleClick;

    void _updateAxisLabelUnits(const QModelIndex &curveIdx,QChar axis) const;

    QRectF _currBBox;

    QRectF _curveBBox(TrickCurveModel *curveModel,
                      const QModelIndex &curveIdx) const ;
    QRectF _calcBBox() const ;

    void _paintCoplot(const QTransform& T,QPainter& painter,QPen& pen);
    void _paintErrorplot(QPainter& painter, const QPen &pen,
                         QPainterPath *errorPath,
                         const QModelIndex &plotIdx);
    void _paintCurve(const QModelIndex& curveIdx,
                     const QTransform &T, QPainter& painter, QPen& pen);
    void _paintCoordArrow(const QPointF& coord, QPainter &painter);



    QList<QModelIndex> _curvesInsideMouseRect(const QRectF& R);

    // Key Events
    void _keyPressSpace();
    void _keyPressUp();
    void _keyPressDown();

    QPointF _liveCoord;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);


};

#endif // CURVESVIEW_H
