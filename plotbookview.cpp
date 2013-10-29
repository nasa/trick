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

QRect PlotBookView::visualRect(const QModelIndex &index) const
{
    //QRect rect = itemRect(index);
    qDebug() << viewport()->rect();
    return viewport()->rect();
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
    qDebug() << "MOOOOOOOOOOOOOOOOOOOOOOOO";
    return QModelIndex();
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
}

QRegion PlotBookView::visualRegionForSelection(
                               const QItemSelection &selection) const
{
}

QItemSelectionModel::SelectionFlags PlotBookView::selectionCommand(
                           const QModelIndex &index, const QEvent *event) const
{
}

void PlotBookView::dataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight)
{
    //qDebug() << "dchanged=" << topLeft ;
    QAbstractItemView::dataChanged(topLeft, bottomRight);
    viewport()->update();
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
            _nb->setCurrentIndex(_nb->count()-1);
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
            default: break;
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
                plot->addCurve(curveModel);
            }
        }
    }
}
