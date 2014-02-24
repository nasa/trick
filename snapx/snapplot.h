#ifndef SNAP_PLOT_H
#define SNAP_PLOT_H

#include <QRubberBand>
#include "libqplot/qcustomplot.h"
#include "libsnapdata/trickcurvemodel.h"
#include "monte/trickcurve.h"

class SnapPlot : public QCustomPlot
{
  public:
    SnapPlot(QWidget* parent=0);
    ~SnapPlot();

    TrickCurve* addCurve(TrickModel* model, const QString& yparam,
                         double valueScaleFactor=1.0);
    TrickCurve *addCurve(TrickCurveModel *model);
    bool removeCurve(int index);
    //int clearCurves();
    int curveCount() const { return _curves.size(); }

    void zoomToFit(const QCPRange& xrange=QCPRange());

    virtual QCPRange xDataRange(bool& isValidRange);
    virtual QCPRange yDataRange(bool& isValidRange);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

  private:
    QList<TrickCurve*> _curves;
    QList<TrickCurveModel*> _curve_models;
    void _set_interactions();

    QPoint _origin;
    QRubberBand* _rubber_band;

    void _fitXRange();
    void _fitYRange();

    bool _isXRangeCalculated;
    bool _isYRangeCalculated;
    QCPRange _xrange;
    QCPRange _yrange;
};

#endif

