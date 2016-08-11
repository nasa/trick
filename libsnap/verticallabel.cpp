#include "verticallabel.h"

VerticalLabel::VerticalLabel(KPlotModel *kPlotModel,
                             const QModelIndex &plotIdx,
                             QWidget *parent) :
    QLabel(parent),
    _kPlotModel(kPlotModel),
    _plotIdx(plotIdx),
    _yAxisItem(0)
{
    PlotBookModel* bookModel = kPlotModel->bookModel();
    connect(bookModel,SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(rowsInserted(QModelIndex,int,int)));
}

void VerticalLabel::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    if ( !_yAxisItem ) return;

    QPainter painter(this);
    painter.translate( width()/2, height()/2 );
    painter.rotate(270);
    painter.drawText(0,0, _yAxisItem->data().toString());
}

QSize VerticalLabel::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    return QSize(s.height(), s.width());
}

QSize VerticalLabel::sizeHint() const
{
    QSize s = QLabel::sizeHint();
    return QSize(s.height(), s.width());
}

void VerticalLabel::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    const QAbstractItemModel* model = pidx.model();

    if ( !model ) return;
    if ( model != (QAbstractItemModel*)(_kPlotModel->bookModel()) ) return;
    if ( pidx != _plotIdx ) return;

    PlotBookModel* bookModel = _kPlotModel->bookModel();
    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = bookModel->index(i,0,pidx);
        QStandardItem* item = bookModel->itemFromIndex(idx);
        if ( item->text() == "PlotYAxisLabel" ) {
            _yAxisItem = item;
            break;
        }
    }

}
