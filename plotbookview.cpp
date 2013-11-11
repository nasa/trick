#include "plotbookview.h"
#include <QDebug>

PlotBookView::PlotBookView(QWidget *parent) :
    QAbstractItemView(parent),
    _monteModel(0)
{
    _nb = new QTabWidget(parent);
}

PlotBookView::~PlotBookView()
{
    // I think, not sure, the grids, plots frames are deleted with notebook
    // so nothing to do
}

void PlotBookView::setData(MonteModel *monteModel)
{
    _monteModel = monteModel;
}

//
// This is really bogus right now
//
QRect PlotBookView::visualRect(const QModelIndex &index) const
{
    QRect rect;
    QFrame* f = _frames.value(index);
    if ( f )  {
        rect = f->rect();
    }

    return rect;
#if 0
    if (rect.isValid())
        return QRect(rect.left() - horizontalScrollBar()->value(),
                     rect.top() - verticalScrollBar()->value(),
                     rect.width(), rect.height());
    else
        return rect;
#endif
}

void PlotBookView::scrollTo(const QModelIndex &index,
                            QAbstractItemView::ScrollHint hint)
{
    // TODO
    update();
}

QModelIndex PlotBookView::indexAt(const QPoint &point) const
{
    // TODO
    qDebug() << "indexAt!";
    return QModelIndex();
}

void PlotBookView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    connect(selectionModel,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(currentSelectChanged(QModelIndex,QModelIndex)));
    connect(_nb, SIGNAL(currentChanged(int)),
            this, SLOT(currentTabChanged(int)));
    QAbstractItemView::setSelectionModel(selectionModel);
}

QModelIndex PlotBookView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                     Qt::KeyboardModifiers modifiers)
{
    // TODO
    return QModelIndex();
}

int PlotBookView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int PlotBookView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool PlotBookView::isIndexHidden(const QModelIndex &index) const
{
    return false; // TODO
}

void PlotBookView::setSelection(const QRect &rect,
                                QItemSelectionModel::SelectionFlags command)
{
    qDebug() << "setSelection!!";
}

//
// This is really bogus right now
//
QRegion PlotBookView::visualRegionForSelection(
                               const QItemSelection &selection) const
{
    QModelIndexList idxs = selection.indexes();
    foreach ( QModelIndex idx, idxs ) {
        QFrame* f = _frames.value(idx);
        if ( f ) {
            return QRegion(f->rect());
        }
        break; // do first one for now
    }
    return QRegion();
}

//
// This is really bogus right now
//
QItemSelectionModel::SelectionFlags PlotBookView::selectionCommand(
                           const QModelIndex &index, const QEvent *event) const
{
    return QItemSelectionModel::Select;
}

void PlotBookView::setCurrentIndex(const QModelIndex &index)
{
    QAbstractItemView::setCurrentIndex(index);
}

//
// This is really bogus right now
//
void PlotBookView::dataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);
    viewport()->update();
}

void PlotBookView::currentSelectChanged(const QModelIndex &currIdx,
                                  const QModelIndex &prevIdx)
{
    QModelIndex pidx = model()->parent(currIdx);

    //
    // Update notebook widget index
    //
    int level = 0 ;
    QModelIndex pageIdx(currIdx);
    while ( pageIdx.parent().isValid() ) {
        pageIdx = pageIdx.parent();
        level++;
    }
    _nb->setCurrentIndex(_idx2nbIdx.value(pageIdx));

    //
    // If currIdx is a curve index, select curve
    //
    if ( level == 2 ) {
        TrickCurve* prevCurve = _curves.value(prevIdx);
        if ( prevCurve ) {
            prevCurve->setSelected(false);
        }
        TrickCurve* currCurve = _curves.value(currIdx);
        if ( currCurve ) {
            currCurve->setSelected(true);
            QModelIndex plotIdx = currIdx.parent();
            Plot* plot = _plots.value(plotIdx);
            plot->replot();
        }
    }
}

void PlotBookView::currentTabChanged(int currIdx)
{
    if ( ! selectionModel() ) {
        return;
    }

    QModelIndex idx = _nbidx2idx.value(currIdx);
    selectionModel()->setCurrentIndex(idx,QItemSelectionModel::ClearAndSelect);
}

void PlotBookView::currentCustomPlotCurveChanged(TrickCurve* curve)
{
    QModelIndex curveIdx = _curves.key(curve);
    selectionModel()->setCurrentIndex(curveIdx,
                                      QItemSelectionModel::ClearAndSelect);
}

void PlotBookView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    QModelIndex gpidx = model()->parent(pidx);
    QModelIndex g2pidx = model()->parent(gpidx);
    QModelIndex g3pidx = model()->parent(g2pidx);

    for ( int row = start; row < end+1; ++row) {
        QModelIndex idx = model()->index(row,0,pidx);
        if ( ! pidx.isValid() ) {
            // Page (below root idx)
            QString dpfile = model()->data(idx).toString();
            QFrame* frame = new QFrame;
            _frames .insert(idx,frame);
            QGridLayout* grid = new QGridLayout(frame);
            grid->setContentsMargins(0, 0, 0, 0);
            grid->setSpacing(0);
            _grids.insert(idx,grid);
            _nb->addTab(frame,QFileInfo(dpfile).baseName());
            int nbIdx = _nb->count()-1;
            _idx2nbIdx.insert(idx,nbIdx);
            _nbidx2idx.insert(nbIdx,idx);
            _nb->setCurrentIndex(nbIdx);
            _nb->setAttribute(Qt::WA_AlwaysShowToolTips, true);
        } else if ( ! gpidx.isValid() ) {
            // Plot
            QFrame* frame = _frames.value(pidx);
            QGridLayout* grid = _grids.value(pidx) ;
            Plot* plot = new Plot(idx,frame);
            _plots .insert(idx,plot);
            int nPlots = model()->rowCount(pidx);
            switch ( nPlots ) {
            case 1: {
                grid->addWidget(plot,0,0);
                break;
            }
            case 2: {
                grid->addWidget(plot,1,0);
                break;
            }
            case 3: {
                grid->addWidget(plot,2,0);
                break;
            }
            case 4: {
                QWidget* w2 = grid->itemAtPosition(1,0)->widget();
                QWidget* w3 = grid->itemAtPosition(2,0)->widget();
                grid->removeWidget(w2);
                grid->removeWidget(w3);
                grid->addWidget(w2,0,1);
                grid->addWidget(w3,1,0);
                grid->addWidget(plot,1,1);
                break;
            }
            case 5: {
                grid->addWidget(plot,2,0,1,2);
                break;
            }
            case 6: {
                QWidget* w5 = grid->itemAtPosition(2,0)->widget();
                grid->removeWidget(w5);
                grid->addWidget(w5,2,0);
                grid->addWidget(plot,2,1);
                break;
            }
            case 7: {
                grid->addWidget(plot,3,0,1,2);
                break;
            }
            default: {
                qDebug() << "snap limitation: 7 plots max on DP :(";
                qDebug() << "snap will probably crash now!";
            }
            }

        } else if ( ! g2pidx.isValid() ) {
            // Curve
        } else if ( ! g3pidx.isValid() ) {
            // t,x,y,run
            if ( idx.row() == 3 ) {
                // Run
                QModelIndex yidx = model()->index(2,0,pidx);
                QString yparam = model()->data(yidx).toString();
                TrickCurveModel* curveModel = _monteModel->curve(pidx.row(),
                                                                yparam);
                Plot* plot = _plots.value(gpidx);
                TrickCurve* curve = plot->addCurve(curveModel);
                _curves.insert(pidx,curve);
                connect(curve,SIGNAL(selectionChanged(TrickCurve*)),
                        this,SLOT(currentCustomPlotCurveChanged(TrickCurve*)));
            }
        }
    }
}

//
// This is a start.  I realized I should not depend on QModelIndexes
// as keys in private maps.  Going to keep this for now.
//
void PlotBookView::rowsAboutToBeRemoved(const QModelIndex &pidx,
                                        int start, int end)
{
    QModelIndex gpidx = model()->parent(pidx);
    QModelIndex g2pidx = model()->parent(gpidx);
    QModelIndex g3pidx = model()->parent(g2pidx);

    for ( int row = start; row < end+1; ++row) {

        QModelIndex idxToRemove = model()->index(row,0,pidx);

        if ( ! pidx.isValid() ) {
            // Page
        } else if ( ! gpidx.isValid() ) {
            // Plot
#if 0
            QFrame* frame = _frames.value(pidx);
            QGridLayout* grid = _grids.value(pidx) ;

            int nCurves = model()->rowCount(idxToRemove);

            model()->removeRows(0,nCurves,idxToRemove);

            Plot* plot = _plots.value(idxToRemove);
            grid->removeWidget(plot);
            delete plot;

            QList<QWidget*> plots;
            QWidget* p;
            int nPlots = grid->count();
            for ( int i = nPlots-1; i >= 0 ; --i ) {
                p = grid->takeAt(i)->widget();
                plots << p;
            }

            switch (nPlots)
            {
            case 0:
            {
                break;
            }
            case 1:
            {
                grid->addWidget(plots.at(0),0,0);
                break;
            }
            case 2:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                break;
            }
            case 3:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),0,2);
                break;
            }
            case 4:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                break;
            }
            case 5:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0,1,2);
                break;
            }
            case 6:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0);
                grid->addWidget(plots.at(4),2,1);
                break;
            }
            case 7:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0);
                grid->addWidget(plots.at(4),2,1);
                grid->addWidget(plots.at(0),3,0,1,2);
                break;
            }
            default:
            {
                qDebug() << "snap [error]: can't handle more than 7 plots!";
            }
            }
#endif

        } else if ( ! g2pidx.isValid() ) {
            // Curve
#if 0
            Plot* plot = _plots.value(pidx);
            TrickCurve* curve = _curves.value(idxToRemove);
            disconnect(curve,SIGNAL(selectionChanged(TrickCurve*)));
            plot->removePlottable(curve);
            plot->replot();
            _curves.remove(idxToRemove);
#endif
        } else if ( ! g3pidx.isValid() ) {
            // t,x,y,run
        }
    }
}
