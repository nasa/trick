#ifndef TRICKCURVE_H
#define TRICKCURVE_H

#include "qplot/qcustomplot.h"
#include "trickmodel.h"
#include "timeit_linux.h"

class QCP_LIB_DECL TrickCurve : public QCPAbstractPlottable
{
    Q_OBJECT
    Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)
    enum LineStyle { lsNone, lsLine };

public:
    explicit TrickCurve(QCPAxis *keyAxis, QCPAxis *valueAxis) ;
    virtual ~TrickCurve();

    LineStyle lineStyle() const { return mLineStyle; }
    void setLineStyle(LineStyle style);
    QCPRange xRange(bool &validRange, SignDomain inSignDomain=sdBoth);
    QCPRange yRange(bool &validRange, SignDomain inSignDomain=sdBoth);

    virtual double selectTest(const QPointF &pos,
                              bool onlySelectable, QVariant *details=0) const;

    void setData(TrickModel* model, int tcol, int xcol, int ycol);
    void setValueScaleFactor(double sf) { _valueScaleFactor = sf; }
    double valueScaleFactor() { return _valueScaleFactor; }

    virtual void clearData()
    {
        _model = 0 ;
        _tcol = 0;
        _xcol = 0;
        _ycol = 0;
        _isXRangeCalculated = false;
        _isYRangeCalculated = false;
    }

protected:
    // property members:
    TrickModel* _model;
    int _tcol;
    int _xcol;
    int _ycol;

    LineStyle mLineStyle;

    // reimplemented virtual methods:
    virtual void draw(QCPPainter *painter);
    virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
    virtual QCPRange getKeyRange(bool &validRange,
                                 SignDomain inSignDomain=sdBoth) const;
    virtual QCPRange getValueRange(bool &validRange,
                                   SignDomain inSignDomain=sdBoth) const;

    // non-virtual methods:
    void getCurveData(QVector<QPointF> *lineData) const;
    double pointDistance(const QPointF &pixelPoint) const;
    QPointF outsideCoordsToPixels(double key, double value,
                                  int region, QRect axisRect) const;

    friend class QCustomPlot;
    friend class QCPLegend;

private:
    TrickCurve();
    double _valueScaleFactor;
    bool _isXRangeCalculated;
    bool _isYRangeCalculated;
    QCPRange _xrange;
    QCPRange _yrange;
};


#endif // TRICKCURVE_H
