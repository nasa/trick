#ifndef AXISRECT_H
#define AXISRECT_H

#include <QAbstractItemModel>
#include <QRubberBand>
#include "qcustomplot.h"
#include "libsnapdata/montemodel.h"
#include "libsnapdata/trickmodel.h"
#include "trickcurve.h"

class AxisRect : public QCPAxisRect
{
  public:
    AxisRect(QCustomPlot* plotwidget);
    ~AxisRect();

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
    virtual void wheelEvent(QWheelEvent *event);

  private:
    QCustomPlot* _plotwidget;
    QCPAxis* _xAxis;
    QCPAxis* _yAxis;
    QPen _currPen;
    QList<QColor> _createColorBands(int nBands, bool isMonte);
    QList<QColor> _colorBandsNormal;
    QList<QColor> _colorBandsMonte;
    int _red;
    int _green;
    int _blue;
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

    double _keyPressMoveFactor;
    void _keyPressMoveLeft();
    void _keyPressMoveRight();
    void _keyPressMoveUp();
    void _keyPressMoveDown();
    void _keyPressZoomIn();
    void _keyPressZoomOut();

    friend class Plot;
};

#endif

