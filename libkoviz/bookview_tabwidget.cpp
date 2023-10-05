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
                QString kovizType(event->mimeData()->data("koviz-model-type"));
                if ( kovizType.isEmpty() ) {
                    kovizType = valueMap.value(Qt::UserRole).toString();
                }
                if ( kovizType == "VarsModel" ) {
                    // TODO:time and unit overrides needs to be in book model
                    QString hardCodedTime("sys.exec.out.time");
                    QStringList emptyUnitOverrides;
                    QStandardItem* pageItem = _bookModel->createPageItem();
                    QStandardItem* plotItem = _bookModel->createPlotItem(
                                                             pageItem,
                                                             hardCodedTime,
                                                             dropString,
                                                             emptyUnitOverrides,
                                                             this);
                    Q_UNUSED(plotItem);
                } else if ( kovizType == "SieListModel" ) {
                    QMessageBox msgBox;
                    QString msg = QString("TODO: Drag-n-drop of TV params!");
                    msgBox.setText(msg);
                    msgBox.exec();
                    fprintf(stderr, "%s\n", msg.toLatin1().constData());
                    fprintf(stderr, "TV dropped param=%s\n",
                                    dropString.toLatin1().constData());

                }
            }
        }
    }
}

void BookViewTabWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
