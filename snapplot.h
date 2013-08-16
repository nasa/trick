#ifndef SNAP_PLOT_H
#define SNAP_PLOT_H

#ifdef SNAPGUI

#include <QRubberBand>
#include "qplot/qcustomplot.h"
#include "trickdatamodel.h"

class SnapCurve : public QCPCurve
{
  public:
    SnapCurve(QCPAxis* xaxis, QCPAxis* yaxis,
              TrickDataModel *model, int xcol, int ycol,
              const QColor& color=Qt::blue);

    double xmin() { return _xmin; }
    double xmax() { return _xmax; }
    double ymin() { return _ymin; }
    double ymax() { return _ymax; }

  private:
    double _xmin;
    double _ymin;
    double _xmax;
    double _ymax;

};

class SnapPlot : public QCustomPlot
{
  public:
    SnapPlot(QWidget* parent=0);

   //SnapCurve*    curve(int index) const;
    SnapCurve* addCurve(TrickDataModel *model,
                       int xcol, int ycol,
                       const QColor& color=Qt::blue);
    bool removeCurve(int index);
    //int clearCurves();
    //int curveCount() const;

    void zoomToFit();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

  private:
    QList<SnapCurve*> _curves;
    void _set_interactions();

    QPoint _origin;
    QRubberBand* _rubber_band;


};

#endif // SNAPGUI

#endif

