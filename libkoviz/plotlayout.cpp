#include "plotlayout.h"

PlotLayout::PlotLayout() : QLayout()
{
}

PlotLayout::~PlotLayout()
{
}

void PlotLayout::addWidget(QWidget *widget)
{
    _name2widget.insert(widget->whatsThis(),widget);
    _items << new QWidgetItem(widget);
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
    int w0 = _name2widget.value("yAxisLabel")->sizeHint().width();
    int w1 = _name2widget.value("yTicLabels")->sizeHint().width();
    int w2 = _name2widget.value("tlCorner")->sizeHint().width();
    int w4 = _name2widget.value("trCorner")->sizeHint().width();
    int w3 = w-(w0+w1+w2+w4);
    int h0 = _name2widget.value("tlCorner")->sizeHint().height();
    int h1 = _name2widget.value("titleView")->sizeHint().height();
    int h3 = _name2widget.value("brCorner")->sizeHint().height();
    int h4 = _name2widget.value("xTicLabels")->sizeHint().height();
    int h5 = _name2widget.value("xAxisLabel")->sizeHint().height();
    int h2 = h-(h0+h1+h3+h4+h5);

    _name2widget.value("yAxisLabel")->setGeometry(QRect(0,0,w0,h-(h4+h5)));
    _name2widget.value("yTicLabels")->setGeometry(QRect(w0,0,w1,h-(h4+h5)));
    _name2widget.value("titleView")->setGeometry(QRect(w0+w1+w2,h0,w3,h1));
    _name2widget.value("curvesView")->setGeometry(QRect(w0+w1+w2,h0+h1,w3,h2));
    _name2widget.value("tlCorner")->setGeometry(QRect(w0+w1,0,w2,h0));
    _name2widget.value("trCorner")->setGeometry(QRect(w-w4,0,w4,h0));
    _name2widget.value("brCorner")->setGeometry(QRect(w-w4,h0+h1+h2,w4,h3));
    _name2widget.value("blCorner")->setGeometry(QRect(w0+w1,h0+h1+h2,w2,h3));
    _name2widget.value("lTics")->setGeometry(QRect(w0+w1,h0,w2,h1+h2));
    _name2widget.value("tTics")->setGeometry(QRect(w0+w1+w2,0,w3,h0));
    _name2widget.value("rTics")->setGeometry(QRect(w0+w1+w2+w3,h0,w4,h1+h2));
    _name2widget.value("bTics")->setGeometry(QRect(w0+w1+w2,h0+h1+h2,w3,h3));
    _name2widget.value("xTicLabels")->setGeometry(QRect(0,h0+h1+h2+h3,w,h4));
    _name2widget.value("xAxisLabel")->setGeometry(QRect(w1+w2,h-h5,w,h5));
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

void PlotLayout::addItem(QLayoutItem *item)
{
    Q_UNUSED(item);
    fprintf(stderr, "koviz [bad scoobs]: PlotLayout::addItem() "
                    "not implemented!\n");
    exit(-1);
}

QLayoutItem *PlotLayout::takeAt(int index)
{
    Q_UNUSED(index);
    fprintf(stderr, "koviz [bad scoobs]: PlotLayout::takeAt() "
                    "not implemented!\n");
    exit(-1);
    return 0;
}
