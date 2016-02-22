#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QList>
#include <QTransform>

#include "kplotmodel.h"

class KPlot : public QWidget
{
    Q_OBJECT
public:
    explicit KPlot(KPlotModel *plotModel, QWidget *parent = 0);
    /*
    explicit KPlot(const QRectF& rect,   // in value coords
                        QWidget *parent = 0);
                        */

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;


private:

    KPlotModel* _plotModel;
    double _ptSize;   // Point size of pen (0 is "cosmetic", i.e. 1pt unscaled)

    QRectF _rect;
    QTransform _coordToPixelTransform() const;

    void _paintSinTestPattern();
    void _paintSinTest();
    QPainterPath _sinPath();


signals:
    
public slots:
    
};

#endif // PLOT_H
