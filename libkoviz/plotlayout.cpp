#include "plotlayout.h"

PlotLayout::PlotLayout() : QLayout()
{
}

PlotLayout::~PlotLayout()
{
}

void PlotLayout::addWidget(QWidget *widget)
{
    QWidgetItem* item = new QWidgetItem(widget);
    _name2item.insert(widget->whatsThis(),item);
    _items << item;
}

//==================================================
//  Layout
//
//  14 parts in a single plot
//  6 rows and 5 columns
//
//    0    1    2            3               4
//  +---++---++---++----------------------++---+
//  |   ||   || C ||        Tics          || C |   0
//  |   ||   |+---++----------------------++---+
//  |   || Y |+---++----------------------++---+
//  | Y ||   ||   ||      Title           ||   |   1
//  |   || T ||   |+----------------------+|   |
//  | A || i ||   |+----------------------+|   |
//  | x || c || T ||                      || T |
//  | i ||   || i ||                      || i |
//  | s || L || c ||        Curves        || c |   2
//  |   || a || s ||                      || s |
//  | L || b ||   ||                      ||   |
//  | a || e ||   ||                      ||   |
//  | b || l ||   ||                      ||   |
//  | e || s |+---++----------------------++---+
//  | l ||   |+---++-----------------------+---+
//  |   ||   || C ||       Tics            | C |   3
//  +---++---++---++-----------------------+---+
//  +------------------------------------------+
//  |                 X Tic Labels             |   4
//  +------------------------------------------+
//  +--------++--------------------------------+
//  | Nothing||         X Axis Label           |   5
//  +--------++--------------------------------+
//
void PlotLayout::setGeometry(const QRect &rect)
{
    int w = rect.width();
    int h = rect.height();
    int w0 = _name2item.value("yAxisLabel")->sizeHint().width();
    int w1 = _name2item.value("yTicLabels")->sizeHint().width();
    int w2 = _name2item.value("tlCorner")->sizeHint().width();
    int w4 = _name2item.value("trCorner")->sizeHint().width();
    int w3 = w-(w0+w1+w2+w4);
    int h0 = _name2item.value("tlCorner")->sizeHint().height();
    int h1 = _name2item.value("titleView")->sizeHint().height();
    int h3 = _name2item.value("brCorner")->sizeHint().height();
    int h4 = _name2item.value("xTicLabels")->sizeHint().height();
    int h5 = _name2item.value("xAxisLabel")->sizeHint().height();
    int h2 = h-(h0+h1+h3+h4+h5);

    int mx = 0;
    int my = 0;
    if ( _plotRatio == "square" ) {
        if ( w3 > h2 ) {
            w3 = h2;
            mx = (w-(w0+w1+w2+w3+w4))/2;
        } else {
            h2 = w3;
            my = (h-(h0+h1+h2+h3+h4+h5))/2;
        }
    }

    _name2item.value("yAxisLabel")->setGeometry(QRect(mx,0,my+w0,h-(h4+h5)));
    _name2item.value("yTicLabels")->setGeometry(QRect(mx+w0,my, w1,h-(h4+h5)));
    _name2item.value("titleView")->setGeometry(QRect(mx+w0+w1+w2,my+h0,w3,h1));
    _name2item.value("curvesView")->setGeometry(QRect(mx+w0+w1+w2,my+h0+h1,
                                                      w3,h2));
    _name2item.value("tlCorner")->setGeometry(QRect(mx+w0+w1,my,w2,h0));
    _name2item.value("trCorner")->setGeometry(QRect(mx+w0+w1+w2+w3,my,w4,h0));
    _name2item.value("brCorner")->setGeometry(QRect(mx+w0+w1+w2+w3,
                                                    my+h0+h1+h2,
                                                    w4,h3));
    _name2item.value("blCorner")->setGeometry(QRect(mx+w0+w1,my+h0+h1+h2,
                                                    w2,h3));
    _name2item.value("lTics")->setGeometry(QRect(mx+w0+w1,my+h0,w2,h1+h2));
    _name2item.value("tTics")->setGeometry(QRect(mx+w0+w1+w2,my,w3,h0));
    _name2item.value("rTics")->setGeometry(QRect(mx+w0+w1+w2+w3,my+h0,
                                                 w4,h1+h2));
    _name2item.value("bTics")->setGeometry(QRect(mx+w0+w1+w2,my+h0+h1+h2,
                                                 w3,h3));
    _name2item.value("xTicLabels")->setGeometry(QRect(mx,my+h0+h1+h2+h3,
                                                      w-2*mx,h4));
    _name2item.value("xAxisLabel")->setGeometry(QRect(mx+w0+w1,
                                                      my+h0+h1+h2+h3+h4,
                                                      w-w0-w1-2*mx,h5));

    _rect = rect;
}

QRect PlotLayout::geometry() const
{
    return _rect;
}

QSize PlotLayout::sizeHint() const
{
    return QSize();
}

int PlotLayout::count() const
{
    return _items.size();
}

QLayoutItem *PlotLayout::itemAt(int index) const
{
    return _items.value(index,0);
}

// Hard-coded for the pdf case.  It expects items to be inserted in order.
void PlotLayout::addItem(QLayoutItem *item)
{
    int i = _items.size();

    switch (i) {
        case 0:  _name2item.insert("yAxisLabel",item); break;
        case 1:  _name2item.insert("yTicLabels",item); break;
        case 2:  _name2item.insert("titleView",item);  break;
        case 3:  _name2item.insert("curvesView",item); break;
        case 4:  _name2item.insert("tlCorner",item); break;
        case 5:  _name2item.insert("trCorner",item); break;
        case 6:  _name2item.insert("brCorner",item); break;
        case 7:  _name2item.insert("blCorner",item); break;
        case 8:  _name2item.insert("lTics",item); break;
        case 9:  _name2item.insert("tTics",item); break;
        case 10: _name2item.insert("rTics",item); break;
        case 11: _name2item.insert("bTics",item); break;
        case 12: _name2item.insert("xTicLabels",item); break;
        case 13: _name2item.insert("xAxisLabel",item); break;
    }

    _items << item;
}

QLayoutItem *PlotLayout::takeAt(int index)
{
    Q_UNUSED(index);
    fprintf(stderr, "koviz [bad scoobs]: PlotLayout::takeAt() "
                    "not implemented!\n");
    exit(-1);
    return 0;
}

QLayout *PlotLayout::layout()
{
    return this;
}

void PlotLayout::setPlotRatio(const QString &plotRatio)
{
    _plotRatio = plotRatio;
}

QString PlotLayout::plotRatio() const
{
    return _plotRatio;
}
