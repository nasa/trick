#ifndef AXISRECT_H
#define AXISRECT_H

#ifdef SNAPGUI

#include <QAbstractItemModel>
#include <QRubberBand>
#include "qplot/qcustomplot.h"
#include "trickmodel.h"
#include "trickcurve.h"
#include "dp.h"
#include "trickmodel.h"
#include "montemodel.h"

class AxisRect : public QCPAxisRect
{
  public:
    AxisRect(DPPlot* dpplot, QCustomPlot* plotwidget);
    AxisRect(const QModelIndex& plotIdx, QCustomPlot* plotwidget);
    ~AxisRect();

    void setData(MonteModel *monteModel);

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
    virtual void keyPressEvent(QKeyEvent *event);

  private:
    QCustomPlot* _plotwidget;
    DPPlot* _dpplot;
    QCPAxis* _xAxis;
    QCPAxis* _yAxis;
    QList<TrickCurve*> _curves;
    QList<TrickCurveModel*> _curve_models;
    void _set_interactions();

    QPoint _origin;
    QRubberBand* _rubber_band;

    void _fitXRange();
    void _fitYRange();

    bool _isXRangeCalculated;
    bool _isYRangeCalculated;
    QCPRange _xDataRange;
    QCPRange _yDataRange;

    QString _abbreviate(const QString& label,int maxlen=35);

    double _keyPressMoveFactor;
    void _keyPressLeft();
    void _keyPressRight();
    void _keyPressUp();
    void _keyPressDown();
    void _keyPressZoomIn();
    void _keyPressZoomOut();

    friend class Plot;
};

#endif // SNAPGUI
#endif

