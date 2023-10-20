#include "bookview.h"

BookView::BookView(QWidget *parent) :
    BookIdxView(parent)
{
    _mainLayout = new QVBoxLayout;

    _nb = new BookViewTabWidget(this);
    _nb->setTabsClosable(true);
    _nb->setFocusPolicy(Qt::StrongFocus);
    _nb->setMovable(true);
    _nb->setAcceptDrops(true);

    connect(_nb,SIGNAL(tabCloseRequested(int)),
            this,SLOT(_nbCloseRequested(int)));

    connect(_nb,SIGNAL(signalDropEvent(QDropEvent*,QModelIndex)),
            this, SLOT(slotDropEvent(QDropEvent*,QModelIndex)));

    _mainLayout->addWidget(_nb);

    setLayout(_mainLayout);
}

void BookView::setModel(PlotBookModel *model)
{
    _nb->setBookModel(model);
    BookIdxView::setModel(model);
}


void BookView::currentChanged(const QModelIndex &current,
                              const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if ( _bookModel()->isIndex(current,"Page") ) {
        QString pageName = _bookModel()->getDataString(current,
                                                       "PageName","Page");
        pageName = pageName.split(":").at(0);
        QFileInfo fi(pageName);
        pageName = fi.fileName();
        for (int i = 0; i < _nb->count(); ++i) {
            if ( _nb->tabText(i) == pageName ) {
                _nb->setCurrentIndex(i);
                break;
            }
        }
    } else if ( _bookModel()->isIndex(current,"Table") ) {
        QString tableName = _bookModel()->getDataString(current,
                                                       "TableName","Table");
        tableName = tableName.split(":").at(0);
        QFileInfo fi(tableName);
        tableName = fi.fileName() + ".table";
        for (int i = 0; i < _nb->count(); ++i) {
            if ( _nb->tabText(i) == tableName ) {
                _nb->setCurrentIndex(i);
                break;
            }
        }
    }
}

void BookView::selectionChanged(const QItemSelection &selected,
                                const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

int BookView::_modelIdxToTabId(const QModelIndex &idx)
{
    int tabId = -1;

    QString name;
    QString tag = model()->data(idx).toString();
    if ( tag == "Page" ) {
        name = _bookModel()->getDataString(idx,"PageName","Page");
    } else if ( tag == "Table" ) {
        name = _bookModel()->getDataString(idx,"TableName","Table");
    } else {
        fprintf(stderr,"koviz [bad scoobs]:1: BookView::_modelIdxToTabId\n");
        exit(-1);
    }

    int nTabs = _nb->count();
    for ( int i = 0; i < nTabs; ++i ) {

        QString tabToolTip = _nb->tabToolTip(i);
        if ( tabToolTip == name ) {
            tabId = i;
            break;
        }
    }

    if ( tabId == -1 ) {
        fprintf(stderr,"koviz [bad scoobs]:2: BookView::_modelIdxToTabId()\n");
        exit(-1);
    }

    return tabId;
}

QModelIndex BookView::_tabIdToModelIdx(int tabId)
{
    QModelIndex idx;

    QString tabToolTip = _nb->tabToolTip(tabId);
    QString wt = _nb->tabWhatsThis(tabId);

    QModelIndex pidx;
    if ( wt == "Page" ) {
        pidx = _bookModel()->getIndex(QModelIndex(), "Pages");
    } else if ( wt == "Table") {
        pidx = _bookModel()->getIndex(QModelIndex(), "Tables");
    }

    int rc = model()->rowCount(pidx);
    for ( int i = 0; i < rc ; ++i ) {
        QString name;
        if ( wt == "Page") {
            QModelIndex pageIdx = model()->index(i,0,pidx);
            name = _bookModel()->getDataString(pageIdx,"PageName","Page");
        } else if ( wt == "Table" ) {
            QModelIndex tableIdx = model()->index(i,0,pidx);
            name = _bookModel()->getDataString(tableIdx,"TableName","Table");
        }
        if ( name == tabToolTip ) {
            idx = model()->index(i,0,pidx);
            break;
        }
    }

    if ( !idx.isValid() ) {
        fprintf(stderr,"koviz [bad scoobs]:1: BookView::_tabIdToPageIdx() "
                       "Could not find page/table using tabId=%d\n", tabId);
        exit(-1);
    }

    return idx;
}


void BookView::savePdf(const QString &fname)
{
    //
    // Setup printer
    //
    QPrinter printer(QPrinter::HighResolution);
    printer.setCreator("Koviz");
    printer.setDocName(fname);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::Letter);
    printer.setOutputFileName(fname);
    QString orient = _bookModel()->getDataString(QModelIndex(),"Orientation");
    if ( orient == "landscape" ) {
        printer.setOrientation(QPrinter::Landscape);
    } else if ( orient == "portrait" ) {
        printer.setOrientation(QPrinter::Portrait);
    } else {
        fprintf(stderr, "koviz [bad scoobs]: savePdf! Aborting!\n");
        exit(-1);
    }

    //
    // Begin Printing
    //
    QPainter painter;
    if (! painter.begin(&printer)) {
        return;
    }
    painter.save();

    //
    // Set pen
    //
    QPen pen((QColor(Qt::black)));
    double pointSize = printer.logicalDpiX()/72.0;
    pen.setWidthF(pointSize);
    painter.setPen(pen);

    //
    // Print pages
    //
    bool isFirst = true;
    int nTabs = _nb->count();
    for ( int i = 0; i < nTabs; ++i) {

        QModelIndex idx = _tabIdToModelIdx(i);
        QString tag = model()->data(idx).toString();
        if ( tag != "Page") {
            continue; // only print pages (i.e. not tables)
        }
        if ( isFirst ) {
            isFirst = false;
        } else {
            printer.newPage();
        }
        _printPage(&painter,idx);
    }

    //
    // End printing
    //
    painter.restore();
    painter.end();
}

void BookView::saveJpg(const QString &fname)
{
    QWidget* page = _nb->currentWidget();
    int    image_dpi = page->logicalDpiX()*2; // arbitrary factor 2 for highdef
    double image_width_inches =(double)page->size().width()/page->logicalDpiX();
    double k = (double)page->size().height()/(double)page->size().width();
    double image_height_inches = k*image_width_inches;
    QSize size = page->size();
    size.setWidth(image_width_inches*image_dpi);
    size.setHeight(image_height_inches*image_dpi);
    QPixmap pixmap(size);
    pixmap.setDevicePixelRatio(image_dpi/page->logicalDpiX());

    // Begin Printing
    QPainter painter;
    if (! painter.begin(&pixmap)) {
        return;
    }
    painter.save();

    // Print current page onto pixmap
    QModelIndex pageIdx = _tabIdToModelIdx(_nb->currentIndex());
    _printPage(&painter,pageIdx);

    // Save the pixmap to a *.jpg
    pixmap.save(fname,"JPG");

    // End painting
    painter.restore();
    painter.end();
}

void BookView::_printPage(QPainter *painter, const QModelIndex& pageIdx)
{
    QPaintDevice* paintDevice = painter->device();
    if ( !paintDevice ) return;

    painter->save();

    // Foreground
    QPen origPen = painter->pen();
    QColor fg = _bookModel()->pageForegroundColor(pageIdx);
    QPen pagePen = painter->pen();
    pagePen.setColor(fg);
    painter->setPen(pagePen);

    // Background
    QColor bg = _bookModel()->pageBackgroundColor(pageIdx);
    painter->fillRect(QRect(0,0,paintDevice->width(),paintDevice->height()),bg);

    // Load page and plot layouts
    PageLayout pageLayout;
    pageLayout.setModelIndex(_bookModel(),pageIdx);
    PageTitleLayoutItem* pageTitleLayoutItem = new PageTitleLayoutItem(
                                                  _bookModel(),pageIdx,
                                                  painter->font());
    pageLayout.addItem(pageTitleLayoutItem);
    QModelIndex plotsIdx = _bookModel()->getIndex(pageIdx,"Plots", "Page");
    QModelIndexList plotIdxs = _bookModel()->plotIdxs(pageIdx);
    int nPlots = model()->rowCount(plotsIdx);
    QHash<QLayout*,QRectF> plotlayout2mathrect;
    QFontMetrics fm = painter->fontMetrics();
    QFont font8 = painter->font();
    font8.setPointSizeF(8);
    QFontMetrics fm8(font8);
    for ( int i = 0; i < nPlots; ++i ) {
        PlotLayout* plotLayout = new PlotLayout;
        pageLayout.addItem(plotLayout);

        // PlotLayout expects items to be added in a specific order!!!
        QModelIndex plotIdx = plotIdxs.at(i);
        QString plotRatio = _bookModel()->getDataString(plotIdx,
                                                        "PlotRatio","Plot");
        plotLayout->setPlotRatio(plotRatio);
        QRectF M = _bookModel()->getPlotMathRect(plotIdx);
        plotlayout2mathrect.insert(plotLayout,M);
        QLayoutItem* item = new YAxisLabelLayoutItem(fm,_bookModel(),plotIdx);
        plotLayout->addItem(item);  // yAxisLabel
        item = new TicLabelsLayoutItem(fm,fm8,_bookModel(),plotIdx);
        item->setAlignment(Qt::AlignLeft);
        plotLayout->addItem(item);  // yTicLabels
        item = new PlotTitleLayoutItem(fm,_bookModel(),plotIdx);
        plotLayout->addItem(item);  // plottitle
        item = new CurvesLayoutItem(_bookModel(),plotIdx,0);
        plotLayout->addItem(item);  // curves
        item = new PlotCornerLayoutItem(fm,Qt::TopLeftCorner);
        plotLayout->addItem(item);  // tlcorner
        item = new PlotCornerLayoutItem(fm,Qt::TopRightCorner);
        plotLayout->addItem(item);  // trcorner
        item = new PlotCornerLayoutItem(fm,Qt::BottomRightCorner);
        plotLayout->addItem(item);  // brcorner
        item = new PlotCornerLayoutItem(fm,Qt::BottomLeftCorner);
        plotLayout->addItem(item);  // blcorner
        item = new PlotTicsLayoutItem(fm,_bookModel(),plotIdx);
        item->setAlignment(Qt::AlignLeft);
        plotLayout->addItem(item);  // ltics
        item = new PlotTicsLayoutItem(fm,_bookModel(),plotIdx);
        item->setAlignment(Qt::AlignTop);
        plotLayout->addItem(item);  // ttics
        item = new PlotTicsLayoutItem(fm,_bookModel(),plotIdx);
        item->setAlignment(Qt::AlignRight);
        plotLayout->addItem(item);  // rtics
        item = new PlotTicsLayoutItem(fm,_bookModel(),plotIdx);
        item->setAlignment(Qt::AlignBottom);
        plotLayout->addItem(item);  // btics
        item = new TicLabelsLayoutItem(fm,fm8,_bookModel(),plotIdx);
        item->setAlignment(Qt::AlignBottom);
        plotLayout->addItem(item);  // xticlabels
        item = new XAxisLabelLayoutItem(fm,_bookModel(),plotIdx);
        plotLayout->addItem(item);  // xaxislabel
    }
    double pixelRatio = paintDevice->devicePixelRatio();
    int ww = qRound((double)paintDevice->width()/pixelRatio);
    int hh = qRound((double)paintDevice->height()/pixelRatio);
    pageLayout.setGeometry(QRect(0,0,ww,hh));

    // Print layouts
    //QColor green(0,255,0);
    //QPen greenPen(green);
    painter->setPen(pagePen);
    int nItems = pageLayout.count();
    for ( int i = 0; i < nItems; ++i ) {
        QLayoutItem* item = pageLayout.itemAt(i);
        QRect parentRect = item->geometry();
        //painter->setPen(greenPen);
        //painter->drawRect(parentRect);
        //painter->setPen(pagePen);
        if ( item->layout() && item->layout()->count() == 14 ) {
            // It's a plotlayout since it has 14 elements!
            QLayout* plotLayout= item->layout();
            QRect C = plotLayout->itemAt(3)->geometry();
            C.translate(parentRect.x(),parentRect.y());
            QRectF M = plotlayout2mathrect.value(plotLayout);
            for ( int j = 0; j < plotLayout->count(); ++j ) {
                QLayoutItem* childItem = plotLayout->itemAt(j);
                QRect R = childItem->geometry();
                R.translate(parentRect.x(),parentRect.y());
                //painter->setPen(greenPen);
                //painter->drawRect(R);
                //painter->setPen(pagePen);
                PaintableLayoutItem* paintItem =
                                  dynamic_cast<PaintableLayoutItem*>(childItem);
                if ( paintItem ) {
                    paintItem->paint(painter,R,R,C,M);
                }
            }
        }
    }
    QRect R = pageTitleLayoutItem->geometry();
    QRect RG;
    QRect C;
    QRect M;
    pageTitleLayoutItem->paint(painter,R,RG,C,M);

    // Clean up
    for ( int i = 0; i < nItems; ++i ) {
        QLayoutItem* item = pageLayout.itemAt(i);
        if ( item->layout() ) {
            for ( int j = 0; j < item->layout()->count(); ++j ) {
                delete item->layout()->itemAt(j);
            }
        }
        delete item;
    }

    painter->setPen(origPen);
    painter->restore();
}

void BookView::_nbCloseRequested(int tabId)
{
    if ( model() == 0 ) return;

    QModelIndex pageIdx = _tabIdToModelIdx(tabId);
    model()->removeRow(pageIdx.row(),pageIdx.parent()); // rowsAboutToBeRemoved deletes page
}

void BookView::_pageViewCurrentChanged(const QModelIndex &currIdx,
                                       const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    selectionModel()->setCurrentIndex(currIdx,QItemSelectionModel::NoUpdate);
}

void BookView::dataChanged(const QModelIndex &topLeft,
                           const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
}

void BookView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if (  model()->data(pidx).toString() != "Page" &&
          model()->data(pidx).toString() != "Table" ) {
        return;
    }

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();

        if ( cText == "PageName" ) {
            PageView* pageView = new PageView;
            _childViews << pageView;
            pageView->setModel(model());
            pageView->setRootIndex(idx.parent());
            connect(pageView->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    this,
                    SLOT(_pageViewCurrentChanged(QModelIndex,QModelIndex)));
            connect(pageView, SIGNAL(signalDropEvent(QDropEvent*,QModelIndex)),
                    this, SLOT(slotDropEvent(QDropEvent*,QModelIndex)));
            int tabId = _nb->addTab(pageView,"Page");
            QString pageName = _bookModel()->getDataString(pidx,
                                                           "PageName","Page");
            QString shortName = pageName.split(":").at(0);
            QFileInfo fi(shortName);
            shortName = fi.fileName();
            _nb->setTabToolTip(tabId,pageName);
            _nb->setTabText(tabId,shortName);
            _nb->setTabWhatsThis(tabId, "Page");
        } else if ( cText == "TableName") {
            TablePageView* tablePageView = new TablePageView;
            _childViews << tablePageView;
            tablePageView->setModel(model());
            tablePageView->setRootIndex(idx.parent());
            int tabId = _nb->addTab(tablePageView,"Table");
            QString tableName = _bookModel()->getDataString(pidx,
                                                           "TableName","Table");
            QString shortName = tableName.split(":").at(0) + ".table";
            QFileInfo fi(shortName);
            shortName = fi.fileName();
            _nb->setTabToolTip(tabId,tableName);
            _nb->setTabText(tabId,shortName);
            _nb->setTabWhatsThis(tabId, "Table");
        }
    }
}

void BookView::rowsAboutToBeRemoved(const QModelIndex &pidx, int start, int end)
{
    if ( start != end ) {
        fprintf(stderr,"koviz [bad scoobs]:1:BookView::rowsAboutToBeRemoved(): "
                       "TODO: support deleting multiple rows at once.\n");
        exit(-1);
    }
    if ( start < 0 || start >= _childViews.size() ) {
        fprintf(stderr,"koviz [bad scoobs]:2:BookView::rowsAboutToBeRemoved(): "
                       "childViews list not in sync with model.\n");
        exit(-1);
    }

    QModelIndex idx = model()->index(start,0,pidx);

    int tabId = _modelIdxToTabId(idx);
    QWidget* widget = _nb->widget(tabId);
    _nb->removeTab(tabId);

    foreach ( QAbstractItemView* view, _childViews ) {
        if ( view == widget ) {
            disconnect(view,0,0,0);
            _childViews.removeOne(view);
            delete view;
            break;
        }
    }
}
