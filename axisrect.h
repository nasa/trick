#ifndef PLOT_H
#define PLOT_H

#ifdef SNAPGUI

#include <QRubberBand>
#include "qplot/qcustomplot.h"
#include "trickmodel.h"
#include "trickcurve.h"
#include "dp.h"
#include "trickmodel.h"
#include "monte.h"

class AxisRect : public QCPAxisRect
{
  public:
    AxisRect(DPPlot* dpplot, QCustomPlot* plotwidget);
    ~AxisRect();

    void setData(Monte* monte);

    TrickCurve* addCurve(TrickModel* model, const QString& yparam,
                         double valueScaleFactor=1.0);
    TrickCurve *addCurve(TrickModel *model,
                        int tcol, int xcol, int ycol,
                        double valueScaleFactor=1.0);
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
    QCustomPlot* _plotwidget;
    DPPlot* _dpplot;
    QCPAxis* _xAxis;
    QCPAxis* _yAxis;
    QList<TrickCurve*> _curves;
    void _set_interactions();

    QPoint _origin;
    QRubberBand* _rubber_band;

    void _fitXRange();
    void _fitYRange();

    bool _isXRangeCalculated;
    bool _isYRangeCalculated;
    QCPRange _xDataRange;
    QCPRange _yDataRange;
};

#endif // SNAPGUI
#endif

