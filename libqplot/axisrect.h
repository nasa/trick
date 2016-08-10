#ifndef AXISRECT_H
#define AXISRECT_H

#include <QAbstractItemModel>
#include <QRubberBand>
#include "qcustomplot.h"
#include "libkplot/montemodel.h"
#include "libkplot/trickmodel.h"
#include "trickcurve.h"

class AxisRect : public QCPAxisRect
{
    Q_OBJECT

  public:
    AxisRect(QCustomPlot* plotwidget);
    ~AxisRect();

    TrickCurve *addCurve(TrickCurveModel *model);
    TrickCurve *addCurve(const QVector<double> *t, const QVector<double> *v);
    bool removeCurve(int index);
    //int clearCurves();
    int curveCount() const { return _curves.size(); }
    void zoomToFit(const QCPRange& xrange=QCPRange());
    void setXMinRange(double xMin);
    void setXMaxRange(double xMax);
    void setYMinRange(double yMin);
    void setYMaxRange(double yMax);
    void setStartTime(double startTime);
    void setStopTime(double stopTime);

    void showCurveDiff();
    void toggleCurveDiff();

    virtual QCPRange xDataRange(bool& isValidRange);
    virtual QCPRange yDataRange(bool& isValidRange);


protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

private slots:
    void _curveXScaleChanged(double sf);
    void _curveYScaleChanged(double sf);
    void _curveXBiasChanged(double b);
    void _curveYBiasChanged(double b);

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
    void _set_interactions();
    void _addCurve(TrickCurve* curve);
    QVector<double> _diffCurveTimes;
    QVector<double> _diffCurveVals;
    bool _isDiffPlot;

    QPoint _origin;
    QRubberBand* _rubber_band;

    void _fitXRange();
    void _fitYRange();

    bool _isXRangeCalculated;
    bool _isYRangeCalculated;
    QCPRange _xDataRange;
    QCPRange _yDataRange;
    double _xMinRange;
    double _xMaxRange;
    double _yMinRange;
    double _yMaxRange;
    double _startTime;
    double _stopTime;

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

