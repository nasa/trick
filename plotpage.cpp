#include "plotpage.h"

PlotPage::PlotPage(DPPage *page, QWidget *parent) :
    QWidget(parent),
    _dppage(page)
{
    _grid = new QGridLayout(parent);
    _grid->setContentsMargins(0, 0, 0, 0);
    _grid->setSpacing(0);

    foreach (DPPlot* dpplot, _dppage->plots()) {
        _plots.append(new Plot(dpplot));
    }

    int npages = _dppage->plots().size();
    switch ( npages ) {
    case 1: _lay1(); break;
    case 6: _lay6(); break;
    }

    this->setLayout(_grid);
}

void PlotPage::setData(MonteModel *monteModel)
{
    foreach (Plot* plot, _plots) {
        plot->setData(monteModel);
    }
}

void PlotPage::_lay1()
{
    _grid->addWidget(_plots.at(0),0,0,1,1);
}

void PlotPage::_lay6()
{
    _grid->addWidget(_plots.at(0),0,0,1,1);
    _grid->addWidget(_plots.at(1),0,1,1,1);
    _grid->addWidget(_plots.at(2),1,0,1,1);
    _grid->addWidget(_plots.at(3),1,1,1,1);
    _grid->addWidget(_plots.at(4),2,0,1,1);
    _grid->addWidget(_plots.at(5),2,1,1,1);
}
