#ifndef KPLOT_H
#define KPLOT_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include <QList>
#include <QTransform>
#include <QHash>

#include <float.h>

#include "kplotmodel.h"
#include "libsnapdata/trickcurvemodel.h"

class KPlot : public QWidget
{
    Q_OBJECT
public:
    explicit KPlot(KPlotModel *kPlotModel,
                   const QModelIndex& plotIdx,
                   QWidget *parent = 0);
    ~KPlot();

    /*
    explicit KPlot(const QRectF& rect,   // in value coords
                        QWidget *parent = 0);
                        */

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;


private:

    KPlotModel* _kPlotModel;
    QPersistentModelIndex _plotIdx;

    void _paintTrickCurves();
    void _paintSinTestPattern();
    void _paintSinTest();
    QPainterPath _sinPath();

    QHash<TrickCurveModel*,QPainterPath*> _curve2path;
    void _createPainterPath(TrickCurveModel *curveModel);

signals:
    
public slots:

    void rowsInserted(const QModelIndex& pidx,
                      int start,
                      int end);

};

#endif // KPLOT_H
