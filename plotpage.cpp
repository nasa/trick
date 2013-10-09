#include "plotpage.h"

//_plot_monte->plotLayout()->clear(); // let's start from scratch and remove the default axis rect
//_plot_monte->plotLayout()->addElement(1, 0, new QCPAxisRect(_plot_monte));
//QCPLayoutGrid *subLayout = new QCPLayoutGrid;
//_plot_monte->plotLayout()->addElement(0, 0, subLayout);
//subLayout->addElement(0, 0, new QCPAxisRect(_plot_monte));
//subLayout->addElement(0, 1, new QCPAxisRect(_plot_monte));
//subLayout->setColumnStretchFactor(0, 3); // left axis rect shall have 60% of width
//subLayout->setColumnStretchFactor(1, 2); // right one only 40% (3:2 = 60:40)

PlotPage::PlotPage(DPPage* page, QWidget* parent) :
    QCustomPlot(parent),
    _dppage(page)
{
    setInteractions(QCP::iRangeDrag | QCP::iSelectAxes |
                    QCP::iSelectLegend | QCP::iSelectPlottables);

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);

    setNoAntialiasingOnDrag(true);
    setAutoAddPlottableToLegend(false);

    _lay6(page);
}

void PlotPage::setData(Monte *monte)
{
    foreach (Plot* plot, _plots) {
        plot->setData(monte);
        plot->zoomToFit();
    }
}

void PlotPage::mousePressEvent(QMouseEvent *event)
{
    QCustomPlot::mousePressEvent(event);
}

void PlotPage::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);
}

void PlotPage::mouseReleaseEvent(QMouseEvent *event)
{
    QCustomPlot::mouseReleaseEvent(event);
}

void PlotPage::_lay6(DPPage *page)
{
    plotLayout()->clear();
    plotLayout()->insertColumn(0);
    plotLayout()->insertRow(0);
    plotLayout()->insertRow(1);

    for ( int ii = 0; ii < page->plots().size(); ++ii) {
        _plots.append(new Plot(page->plots().at(ii),this));
    }

    plotLayout()->addElement(0,0,_plots.at(0));
    plotLayout()->addElement(0,1,_plots.at(1));
    plotLayout()->addElement(1,0,_plots.at(2));
    plotLayout()->addElement(1,1,_plots.at(3));
    plotLayout()->addElement(2,0,_plots.at(4));
    plotLayout()->addElement(2,1,_plots.at(5));
}
