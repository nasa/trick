#ifndef PLOTMODEL_H
#define PLOTMODEL_H

#include <QStandardItemModel>
#include <QPersistentModelIndex>
#include <QRectF>
#include <QMatrix4x4>
#include <QVector4D>
#include <QPolygonF>
#include <QPolygon>
#include <QTransform>
#include <QWidget>
#include <math.h>

#include "libqplot/plotbookmodel.h"

class KPlotModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit KPlotModel(PlotBookModel* plotBookModel,
                        const QModelIndex& plotIdx,
                        QObject *parent = 0);

    PlotBookModel* bookModel() { return _plotBookModel; }
    QPersistentModelIndex plotIdx() { return _plotIndex; }

    QRectF xyRect() const;
    QRectF mathRect(const QWidget* widget) const;
    double curvePointSize(const QWidget *widget) const;
    double hLinePointSize(const QWidget *widget) const;
    double vLinePointSize(const QWidget *widget) const;
    QList<double> majorXTics();
    QList<double> minorXTics();
    QList<double> majorYTics();
    QList<double> minorYTics();
    QTransform coordToPixelTransform(const QWidget *widget) const;
    void setPlotWidget(QWidget* plotWidget) { _plotWidget = plotWidget; }

    bool isEqual(double a, double b, ulong maxD=10, bool isNeighborMethod=true);

    // Get/Set convenience wrappers for model items
    QString title() const;
    void setTitle(const QString& title);
    void setXAxisLabel(const QString& label);
    void setYAxisLabel(const QString& label);
#if 0
    void setXMinRange(double xMin);
    void setXMaxRange(double xMax);
    void setYMinRange(double yMin);
    void setYMaxRange(double yMax);
    void setStartTime(double startTime);
    void setStopTime(double stopTime);
    void setGrid(bool isOn);
    void setGridColor(const QString& colorString);
    void setBackgroundColor(const QString& colorString);
#endif

private:
    PlotBookModel* _plotBookModel;
    QPersistentModelIndex _plotIndex;
    QWidget* _plotWidget;
    void _initModel();
    QList<double> calcTicSet(double aIn, double bIn,
                             double u=1.0, double n=10.0);

    QPointF _scaledPointSize(const QWidget *widget) const;
    double _pointSize() const;

signals:
    
public slots:
    
};

#endif // PLOTMODEL_H
