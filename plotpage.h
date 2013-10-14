#ifndef PLOTPAGE_H
#define PLOTPAGE_H

#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QList>
#include "dp.h"
#include "monte.h"
#include "plot.h"

class PlotPage : public QWidget
{
    Q_OBJECT
public:
    explicit PlotPage(DPPage* page, QWidget *parent = 0);
    void setData(Monte *monte);

signals:
    
public slots:

private:
    QList<Plot*> _plots;
    DPPage* _dppage;
    QGridLayout* _grid;
    void _lay1();
    void _lay6();
};

#endif // PLOTPAGE_H
