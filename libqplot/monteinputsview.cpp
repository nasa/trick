#include "monteinputsview.h"
#include <QHeaderView>

MonteInputsView::MonteInputsView(BookView *bookView,
                                 QWidget *parent) :
    QTableView(parent),
    _bookView(bookView)
{
    setSortingEnabled(true);
    sortByColumn(0,Qt::AscendingOrder);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setTextElideMode(Qt::ElideNone);

    QHeaderView* headerView = horizontalHeader();
    headerView->setTextElideMode(Qt::ElideLeft);
    headerView->setResizeMode(QHeaderView::ResizeToContents);
    connect(headerView,SIGNAL(sectionClicked(int)),
            this,SLOT(_viewHeaderSectionClicked(int)));

}

void MonteInputsView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);

    _selectModel = new QItemSelectionModel(model);
    setSelectionModel(_selectModel);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setFocusPolicy(Qt::ClickFocus);
    setTabKeyNavigation(false);
    verticalHeader()->hide();
    connect(_selectModel,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(_selectModelCurrentChanged(QModelIndex,QModelIndex)));
}

int MonteInputsView::currSelectedRun()
{
    int runId = -1;
    QItemSelection monteInputSel =  selectionModel()->selection();
    if ( monteInputSel.size() > 0 ) {
        QModelIndex selIdx = monteInputSel.indexes().at(0);
        bool ok = false;
        runId = model()->data(selIdx).toInt(&ok);
        if ( !ok ) {
            runId = -1;
        }
    }
    return runId;
}

void MonteInputsView::_selectModelCurrentChanged(
                     const QModelIndex &curr, const QModelIndex &prev)
{
    Q_UNUSED(prev);

    if ( ! curr.model() ) return ;

    QModelIndex runIdx = curr.model()->index(curr.row(),0);
    int runId = curr.model()->data(runIdx).toInt();
#if 0
    _plotBookView->selectRun(runId);
#endif
}


void MonteInputsView::_viewHeaderSectionClicked(int section)
{
    Q_UNUSED(section);

    QModelIndexList selIdxs = _selectModel->selectedIndexes();
    if ( selIdxs.size() > 0 ) {
        QModelIndex idx = selIdxs.at(0);
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }
}
