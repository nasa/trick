#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QList>
#include <QDebug>

#include "bookidxview.h"
#include "plottitleview.h"
#include "cornerview.h"
#include "linedrulerview.h"
#include "labeledrulerview.h"
#include "curvesview.h"
#include "libsnapdata/trickcurvemodel.h"

class PlotView : public BookIdxView
{
    Q_OBJECT

public:
    explicit PlotView(QWidget *parent = 0);

protected:
    virtual void _update();

protected:
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

private:
    QGridLayout* _grid;
    PlotTitleView* _titleView ;
    CornerView* _tlCorner;
    CornerView* _trCorner;
    CornerView* _blCorner;
    CornerView* _brCorner;
    CurvesView* _curvesView;
    LinedRulerView* _tTics;
    LinedRulerView* _bTics;
    LinedRulerView* _rTics;
    LinedRulerView* _lTics;
    LabeledRulerView* _xTicLabels;
    LabeledRulerView* _yTicLabels;

    QList<QAbstractItemView*> _childViews;

protected slots:
    /*
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    */
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    //virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    //virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

};

#endif // PLOTVIEW_H
