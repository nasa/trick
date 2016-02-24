#ifndef KOVI_H
#define KOVI_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QPair>
#include <QPersistentModelIndex>
#include <QDebug>

#include <math.h>
#include <stdio.h>
#include <float.h>

#include "kplotmodel.h"
#include "kplot.h"
#include "verticallabel.h"
#include "linedruler.h"
#include "labeledruler.h"
#include "plotcorner.h"
#include "libqplot/plotbookmodel.h"

class KoviPlot : public QFrame
{
    Q_OBJECT
public:
    explicit KoviPlot(PlotBookModel* bookModel,
                      const QModelIndex& plotIdx,
                      QWidget *parent = 0);
    
signals:
    
public slots:

private:
    PlotBookModel* _bookModel;
    QPersistentModelIndex _plotIdx;
    QGridLayout* _layout;
    QLabel* _yAxisLabel;
};

#endif // KOVIPLOT_H
