#include "bookview_page.h"

PageView::PageView(QWidget *parent) :
    BookIdxView(parent),
    _grid(0),
    _toggleSingleView(true)
{
    setFrameShape(QFrame::NoFrame);

    // Create/configure gridlayout for page
    _grid = new PageLayout;

    // Create and add titleView
    PageTitleView* titleView = new PageTitleView(this);
    _childViews << titleView;
    _grid->addWidget(titleView);

    setLayout(_grid);
}

void PageView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPalette pal = palette();

    if (_bookModel()->isChildIndex(rootIndex(),"Page","PageBackgroundColor")) {

        QString bgColor = _bookModel()->getDataString(rootIndex(),
                                                      "PageBackgroundColor",
                                                      "Page");
        if ( !bgColor.isEmpty() ) {
            QColor bg(bgColor);
            pal.setColor(QPalette::Base,bg);
        }
    }

    if (_bookModel()->isChildIndex(rootIndex(),"Page","PageForegroundColor")) {

        QString fgColor = _bookModel()->getDataString(rootIndex(),
                                                      "PageForegroundColor",
                                                      "Page");
        if ( !fgColor.isEmpty() ) {
            QColor fg(fgColor);
            pal.setColor(QPalette::Text,fg);
        }
    }

    setPalette(pal);
}

bool PageView::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::MouseButtonRelease ) {
        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if ( mouseEvent ) {
            if (  mouseEvent->button() == Qt::LeftButton ) {
                // Toggle between single/multi view
                _toggleView(obj);
            }
        }

    }
    viewport()->update();
    return false;
}

void PageView::_toggleView(QObject* obj)
{
    if ( _toggleSingleView ) {
        _toggleSingleView = false;
        for ( int i = 1; i < _grid->count(); ++i ) {
            QWidget* w = _grid->itemAt(i)->widget();
            if ( w == obj ) {
                w->show();
            } else {
                w->hide();
            }
        }
    } else {
        _toggleSingleView = true;
        for ( int i = 1; i < _grid->count(); ++i ) {
            QWidget* w = _grid->itemAt(i)->widget();
            w->show();
        }

    }
}

void PageView::dataChanged(const QModelIndex &topLeft,
                           const QModelIndex &bottomRight,
                           const QVector<int> &roles)
{
    Q_UNUSED(roles);

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
        fprintf(stderr,"koviz [bad scoobs]: PageView::rowsInserted(). "
                       "Attempting to insert more than a single row.\n");
        exit(-1);
    }

    int nPlots = model()->rowCount(pidx);

    if ( start != nPlots-1 || end != nPlots-1 ) {
        fprintf(stderr,"koviz [bad scoobs]: PageView::rowsInserted(). "
                       "Attempting to insert a row instead of appending it.\n");
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
    connect(plot, SIGNAL(signalDropEvent(QDropEvent*,QModelIndex)),
            this, SLOT(slotDropEvent(QDropEvent*,QModelIndex)));
    _grid->addWidget(plot);
    plot->show();

    // Calling updateGeometry on child widgets will cause child widgets to
    // recalculate size via sizeHint().  Recalculating size is necessary for
    // some children like LabelRulerView which must be resized so that plot
    // y-axes are aligned vertically across plots on a page
    QList<QWidget *> widgets = this->findChildren<QWidget*>();
    foreach (QWidget* widget, widgets) {
        widget->updateGeometry();
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
    QString tag = model()->data(index).toString();
    if ( tag == "Page" && _bookModel() ) {
        _grid->setModelIndex(_bookModel(),index);
    }
    foreach (QAbstractItemView* view, _childViews ) {
        view->setRootIndex(index);
    }
    QAbstractItemView::setRootIndex(index);
}
