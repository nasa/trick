#ifdef SNAPGUI

#include "snapwindow.h"

#include <QDockWidget>
#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>

#include "trickdatamodel.h"

SnapWindow::SnapWindow(Snap *snap, QWidget *parent) :
    QMainWindow(parent),_snap(snap)
{
    setWindowTitle(tr("Snap!"));

    createMenu();

    // Central Widget
    QSplitter* s = new QSplitter;
    setCentralWidget(s);

    QFrame* frame = new QFrame;
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::StyledPanel);
    //frame->setFrameShadow(QFrame::Sunken);
    frame->setLineWidth(1);
    frame->setMidLineWidth(0);
    s->addWidget(frame);

    QGridLayout* lay = new QGridLayout(frame);
    lay->setSpacing(0);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setObjectName(QString::fromUtf8("verticalLayout"));

    QTabWidget* tab = new QTabWidget(frame);
    lay->addWidget(tab,0,0,1,1);

    tableView = new QTableView();
    tableView->setModel(_snap->tables.at(0));
    tableView->setSortingEnabled(false);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setShowGrid(true);
    tableView->setCurrentIndex(QModelIndex());
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->hide();
    tab->addTab(tableView,"Summary");

    QTableView* tv = new QTableView();
    tv->setModel(_snap->tables.at(1));
    tv->setSortingEnabled(false);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setShowGrid(true);
    tv->setCurrentIndex(QModelIndex());
    tv->horizontalHeader()->setStretchLastSection(false);
    tv->verticalHeader()->hide();
    tab->addTab(tv,"Spikes");

    QTableView* tv2 = new QTableView();
    tv2->setModel(_snap->tables.at(2));
    tv2->setSortingEnabled(false);
    tv2->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv2->setSelectionMode(QAbstractItemView::SingleSelection);
    tv2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv2->setShowGrid(true);
    tv2->setCurrentIndex(QModelIndex());
    tv2->horizontalHeader()->setStretchLastSection(false);
    tv2->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tv2->verticalHeader()->hide();
    tab->addTab(tv2,"Thread Info");

    QTableView* tv3 = new QTableView();
    tv3->setModel(_snap->tables.at(3));
    tv3->setSortingEnabled(false);
    tv3->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv3->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv3->setSelectionMode(QAbstractItemView::SingleSelection);
    tv3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv3->setShowGrid(true);
    tv3->setCurrentIndex(QModelIndex());
    tv3->horizontalHeader()->setStretchLastSection(false);
    tv3->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tv3->verticalHeader()->hide();
    tab->addTab(tv3,"Job Culprits");

    QTableView* tv4 = new QTableView();
    tv4->setModel(_snap->tables.at(4));
    tv4->setSortingEnabled(false);
    tv4->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv4->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv4->setSelectionMode(QAbstractItemView::SingleSelection);
    tv4->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv4->setShowGrid(true);
    tv4->setCurrentIndex(QModelIndex());
    tv4->horizontalHeader()->setStretchLastSection(false);
    tv4->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tv4->verticalHeader()->hide();
    tab->addTab(tv4,"Sim Objects");

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QFrame* f2 = new QFrame;
    f2->setSizePolicy(sizePolicy);
    s->addWidget(f2);

    QGridLayout* lay2 = new QGridLayout(f2);
    lay2->setSpacing(0);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay2->setObjectName(QString::fromUtf8("layout2"));

    QCustomPlot* plot = new QCustomPlot(f2);
    lay2->addWidget(plot,0,0,1,1);

    plot->setSizePolicy(sizePolicy);
    plot->setNoAntialiasingOnDrag(true);

    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, ""));

    plot->xAxis->setLabel("Time (s)");
    plot->yAxis->setLabel("Frame Scheduled Time (s)");
    plot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    plot->legend->setFont(legendFont);
    plot->legend->setSelectedFont(legendFont);
    plot->legend->setSelectableParts(QCPLegend::spItems);

    QCPCurve *curve = new QCPCurve(plot->xAxis, plot->yAxis);
    plot->addPlottable(curve);

    curve->setAntialiased(false);

    QPen graphPen;
    graphPen.setColor(QColor(0,0,255));
    graphPen.setWidthF(0);
    curve->setPen(graphPen);

    //
    // Set curve data
    //
    TrickDataModel trick;
    trick.load_binary_trk("log_frame",
                          "/home/vetter/dev/RUN_preCapture_rm2000");
    for ( int ii = 0; ii < trick.rowCount(); ++ii) {
        QModelIndex idx = trick.index(ii,0);
    }


    int n = trick.rowCount(); // number of points in graph
    QModelIndex idx = trick.index(0,0);
    double _xaxis_min =  trick.data(idx).toDouble();
    idx = trick.index(n-1,0);
    double _xaxis_max =  trick.data(idx).toDouble();

    int column = 1;
    double _yaxis_min = 1.0e20;
    double _yaxis_max = -1.0e20;
    QVector<double> x(n), y(n);
    for ( int i = 0; i < n; ++i )
    {
        QModelIndex ix = trick.index(i,0);

        QModelIndex iy = trick.index(i,column);
        x[i] = trick.data(ix).toDouble();
        y[i] = trick.data(iy).toDouble()/1000000.0;
        _yaxis_min = qMin(_yaxis_min,y[i]);
        _yaxis_max = qMax(_yaxis_max,y[i]);
    }
    curve->setData(x, y);

    //
    // Set curve name
    //
    QString param = trick.headerData(column,Qt::Horizontal,
                                  TrickDataModel::ParamName).toString();
    curve->setName(param);

    _zoom_to_fit(plot,_xaxis_min,_xaxis_max,
                      _yaxis_min, _yaxis_max);

    //
    // Resize main window
    //
    resize(1200,700);
    this->show();

    // Resize main frame
    int w = 0;
    int ncols = _snap->tables.at(3)->columnCount();
    for ( int ii = 0; ii < ncols; ++ii) {
        w += tv3->columnWidth(ii);
    }
    w += frame->contentsMargins().left();
    w += frame->contentsMargins().right();
    int margins = frame->frameRect().width() - frame->childrenRect().width();
    w += margins;
    frame->setMaximumWidth(w+8);
}
    /*
          //ui->customPlot->setInteractions(QCP::iRangeZoom |
                                   //QCP::iRangeDrag | QCP::iSelectAxes |
                                    //QCP::iSelectLegend | QCP::iSelectPlottables);


    connect(ui->customPlot, SIGNAL(selectionChangedByUser()),
            this, SLOT(selectionChanged()));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)),
            this, SLOT(mousePress(QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent*)),
            this, SLOT(mouseMove(QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mouseRelease(QMouseEvent*)),
            this, SLOT(mouseRelease(QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)),
            this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
            ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)),
          ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // connect some interaction slots:
  connect(ui->customPlot, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)),
          this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
  connect(ui->customPlot,
          SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
          this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
  connect(ui->customPlot,
       SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)),
       this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

  // connect slot that shows a message in the status bar when a graph is clicked:
  connect(ui->customPlot,
          SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),
          this, SLOT(graphClicked(QCPAbstractPlottable*)));

    // This is for popup menu on graph with right click (Keith took this out)
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuRequest(QPoint)));
            */



void SnapWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    setMenuWidget(_menuBar);
    //connect(_exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

void SnapWindow::_zoom_to_fit(QCustomPlot* plot,
                              double xmin, double xmax,
                              double ymin, double ymax)
{
    // At least show +/- 10% of outside range
    double xp = 0.10*qAbs(xmax - xmin) ;
    double yp = 0.10*qAbs(ymax - ymin) ;
    xmin = xmin-xp;
    ymin = ymin-yp;
    for ( int ii = 0; ii < 3; ++ii) {
        if ( xmin - ii*xp < 0 ) {
            xmin = xmin - ii*xp;
            break;
        }
    }
    for ( int ii = 0; ii < 3 ; ++ii) {
        if ( ymin - ii*yp < 0 ) {
            ymin = ymin - ii*yp;
            break;
        }
    }
    plot->xAxis->setRange(xmin,xmax+xp);
    plot->yAxis->setRange(ymin,ymax+yp);
    plot->axisRect()->setupFullAxesBox();
    plot->replot();
}

#endif // SNAPGUI
