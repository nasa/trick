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

    for ( int ii = 0; ii < _snap->tables.size(); ++ii) {
        QTableView* tv = _create_table_view(_snap->tables.at(ii),
                                            _snap->tables.at(ii)->orientation());
        _tvs.append(tv);
        QString title = _snap->tables.at(ii)->tableName();
        tab->addTab(tv,title);
    }

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    QFrame* f2 = new QFrame;
    f2->setSizePolicy(sizePolicy);
    s->addWidget(f2);

    QGridLayout* lay2 = new QGridLayout(f2);
    lay2->setSpacing(0);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay2->setObjectName(QString::fromUtf8("layout2"));

    TrickDataModel trick;
    trick.load_binary_trk("log_frame",
                          "/home/vetter/dev/RUN_preCapture_rm2000");
    SnapPlot* plot = new SnapPlot(f2);
    lay2->addWidget(plot,0,0,1,1);
    plot->addCurve(trick,0,1);
    plot->xAxis->setLabel("Time (s)");
    plot->yAxis->setLabel("Frame Scheduled Time (s)");
    plot->zoomToFit();

    //
    // Resize main window
    //
    resize(1200,700);
    this->show();

    //
    // Hack to resize notebook of tables to correct size
    //
    int w = 0;
    int ncols = _snap->tables.at(3)->columnCount();
    for ( int ii = 0; ii < ncols; ++ii) {
        w += _tvs.at(3)->columnWidth(ii);  // Job Culprits
    }
    w += frame->contentsMargins().left();
    w += frame->contentsMargins().right();
    int margins = frame->frameRect().width() - frame->childrenRect().width();
    w += margins;
    frame->setMaximumWidth(w+8);
}

void SnapWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    setMenuWidget(_menuBar);
    //connect(_exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

QTableView* SnapWindow::_create_table_view(QAbstractItemModel *model,
                                           Qt::Orientation orientation)
{
    QTableView* tv = new QTableView();
    tv->setModel(model);
    tv->setSortingEnabled(false);
    tv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setShowGrid(true);
    tv->setCurrentIndex(QModelIndex());
    tv->horizontalHeader()->setStretchLastSection(false);
    tv->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);

    if ( orientation == Qt::Horizontal ) {
        tv->verticalHeader()->hide();
    }

    return tv;
}
SnapCurve::SnapCurve( QCPAxis* xaxis, QCPAxis* yaxis,
                     const TrickDataModel &model,
                     int xcol, int ycol, const QColor &color) :
    QCPCurve(xaxis,yaxis)
{
    QPen graphPen;
    graphPen.setColor(color);
    graphPen.setWidthF(0);
    setPen(graphPen);
    setAntialiased(false);

    //
    // Set curve data
    //
    int n = model.rowCount(); // number of points in graph
    QModelIndex idx = model.index(0,0);
    _xmin =  model.data(idx).toDouble();
    idx = model.index(n-1,0);
    _xmax =  model.data(idx).toDouble();

    _ymin = 1.0e20;
    _ymax = -1.0e20;
    QVector<double> x(n), y(n);
    for ( int i = 0; i < n; ++i )
    {
        QModelIndex ix = model.index(i,xcol);

        QModelIndex iy = model.index(i,ycol);

        x[i] = model.data(ix).toDouble();
        y[i] = model.data(iy).toDouble()/1000000.0;
        _ymin = qMin(_ymin,y[i]);
        _ymax = qMax(_ymax,y[i]);
    }
    setData(x, y);

    //
    // Set curve name
    //
    QString param = model.headerData(ycol,Qt::Horizontal,
                                  TrickDataModel::ParamName).toString();
    setName(param);
}

SnapPlot::SnapPlot(QWidget* parent) :
    QCustomPlot(parent),
    _rubber_band(0)
{
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);

    setNoAntialiasingOnDrag(true);

    // Title (blank for now)
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, new QCPPlotTitle(this, ""));

    QFont legendFont = font();
    legendFont.setPointSize(10);
    legend->setFont(legendFont);
    legend->setSelectedFont(legendFont); legend->setSelectableParts(QCPLegend::spItems); legend->setVisible(true);

    setInteractions(QCP::iRangeZoom |
                    QCP::iRangeDrag | QCP::iSelectAxes |
                    QCP::iSelectLegend | QCP::iSelectPlottables);
}

SnapCurve* SnapPlot::addCurve(const TrickDataModel &model,
                              int xcol, int ycol,
                              const QColor& color)
{
    SnapCurve *curve = new SnapCurve(xAxis, yAxis, model, xcol, ycol, color);
    _curves.append(curve);
    addPlottable(curve);

    //_zoom_to_fit(plot,_xaxis_min,_xaxis_max,_yaxis_min,_yaxis_max);
    return curve;
}

void SnapPlot::zoomToFit()
{
    double xmin = 1.0e20;
    double ymin = 1.0e20;
    double xmax = -1.0e20;
    double ymax = -1.0e20;
    foreach ( SnapCurve* curve, _curves ) {
        if ( curve->xmin() < xmin ) xmin = curve->xmin();
        if ( curve->ymin() < ymin ) ymin = curve->ymin();
        if ( curve->xmax() > xmax ) xmax = curve->xmax();
        if ( curve->ymax() > ymax ) ymax = curve->ymax();
    }

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
    xAxis->setRange(xmin,xmax+xp);
    yAxis->setRange(ymin,ymax+yp);
    axisRect()->setupFullAxesBox();
    replot();
}



void SnapPlot::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton ){
        _origin = event->pos();
        if ( !_rubber_band ) {
            _rubber_band = new QRubberBand(QRubberBand::Rectangle,this);
        }
        _rubber_band->setGeometry(QRect(_origin, QSize()));
        _rubber_band->show();
    } else  if ( event->button() == Qt::RightButton ){
        zoomToFit();
    } else {
        // if an axis is selected, only drag in direction of that axis
        // if no axis is selected, both directions may be dragged
        if (xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            axisRect()->setRangeDrag(xAxis->orientation());
        } else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
            axisRect()->setRangeDrag(yAxis->orientation());
        } else {
            axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
        }
    }

    QCustomPlot::mousePressEvent(event);
}

void SnapPlot::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::NoButton && _rubber_band ){
        _rubber_band->setGeometry(QRect(_origin, event->pos()).normalized());
    }
    QCustomPlot::mouseMoveEvent(event);
}

void SnapPlot::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::MidButton && _rubber_band ) {
        QRect geom = _rubber_band->geometry();
        double gxmin = (double)geom.left();
        double gxmax = (double)geom.right();
        double xmin = xAxis->pixelToCoord(gxmin);
        double xmax = xAxis->pixelToCoord(gxmax);
        double gymin = (double)geom.top();
        double gymax = (double)geom.bottom();
        double ymin = yAxis->pixelToCoord(gymax); // screen flip
        double ymax = yAxis->pixelToCoord(gymin); // screen flip
        xAxis->setRange(xmin,xmax);
        yAxis->setRange(ymin,ymax);
        replot();
        _rubber_band->hide();
    }

    QCustomPlot::mouseReleaseEvent(event);
}

#endif // SNAPGUI
