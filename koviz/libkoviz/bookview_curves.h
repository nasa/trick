#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include <QtGlobal>
#include <QVector>
#include <QPolygonF>
#include <QPainter>
#include <QPixmap>
#include <QList>
#include <QColor>
#include <QMouseEvent>
#include <QRubberBand>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QSizeF>
#include <QLineF>
#include <QItemSelectionModel>
#include <QEasingCurve>
#include <QImage>
#include <QFontMetrics>
#include <QPoint>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QSlider>
#include <QLineEdit>
#include <QIntValidator>
#include <QProgressDialog>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "bookidxview.h"
#include "unit.h"
#include "curvemodel.h"
#include "roundoff.h"
#include "layoutitem_curves.h"
#include "coord_arrow.h"
#include "curvemodel_fft.h"
#include "curvemodel_ifft.h"
#include "curvemodel_bw.h"
#include "curvemodel_sg.h"
#include "curvemodel_deriv.h"
#include "curvemodel_integ.h"

class TimeAndIndex
{
  public:
    TimeAndIndex(double time, int timeIdx, const QModelIndex& modelIdx);
    double time() const;
    int timeIdx() const;
    QModelIndex modelIdx() const;

  private:
    TimeAndIndex() {}
    double _time;
    int _timeIdx;  // Index for duplicate timestamps
    QModelIndex _modelIdx;
};

class FFTCurveCache
{
  public:
    FFTCurveCache(double xbias, double xscale, CurveModel* curveModel);
    double xbias() const ;
    double xscale() const ;
    CurveModel* curveModel() const ;

  private:
    FFTCurveCache() {}
    double _xbias;
    double _xscale;
    CurveModel*  _curveModel;
};

class FFTCache
{
  public:
    FFTCache();
    bool isCache;
    QString xAxisLabel;
    QRectF M;
    double start;
    double stop;
    QList<FFTCurveCache*> curveCaches;
};

class DerivCurveCache
{
  public:
    DerivCurveCache(CurveModel* curveModel,
                    const QString& yUnit,
                    const QString& yLabel);
    CurveModel* curveModel() const ;
    QString yUnit() const ;
    QString yLabel() const;

  private:
    DerivCurveCache() {}
    CurveModel*  _curveModel;
    QString _yUnit;
    QString _yLabel;
};

class DerivPlotCache
{
  public:
    DerivPlotCache();
    ~DerivPlotCache();
    QString yAxisLabel;
    QString yUnit;
    QRectF M;
    QList<DerivCurveCache*> curveCaches;
};

class DerivCache
{
  public:
    DerivCache();
    ~DerivCache();
    QList<DerivPlotCache*> plotCaches;
};

class IntegCurveCache
{
  public:
    IntegCurveCache(CurveModel* curveModel,
                    const QString& yLabel,
                    const QString& yUnit);
    CurveModel* curveModel() const ;
    QString yLabel() const ;
    QString yUnit() const ;

  private:
    IntegCurveCache() {}
    CurveModel*  _curveModel;
    QString _yLabel;
    QString _yUnit;
};

class IntegPlotCache
{
  public:
    IntegPlotCache();
    ~IntegPlotCache();
    double initialValue;
    QString yAxisLabel;
    QString yUnit;
    QRectF M;
    QList<IntegCurveCache*> curveCaches;
};

class IntegCache
{
  public:
    IntegCache();
    ~IntegCache();
    QList<IntegPlotCache*> plotCaches;
};

class CurvesView : public BookIdxView
{
    Q_OBJECT

public:
    explicit CurvesView(QWidget *parent = 0);
    ~CurvesView();

public:
    virtual void setCurrentCurveRunID(int runID);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void currentChanged(const QModelIndex& current,
                                const QModelIndex& previous);
    virtual void resizeEvent(QResizeEvent *event);


private:
    QPainterPath* _errorPath;

    QPainterPath _sinPath();
    QPainterPath _stepPath();

    QList<TimeAndIndex*> _markers;

    void _paintCoplot(const QTransform& T, QPainter& painter,QPen& pen);
    void _paintErrorplot(const QTransform& T,
                         QPainter& painter, const QPen &pen,
                         const QModelIndex &plotIdx);
    void _paintCurve(const QModelIndex& curveIdx,
                     const QTransform &T, QPainter& painter,
                     bool isHighlight);
    void _paintMarkers(QPainter& painter);

    QModelIndex _chooseCurveNearMousePoint(const QPoint& pt);
    bool _isErrorCurveNearMousePoint(const QPoint& pt);

    QPixmap* _pixmap;
    QRectF _lastM;
    bool _isMeasure;
    QPoint _mouseCurrPos;
    QPixmap* _createLivePixmap();

    double _mousePressXBias;
    double _mousePressYBias;

    QString _format(double d);

    int _idxAtTimeBinarySearch(QPainterPath* path,
                               int low, int high, double time);

    // Key Events
    bool _isLastPoint;
    QPointF _lastPoint;
    void _keyPressSpace();
    void _keyPressUp();
    void _keyPressDown();
    void _keyPressPeriod();
    void _keyPressArrow(const Qt::ArrowType& arrow);
    void _keyPressComma();
    void _keyPressEscape();
    void _keyPressF();
    void _keyPressB();
    void _keyPressG();
    void _keyPressD();
    void _keyPressI();
    void _keyPressMinus();

    QFrame* _bw_frame;
    QLineEdit* _bw_label;
    QSlider* _bw_slider;

    QFrame* _sg_frame;
    QLineEdit* _sg_window;
    QLineEdit* _sg_degree;
    QSlider* _sg_slider;
    void _keyPressGChange(int window, int degree);

    QFrame* _integ_frame;
    QLineEdit* _integ_ival;

    FFTCache _fftCache ;
    DerivCache _derivCache ;
    IntegCache _integCache ;

private slots:
    void _keyPressBSliderChanged(int value);
    void _keyPressBLineEditReturnPressed();
    void _keyPressGSliderChanged(int value);
    void _keyPressGLineEditReturnPressed();
    void _keyPressGDegreeReturnPressed();
    void _keyPressIInitValueReturnPressed();

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);


};

#endif // CURVESVIEW_H
