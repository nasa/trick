#ifdef SNAPGUI

#include "snapplot.h"

SnapCurve::SnapCurve(QCPAxis* xaxis, QCPAxis* yaxis,
                     TrickDataModel* model,
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
    int n = model->rowCount(); // number of points in graph
    QModelIndex idx = model->index(0,0);
    _xmin =  model->data(idx).toDouble();
    idx = model->index(n-1,0);
    _xmax =  model->data(idx).toDouble();

    _ymin = 1.0e20;
    _ymax = -1.0e20;
    QVector<double> x(n), y(n);
    for ( int i = 0; i < n; ++i )
    {
        QModelIndex ix = model->index(i,xcol);

        QModelIndex iy = model->index(i,ycol);

        x[i] = model->data(ix).toDouble();
        y[i] = model->data(iy).toDouble()/1000000.0;
        _ymin = qMin(_ymin,y[i]);
        _ymax = qMax(_ymax,y[i]);
    }
    setData(x, y);

    //
    // Set curve name
    //
    QString param = model->headerData(ycol,Qt::Horizontal,
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
    legend->setSelectedFont(legendFont);
    legend->setSelectableParts(QCPLegend::spItems);
    legend->setVisible(true);

    setInteractions(QCP::iRangeDrag | QCP::iSelectAxes |
                    QCP::iSelectLegend | QCP::iSelectPlottables);
}

SnapCurve* SnapPlot::addCurve(TrickDataModel* model,
                              int xcol, int ycol,
                              const QColor& color)
{
    SnapCurve *curve = new SnapCurve(xAxis, yAxis, model, xcol, ycol, color);
    _curves.append(curve);
    addPlottable(curve);
    replot();

    //_zoom_to_fit(plot,_xaxis_min,_xaxis_max,_yaxis_min,_yaxis_max);
    return curve;
}

bool SnapPlot::removeCurve(int index)
{
    if ( index < 0 || index >= _curves.size() ) {
        return false;
    }

    bool ret;
    SnapCurve* curve = _curves.at(index);
    ret = removePlottable(curve);
    _curves.removeAt(index);
    replot();
    return ret;
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
