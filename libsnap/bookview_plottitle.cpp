#include "bookview_plottitle.h"

PlotTitleView::PlotTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);

    _vBoxLayout = new QVBoxLayout;
    _vBoxLayout->setContentsMargins(0,0,0,0);
    _vBoxLayout->setSpacing(0);

    _label = new QLabel(this);
    _label->setText("No Model");
    _label->setAlignment(Qt::AlignHCenter| Qt::AlignBottom);
    _vBoxLayout->addWidget(_label);

    setLayout(_vBoxLayout);
}

void PlotTitleView::_update()
{
    PlotBookModel* bookModel = _bookModel();

    if ( !bookModel->isChildIndex(_myIdx,"Plot","PlotTitle") ) {
        return;
    }

    QModelIndex plotTitleIdx  = bookModel->getIndex(_myIdx, "PlotTitle","Plot");
    plotTitleIdx = bookModel->index(plotTitleIdx.row(),1,_myIdx);
    QString plotTitle = bookModel->data(plotTitleIdx).toString();

    _label->setText(plotTitle);
}


// TODO: For now and only handle single item changes
void PlotTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    if ( topLeft.parent() != _myIdx ) return;

    // Value is in column 1
    if ( topLeft.column() != 1 ) return;

    // TODO: For now and only handle single item changes
    if ( topLeft != bottomRight ) return;

    _update();
}

// TODO: only single append works
void PlotTitleView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( pidx != _myIdx ) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        QString cText = model()->data(idx).toString();
        if ( cText == "PlotTitle" ) {
            _update();
        }
    }

    return;
}


QSize PlotTitleView::minimumSizeHint() const
{
    return sizeHint();
}

QSize PlotTitleView::sizeHint() const
{
    QSize sz;

    QFontMetrics fm = _label->fontMetrics();
    QRect bb = fm.boundingRect(_label->text());
    sz = bb.size();
    return sz;
}
