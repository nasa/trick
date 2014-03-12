#include "timelineplot.h"

TimeLinePlot::TimeLinePlot(const QString &csv_file, QWidget* parent) :
    QCustomPlot(parent),
    _rubber_band(0)
{
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);
    setNoAntialiasingOnDrag(true);

    // Font
    QFont legendFont = font();
    legendFont.setPointSize(10);

    // Title (blank for now)
    plotLayout()->insertRow(0);
    QCPPlotTitle* title = new QCPPlotTitle(this,"Time Line");
    title->setFont(legendFont);
    plotLayout()->addElement(0, 0,title);

    legend->setFont(legendFont);
    legend->setSelectedFont(legendFont);
    legend->setSelectableParts(QCPLegend::spItems);
    legend->setVisible(false);

    setInteractions(QCP::iRangeDrag | QCP::iSelectAxes |
                    QCP::iSelectLegend | QCP::iSelectPlottables);

    _timeline_loader = new TimeLineLoader(csv_file);

    double y0 = 0.0;
    double dy = 1.0;

    QCPGraph* g0;
    QCPGraph* g1;
    QCPGraph* g2;
    QCPGraph* g3;

    double t0 = 75.0;
    double t1 = 80.0;
    int td0 = 2;  int j0 = 478.91;
    int td1 = 3;  int j1 = 479.31;
    int td2 = 11; int j2 = 482.70;
    int td3 = 11; int j3 = 482.75;

    g0 = _add_timeline_graph(this, QString("g0"),
                             Qt::gray,_timeline_loader,
                             td0,j0,y0,y0+dy,t0,t1,true);
    y0 += dy;

    g1 = _add_timeline_graph(this,QString("g1"),
                             Qt::green, _timeline_loader,
                             td1,j1,y0,y0+dy,t0,t1);
    y0 += dy;

    g2 = _add_timeline_graph(this,QString("g2"),
                             Qt::blue,_timeline_loader,
                             td2,j2,y0,y0+dy,t0,t1);
    y0 += dy;

    g3 = _add_timeline_graph(this,QString("g3"),
                             Qt::red,_timeline_loader,
                             td3,j3,y0,y0+dy,t0,t1);
    y0 += dy;

    //QCPRange xrange(63.96,64.25);
    QCPRange xrange(t0,t1);
    this->xAxis->setRange(xrange);
    QCPRange yrange(0,y0);
    this->yAxis->setRange(yrange);
    this->yAxis->setAutoTicks(false);
    this->yAxis->setAutoTickLabels(false);
    QVector<double> tics;
    tics.append(0.5);
    tics.append(1.5);
    tics.append(2.5);
    tics.append(3.5);
    this->yAxis->setTickVector(tics);
    QVector<QString> tic_labels;
    QString s0 = QString("T%1-J%2").arg(td0).arg(j0);
    QString s1 = QString("T%1-J%2").arg(td1).arg(j1);
    QString s2 = QString("T%1-J%2").arg(td2).arg(j2);
    QString s3 = QString("T%1-J%2").arg(td3).arg(j3);
    tic_labels.append(s0);
    tic_labels.append(s1);
    tic_labels.append(s2);
    tic_labels.append(s3);
    this->yAxis->setTickVectorLabels(tic_labels);
    //this->axisRect()->setupFullAxesBox();

#if 0
    QCPGraph* g01 = new QCPGraph(this->xAxis,this->yAxis);
    this->addPlottable(g01);
    double t = 0.0;
    double dt = 0.1;
    double end_time = 300.0;
    QVector<double> y(end_time/dt);
    QVector<double> x;
    while ( t <= end_time ) {
        x.append(t);
        t += dt;
    }
    g01->setData(x,y);

    this->xAxis2->setRange(xrange);
    this->xAxis2->setVisible(true);
    this->xAxis2->setAutoTicks(false);
    //this->xAxis2->setAutoTickLabels(false);
    this->xAxis2->setTickVector(x);
    this->xAxis2->setTickLengthIn(424);
    //this->xAxis2->setTickVectorLabels(tic_labels);
#endif

    /*
    g->setName(curvename);
    g->setPen(QPen(Qt::black));
    g->setBrush(color);

    QCPGraph *bottomline = new QCPGraph(this->xAxis, this->yAxis);
    this->addPlottable(bottomline);
    QString title = QString("bottomline_%1").arg(y0);
    bottomline->setName(title);
    bottomline->setVisible(false);
    bottomline->setBrush(color);
    bottomline->removeFromLegend();
    QVector<double> xd; QVector<double> yd;
    xd.append(beg_time);  yd.append(y0);
    xd.append(end_time);  yd.append(y0);
    bottomline->setData(xd, yd);
    g->setChannelFillGraph(bottomline);
    return g;
    */

}

TimeLinePlot::~TimeLinePlot()
{
    delete _timeline_loader;
}

QCPGraph* TimeLinePlot::_add_timeline_graph(QCustomPlot *plot,
                             QString curvename, QColor color,
                             TimeLineLoader *tl,
                             int tid, double jid,
                             double y0, double y1,
                             double beg_time, double end_time,
                             bool is_exclude_jid )
{
    QCPGraph *g= new QCPGraph(plot->xAxis,plot->yAxis);
    plot->addPlottable(g);
    _add_timeline_data(tl,g,tid,jid,y0,y1,beg_time,end_time,is_exclude_jid);
    g->setName(curvename);
    if ( color == Qt::gray ) {
        g->setPen(QPen(Qt::black));
    } else {
        g->setPen(color);
    }
        g->setPen(color);
    g->setBrush(color);

    QCPGraph *bottomline = new QCPGraph(this->xAxis, this->yAxis);
    this->addPlottable(bottomline);
    QString title = QString("bottomline_%1").arg(y0);
    bottomline->setName(title);
    bottomline->setVisible(false);
    bottomline->setBrush(color);
    bottomline->removeFromLegend();
    QVector<double> xd; QVector<double> yd;
    xd.append(beg_time);  yd.append(y0);
    xd.append(end_time);  yd.append(y0);
    bottomline->setData(xd, yd);
    g->setChannelFillGraph(bottomline);

    return g;
}

void TimeLinePlot::_add_timeline_data(TimeLineLoader *timelineloader,
                                    QCPGraph *graph,
                                    int tid, double jid,
                                    double y0, double y1,
                                    double beg_time, double end_time,
                                    bool is_exclude_jid)
{
    SnapTable* table = timelineloader->table(tid,beg_time,end_time,jid,
                                             is_exclude_jid);
    double dt = 1.0e-6;
    QVector<double> xd;
    QVector<double> yd;
    for ( int ii = 0; ii < table->rowCount(); ++ii) {
        double t0 = table->data(table->index(ii,0)).toDouble();
        double t1 = table->data(table->index(ii,1)).toDouble();

        xd.append(t0);
        yd.append(y0);

        if ( qAbs(t1-t0) > 1.0e-5 ) {
            xd.append(t0+dt);
            yd.append(y1);

            xd.append(t1-dt);
            yd.append(y1);
        } else {
            xd.append((t0+t1)/2.0);
            yd.append(y1);
        }

        xd.append(t1);
        yd.append(y0);
    }
    delete table;
    graph->setData(xd, yd);
}

void TimeLinePlot::mousePressEvent(QMouseEvent *event)
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

void TimeLinePlot::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::NoButton && _rubber_band ){
        _rubber_band->setGeometry(QRect(_origin, event->pos()).normalized());
    }
    QCustomPlot::mouseMoveEvent(event);
}

void TimeLinePlot::mouseReleaseEvent(QMouseEvent *event)
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

void TimeLinePlot::zoomToFit(const QCPRange& xrange)
{
    return;
    double dy = 0.005;  // TODO: hard coded
    int num_graphs = _graphs.size();
    num_graphs = 4;  // TODO: hard coded

    xAxis->setRange(xrange);
    yAxis->setRange(0.0,num_graphs*dy);
    axisRect()->setupFullAxesBox();
    replot();
}
