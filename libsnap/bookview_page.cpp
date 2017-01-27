#include "bookview_page.h"

PageView::PageView(QWidget *parent) :
    BookIdxView(parent),
    _grid(0),
    _toggleSingleView(true)
{
    setFrameShape(QFrame::NoFrame);

    // Create/configure gridlayout for page
    _grid = new QGridLayout;
    _grid->setContentsMargins(0, 0, 5, 0);
    _grid->setSpacing(0);

    // Create and add titleView
    PageTitleView* titleView = new PageTitleView(this);
    _childViews << titleView;
    _grid->addWidget(titleView,0,0,1,100);
    _grid->setRowStretch(0,1);

    setLayout(_grid);
}

bool PageView::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::MouseButtonRelease ) {
        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if ( mouseEvent ) {
            if (  mouseEvent->button() == Qt::LeftButton ) {
                Qt::KeyboardModifiers keymods = mouseEvent->modifiers();
                bool isShift = false;
                if ( keymods & Qt::ShiftModifier ) {
                    isShift = true;
                }
                if ( !currentIndex().isValid() || isShift ) {
                    // Toggle between single/multi view when:
                    //   1. Mouse clicked into white space and
                    //      there is no "selected" curve
                    //      If a curve is selected and mouse is
                    //      clicked into white space, the curve
                    //      is deselected (in curves view)
                    //   2. Shift key held down when mouse clicked
                    _toggleView(obj);
                }
            }
        }

    }
    viewport()->update();
    return false;
}

void PageView::_toggleView(QObject* obj)
{
    int r, c, rSpan, cSpan;
    int row = -1;
    int col = -1;
    if ( _toggleSingleView ) {
        _toggleSingleView = false;
        for ( int i = 1; i < _grid->count(); ++i ) {
            QWidget* w = _grid->itemAt(i)->widget();
            _grid->getItemPosition(i, &r, &c, &rSpan, &cSpan);
            if ( w == obj ) {
                row = r;
                col = c;
                _grid->setRowStretch(r,100);
                _grid->setColumnStretch(c,100);
            } else {
                if ( r != row ) _grid->setRowStretch(r,1);
                if ( c != col ) _grid->setColumnStretch(c,1);
                w->hide();
            }
        }
    } else {
        _toggleSingleView = true;
        for ( int i = 1; i < _grid->count(); ++i ) {
            QWidget* w = _grid->itemAt(i)->widget();
            _grid->getItemPosition(i, &r, &c, &rSpan, &cSpan);
            _grid->setRowStretch(r,100);
            _grid->setColumnStretch(c,100);
            w->show();
        }

    }
}

void PageView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    QModelIndex pidx = topLeft.parent();
    if ( pidx != bottomRight.parent() ) return;

    // TODO: For now and only handle single item changes
    if ( topLeft != bottomRight ) return;

    // Value is in column 1
    if ( topLeft.column() != 1 ) return;

#if 0
    int row = topLeft.row();
    QModelIndex idx0 = model()->index(row,0,pidx);
    if ( model()->data(idx0).toString() == "PageTitle" ) {
        QString title = model()->data(topLeft).toString();
        _title1->setText(title);
    }
#endif
}

// TODO: this will die if not appending plots one by one
void PageView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( model()->data(pidx).toString() != "Plots" ) return;

    QModelIndex pageIdx = pidx.parent();
    if ( pageIdx != rootIndex() ) return;

    int nPlotsToAdd = end-start+1;
    if ( nPlotsToAdd != 1 ) {
        qDebug() << "snap [bad scoobs]: PageView::rowsInserted(). "
                    "Attempting to insert more than a single row.";
        exit(-1);
    }

    int nPlots = model()->rowCount(pidx);

    if ( start != nPlots-1 || end != nPlots-1 ) {
        qDebug() << "snap [bad scoobs]: PageView::rowsInserted(). "
                    "Attempting to insert a row instead of appending it.";
        exit(-1);
    }

    QModelIndex plotIdx = model()->index(start,0,pidx);
    PlotView* plot = new PlotView(this);
    _childViews << plot;
    plot->setModel(model());
    plot->setRootIndex(plotIdx);
    plot->installEventFilter(this);  // for double clicking a plot to view alone
    connect(plot->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(_plotViewCurrentChanged(QModelIndex,QModelIndex)));

    switch ( nPlots ) {
    case 1: {
        _grid->addWidget(plot,1,0);
        _grid->setRowStretch(1,100);
        break;
    }
    case 2: {
        _grid->addWidget(plot,2,0);
        _grid->setRowStretch(2,100);
        break;
    }
    case 3: {
        _grid->addWidget(plot,3,0);
        _grid->setRowStretch(3,100);
        break;
    }
    case 4: {
        QWidget* w2 = _grid->itemAtPosition(2,0)->widget();
        QWidget* w3 = _grid->itemAtPosition(3,0)->widget();
        _grid->removeWidget(w2);
        _grid->removeWidget(w3);
        _grid->addWidget(w2,1,1);
        _grid->addWidget(w3,2,0);
        _grid->addWidget(plot,2,1);
        _grid->setRowStretch(3,0);
        break;
    }
    case 5: {
        _grid->addWidget(plot,3,0,1,2);
        _grid->setRowStretch(3,100);
        break;
    }
    case 6: {
        QWidget* w2 = _grid->itemAtPosition(1,1)->widget();
        QWidget* w3 = _grid->itemAtPosition(2,0)->widget();
        QWidget* w4 = _grid->itemAtPosition(2,1)->widget();
        QWidget* w5 = _grid->itemAtPosition(3,0)->widget();
        _grid->removeWidget(w2);
        _grid->removeWidget(w3);
        _grid->removeWidget(w4);
        _grid->removeWidget(w5);
        _grid->addWidget(w2,2,0);
        _grid->addWidget(w3,3,0);
        _grid->addWidget(w4,1,1);
        _grid->addWidget(w5,2,1);
        _grid->addWidget(plot,3,1);
        break;
    }
    case 7: {
        _grid->addWidget(plot,4,0,1,2);
        _grid->setRowStretch(4,100);
        break;
    }
    default: {
        qDebug() << "snap [current limitation]: 7 plots max on DP :(";
        qDebug() << "Aborting!!!";
        exit(-1);
    }
    }

}

void PageView::_plotViewCurrentChanged(const QModelIndex &currIdx,
                                       const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    setCurrentIndex(currIdx);
}

void PageView::setModel(QAbstractItemModel *model)
{
    foreach (QAbstractItemView* view, _childViews ) {
        view->setModel(model);
        BookIdxView* bview = dynamic_cast<BookIdxView*>(view);
        if ( bview ) {
            bview->setCurvesView(_curvesView);
        }
    }
    QAbstractItemView::setModel(model);
}

void PageView::setRootIndex(const QModelIndex &index)
{
    foreach (QAbstractItemView* view, _childViews ) {
        view->setRootIndex(index);
    }
    QAbstractItemView::setRootIndex(index);
}
