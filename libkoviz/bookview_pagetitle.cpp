#include "bookview_pagetitle.h"

PageTitleView::PageTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Maximum));
}

void PageTitleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QPainter painter(viewport());
    _paintPageTitle(rootIndex(),painter);
}

// Note: title3 and title4 are fit beside legend or title1/2
QSize PageTitleView::sizeHint() const
{
    QSize size;

    size.setWidth(1400);

    QFont font(this->font());
    font.setPointSize(14); // title1
    QFontMetrics fm1(font);
    font.setPointSize(11); // title2
    QFontMetrics fm2(font);
    font.setPointSize(8);  // legend font
    QFontMetrics fml(font);

    int hl = _sizeLegend().height();

    int h = 0;
    h =  fm1.lineSpacing();      // Margin above Title1
    h += fm1.lineSpacing();      // Title1
    h += fm2.lineSpacing();      // Title2
    if ( hl > 0 ) {
        h += hl;                 // Legend
        h += fml.lineSpacing();  // Margin below legend
    }

    size.setHeight(h);

    return size;
}

void PageTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight,
                                const QVector<int> &roles)
{
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);

    if ( topLeft.column() != 1 ) return;

    QString tag = model()->data(topLeft.sibling(topLeft.row(),0)).toString();

    if ( tag == "PlotPresentation" ) {
        viewport()->update();
    } else if ( tag == "CurveColor") {
        viewport()->update();
    } else if ( tag == "CurveData") {
        viewport()->update();
        updateGeometry();
    }
}

QSize PageTitleView::_sizeLegend() const
{
    QSize size(0,0);

    if ( _bookModel()->isPageLegend(rootIndex()) ) {

        // Since _isPlotLegendsSame==true, use first plot for labels
        QModelIndex pageIdx = rootIndex();
        QModelIndexList plotIdxs = _bookModel()->plotIdxs(pageIdx);

        if ( !plotIdxs.isEmpty() ) {

            // TODO: Cut-n-pasted from BookIdxView::__paintPageLegend()
            QFont font = this->font();
            font.setPointSize(8);
            QFontMetrics fm(font);
            const int c = fm.averageCharWidth();
            const int v = fm.leading(); // vertical space between legend entries
            const int s = c;   // spaceBetweenLineAndLabel
            const int l = 5*c; // line width
            const int q = 5*c; // space between legend columns

            QModelIndex plotIdx = plotIdxs.at(0);
            QStringList labels = _bookModel()->legendLabels(plotIdx);
            int w = 0;
            int h = 0;
            if ( labels.size() == 2 ) {
                // 2 rows 1 column
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                w = qMax(w0,w1)+l+s;
                h = 2*fm.height()+v;
            } else if ( labels.size() == 3 ) {
                // 3 rows 1 column
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                w = qMax(qMax(w0,w1),w2)+l+s;
                h = 3*fm.height() + 2*v;
            } else if ( labels.size() == 4 ) {
                // 2 rows 2 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                w = qMax(w0,w1) + qMax(w2,w3) + 2*l + 2*s + q;
                h = 2*fm.height() + v;
            } else if ( labels.size() == 5 ) {
                // 3 rows 2 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                int w4 = fm.boundingRect(labels.at(4)).width();
                w = qMax(qMax(w0,w1),w2) + qMax(w3,w4) + 2*l + 2*s + q;
                h = 3*fm.height() + 2*v;
            } else if ( labels.size() == 6 ) {
                // 3 rows 2 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                int w4 = fm.boundingRect(labels.at(4)).width();
                int w5 = fm.boundingRect(labels.at(5)).width();
                w = qMax(qMax(w0,w1),w2) + qMax(qMax(w3,w4),w5) + 2*l + 2*s + q;
                h = 3*fm.height() + 2*v;
            } else if ( labels.size() == 7 ) {
                // 3 rows 3 columns
                int w0 = fm.boundingRect(labels.at(0)).width();
                int w1 = fm.boundingRect(labels.at(1)).width();
                int w2 = fm.boundingRect(labels.at(2)).width();
                int w3 = fm.boundingRect(labels.at(3)).width();
                int w4 = fm.boundingRect(labels.at(4)).width();
                int w5 = fm.boundingRect(labels.at(5)).width();
                int w6 = fm.boundingRect(labels.at(6)).width();
                w = qMax(qMax(w0,w1),w2) + qMax(qMax(w3,w4),w5) +
                    w6 + 3*l + 3*s + 2*q;
                h = 3*fm.height() + 2*v;
            }

            size.setWidth(w);
            size.setHeight(h);
        }
    }

    return size;
}
