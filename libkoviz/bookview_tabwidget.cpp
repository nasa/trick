#include "bookview_tabwidget.h"

BookViewTabWidget::BookViewTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setAcceptDrops(true);
    parent->setAcceptDrops(true);
}

void BookViewTabWidget::setBookModel(PlotBookModel *bookModel)
{
    _bookModel = bookModel;
}

void BookViewTabWidget::dropEvent(QDropEvent *event)
{
    if ( count() == 0 ) {
        // Dropping on empty tab widget - create time-vs-yparam plot on new page
        QString mimeType("application/x-qabstractitemmodeldatalist");
        if ( event->mimeData()->hasFormat(mimeType) ) {
            QByteArray bytes = event->mimeData()->data(mimeType);
            QDataStream stream(&bytes,QIODevice::ReadOnly);
            int row,col;
            QMap<int, QVariant> valueMap;
            stream >> row >> col >> valueMap;
            if ( !valueMap.isEmpty() ) {
                QString dropString = valueMap.value(0).toString();
                QStandardItem* pageItem = _bookModel->createPageItem();
                QString hardCodedTime("sys.exec.out.time"); // TODO:
                                                            // time needs to be
                                                            // in book model
                QStringList emptyUnitOverrides; // TODO: unitoverrides needs
                                                //to be in book model
                QStandardItem* plotItem = _bookModel->createPlotItem(
                                                   pageItem,
                                                "sys.exec.out.time",
                                                 dropString,
                                                 emptyUnitOverrides,this);
                Q_UNUSED(plotItem);
            }
        }
    }
}

void BookViewTabWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
