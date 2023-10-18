#ifndef BOOKIDXVIEW_H
#define BOOKIDXVIEW_H

#include <QAbstractItemView>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEngine>
#include <QPen>
#include <QEasingCurve>
#include <QDropEvent>
#include <qmath.h>
#include <cmath>
#include "bookmodel.h"
#include "unit.h"

struct LabelBox
{
    QPointF center; // preferred center
    QString strVal;
    QRectF  bb;     // actual center is center of bb
};

class BookIdxView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit BookIdxView(QWidget *parent = 0);

public:
    void setCurvesView(QAbstractItemView* view);
    virtual void setCurrentCurveRunID(int runID);

public:
    virtual void setModel(QAbstractItemModel *model);
    virtual void setRootIndex(const QModelIndex &index);
    virtual QModelIndex indexAt( const QPoint& point) const;
    virtual QRect visualRect(const QModelIndex &index) const;
    virtual void scrollTo(const QModelIndex &index,
                          ScrollHint hint = EnsureVisible);

    // Necessary for ignoring events from middle mouse for rubberbanding
    // in PlotView.  If events are not ignored, the event filtering will
    // not catch mouse events.  This keeps from doing a
    // qApp->installEventFilter which would have to filter *all* app events
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    QTransform _coordToPixelTransform() ;

    QRectF _mathRect() ;
    QModelIndex _plotMathRectIdx(const QModelIndex& plotIdx) const;

protected:
    virtual QModelIndex moveCursor(CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers);
    virtual int horizontalOffset() const;
    virtual int verticalOffset() const;
    virtual bool isIndexHidden(const QModelIndex &index) const;
    virtual void setSelection(const QRect &rect,
                              QItemSelectionModel::SelectionFlags command);
    virtual QRegion visualRegionForSelection(
                              const QItemSelection &selection) const;
    Qt::Alignment _alignment;

protected:
    PlotBookModel *_bookModel() const;
    QAbstractItemView* _curvesView;

    Qt::MouseButton _buttonSelectAndPan;
    Qt::MouseButton _buttonRubberBandZoom;
    Qt::MouseButton _buttonResetView;

    QPoint _mousePressPos;
    QRectF _mousePressMathRect;

    QString _curvesXUnit(const QModelIndex& plotIdx) const;
    QString _curvesUnit(const QModelIndex& plotIdx, QChar axis) const;

    void _paintCurvesLegend(const QRect &R,
                            const QModelIndex& curvesIdx, QPainter &painter);
    void __paintCurvesLegend(const QRect &R,
                             const QModelIndex& curvesIdx,
                             const QList<QPen*>& pens,
                             const QStringList& symbols,
                             const QStringList& labels,
                             QPainter& painter);
    virtual void __paintSymbol(const QPointF &p, const QString& symbol,
                               QPainter& painter);
    void _paintGrid(QPainter& painter, const QModelIndex &plotIdx);

    QList<QAbstractItemView*> _childViews;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    //virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    //virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);


signals:
    void signalDropEvent(QDropEvent* dropEvent, const QModelIndex& idx);

public slots:
    void slotDropEvent(QDropEvent* dropEvent, const QModelIndex& idx);

};

#endif // BOOKIDXVIEW_H
