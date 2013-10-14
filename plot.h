#ifndef PLOT_H
#define PLOT_H

#include "qplot/qcustomplot.h"
#include "axisrect.h"
#include "dp.h"
#include "trickmodel.h"
#include "monte.h"

class Plot : public QCustomPlot
{
    Q_OBJECT

public:
    explicit Plot(DPPlot *plot, QWidget* parent=0);
    void setData(Monte *monte);

private:
    DPPlot* _dpplot;
    AxisRect* _axisrect;

    
signals:
    
public slots:
    
};

#endif // PLOTPAGE_H
