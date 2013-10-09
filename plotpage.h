#ifndef PLOTPAGE_H
#define PLOTPAGE_H

#include "qplot/qcustomplot.h"
#include "plot.h"
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

private:
    DPPage* _dppage;
    void _lay6(DPPage* page);
    QList<Plot*> _plots;

    
signals:
    
public slots:
    
};

#endif // PLOTPAGE_H
