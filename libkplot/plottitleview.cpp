#include "plottitleview.h"

PlotTitleView::PlotTitleView(QWidget *parent) :
    QAbstractItemView(parent),
    _plotTitleIdx(QModelIndex())
{
    _vBoxLayout = new QVBoxLayout;

    _label = new QLabel(this);
    _label->setText("No Model");
    _label->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    _vBoxLayout->addWidget(_label);

    setLayout(_vBoxLayout);
}

void PlotTitleView::setModel(QAbstractItemModel *model)
{
    if ( model == this->model() ) {
        QAbstractItemView::setModel(model);
        return;
    }

    // Allow setting model to null
    if ( !model ) {
        _plotTitleIdx = QModelIndex();
        QAbstractItemView::setModel(model);
        return;
    }

    PlotBookModel* bookModel = dynamic_cast<PlotBookModel*>(model);
    if ( !bookModel ) {
        qDebug() << "snap [bad scoobs]: PlotTitleView::setModel() "
                    "could not cast model at address"
                 << (void*)model
                 << "to a PlotBookModel";
        exit(-1);
    }

    if ( _plotTitleIdx.isValid() ) {
        if ( _plotTitleIdx.model() != model ) {
            qDebug() << "snap [bad scoobs]: PlotTitleView::setModel() "
                        "is being set to a model that is not the same "
                        "as _plotTitleIdx.model()"
                     << (void*)model
                     << "to a PlotBookModel";
            exit(-1);
        }
    }

    QAbstractItemView::setModel(model);
}

void PlotTitleView::setRootIndex(const QModelIndex &plotIdx)
{
    if( !plotIdx.isValid() ) {
        QAbstractItemView::setRootIndex(plotIdx);
        return;
    }

    if ( plotIdx.model() == 0 ) {
        qDebug() << "snap [bad scoobs]: PlotTitleView::setRootIndex() "
                 << "to a plotIdx with null model";
        exit(-1);
    }

    if ( !model() ) {
        qDebug() << "snap [bad scoobs]: PlotTitleView::setRootIndex() "
                    "called without setting the view model. "
                    "You must call view->setModel() before setting the "
                    "page index";
        exit(-1);
    }

    if ( plotIdx.model() != this->model() ) {
        qDebug() << "snap [bad scoobs]: PlotTitleView::setRootIndex() "
                    "set to index with model which does not match "
                    "view->model().  Please setModel() to model that "
                    "is same as plotIdx's model.";
        exit(-1);
    }

    PlotBookModel* bookModel = dynamic_cast<PlotBookModel*>(model());
    if ( !bookModel ) {
        qDebug() << "snap [bad scoobs]: PlotTitleView::setRootIndex() "
                    "could not cast model at address"
                 << (void*)model()
                 << "to a PlotBookModel";
        exit(-1);
    }

    if ( model()->data(plotIdx).toString() != "Plot" ) {
        qDebug() << "snap [bad scoobs]: PlotTitleView::setRootIndex() "
                    "given plotIdx, but parent is not a PlotIdx.";
        exit(-1);
    }

    _plotIdx = plotIdx;

    if ( bookModel->isChildIndex(plotIdx,"Plot","PlotTitle") ) {
        _plotTitleIdx = bookModel->getIndex(plotIdx,"PageTitle","Page");
        _plotTitleIdx = model()->index(_plotTitleIdx.row(),1,plotIdx);
        QString title = model()->data(_plotTitleIdx).toString();
        _label->setText(title);
    }

}

// TODO: For now and only handle single item changes
void PlotTitleView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight)
{
    if ( topLeft != _plotTitleIdx ) return;

    QString title = model()->data(topLeft).toString();
    _label->setText(title);
}

// TODO: only single append works
void PlotTitleView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( pidx != _plotIdx ) return;

    QModelIndex cidx = model()->index(start,0,pidx);
    if ( model()->data(cidx).toString() != "PlotTitle" ) return;

    _plotTitleIdx = model()->index(start,1,pidx);

    PlotBookModel* bookModel = dynamic_cast<PlotBookModel*>(model());
    if ( !bookModel ) {
        qDebug() << "snap [bad scoobs]: PlotTitleView::rowsInserted() "
                    "could not cast model at address"
                 << (void*)model()
                 << "to a PlotBookModel";
        exit(-1);
    }

    QString title = model()->data(_plotTitleIdx).toString();
    _label->setText(title);

    return;
}

QModelIndex PlotTitleView::indexAt(const QPoint &point) const
{
    QModelIndex idx;
    return idx;
}

QRect PlotTitleView::visualRect(const QModelIndex &index) const
{
}

void PlotTitleView::scrollTo(const QModelIndex &index,
                             QAbstractItemView::ScrollHint hint)
{
}

QModelIndex PlotTitleView::moveCursor(
        QAbstractItemView::CursorAction cursorAction,
        Qt::KeyboardModifiers modifiers)
{
}

int PlotTitleView::horizontalOffset() const
{
    return 0;
}

int PlotTitleView::verticalOffset() const
{
    return 0;
}

// TODO: the index can be hidden
bool PlotTitleView::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

void PlotTitleView::setSelection(const QRect &rect,
                                 QItemSelectionModel::SelectionFlags command)
{
}

QRegion PlotTitleView::visualRegionForSelection(
        const QItemSelection &selection) const
{
}
