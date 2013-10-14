#ifndef PLOTPAGE_H
#define PLOTPAGE_H

#include "qplot/qcustomplot.h"
#include "axisrect.h"
#include "dp.h"
#include "trickmodel.h"
#include "monte.h"

class PlotPage : public QCustomPlot
{
    Q_OBJECT
public:
    explicit PlotPage(DPPage* page, QWidget* parent=0);
    void setData(Monte *monte);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void draw(QCPPainter* painter);

private:
    DPPage* _dppage;
    void _lay1(DPPage* page);
    void _lay6(DPPage* page);
    QList<AxisRect*> _plots;

    
signals:
    
public slots:
    
};

#endif // PLOTPAGE_H
