#include "bookview_xaxislabel.h"

XAxisLabelView::XAxisLabelView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    setAcceptDrops(true);
}

// See: https://www.walletfox.com/course/dragdroplistscene.php
void XAxisLabelView::dropEvent(QDropEvent *event)
{
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
            QModelIndex plotIdx = rootIndex();
            QModelIndex xAxisLabelIdx = _bookModel()->getDataIndex(plotIdx,
                                                               "PlotXAxisLabel",
                                                                        "Plot");
            if ( kovizType == "VarsModel" ) {
                QModelIndex curvesIdx = _bookModel()->getIndex(plotIdx,
                                                               "Curves","Plot");
                QModelIndexList curveIdxs = _bookModel()->getIndexList(
                                                              curvesIdx,
                                                              "Curve","Curves");
                bool isError = false;
                bool block = _bookModel()->blockSignals(true);
                foreach ( QModelIndex curveIdx, curveIdxs ) {
                    QModelIndex xNameIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveXName","Curve");
                    QModelIndex xUnitIdx = _bookModel()->getDataIndex(curveIdx,
                                                          "CurveXUnit","Curve");
                    QModelIndex curveDataIdx = _bookModel()->getDataIndex(
                                                           curveIdx,
                                                           "CurveData","Curve");
                    int runRow = _bookModel()->getDataInt(curveIdx,
                                                          "CurveRunID","Curve");
                    if ( runRow < 0 ) {
                        QMessageBox msgBox;
                        QString msg = QString("Sorry!  Unable to change x "
                                     "because y parameter is not from "
                                     "a RUN model e.g. x is from recorded data "
                                     "and y from the TV");
                        msgBox.setText(msg);
                        msgBox.exec();
                        isError = true;
                        break;
                    }
                    QString tName = _bookModel()->getDataString(curveIdx,
                                                      "CurveTimeName", "Curve");
                    QString yName = _bookModel()->getDataString(curveIdx,
                                                         "CurveYName", "Curve");
                    QString xName = dropString;
                    CurveModel* curveModel = _bookModel()->createCurve(runRow,
                                                             tName,xName,yName);
                    QString xUnit = curveModel->x()->unit();
                    _bookModel()->setData(xNameIdx,xName);
                    _bookModel()->setData(xUnitIdx,xUnit);
                    QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
                    _bookModel()->setData(curveDataIdx,v);
                }
                _bookModel()->blockSignals(block);

                if ( !isError ) {

                    _bookModel()->setData(xAxisLabelIdx,dropString);

                    // Reset plot math rect
                    QRectF bbox = _bookModel()->calcCurvesBBox(curvesIdx);
                    plotIdx = curvesIdx.parent();
                    QModelIndex pageIdx = plotIdx.parent().parent();
                    QModelIndex plotMathRectIdx = _bookModel()->getDataIndex(
                                                                 plotIdx,
                                                                 "PlotMathRect",
                                                                 "Plot");
                    QModelIndexList siblingPlotIdxs = _bookModel()->plotIdxs(
                                                                       pageIdx);
                    foreach ( QModelIndex siblingPlotIdx, siblingPlotIdxs ) {
                        bool isXTime = _bookModel()->isXTime(siblingPlotIdx);
                        if ( isXTime ) {
                            QRectF sibPlotRect = _bookModel()->
                                                getPlotMathRect(siblingPlotIdx);
                            if ( sibPlotRect.width() > 0 ) {
                                bbox.setLeft(sibPlotRect.left());
                                bbox.setRight(sibPlotRect.right());
                            }
                            break;
                        }
                    }
                    _bookModel()->setData(plotMathRectIdx,bbox);
                }

            } else if ( kovizType == "SieListModel" ) {
                // This signal propagates and is finally caught by trickview
                emit signalDropEvent(event,xAxisLabelIdx);
            }
        }
        event->accept();
    } else {
        event->ignore();
    }
  }

void XAxisLabelView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QRect R = viewport()->rect();
    QRect RG =  R;
    RG.moveTo(viewport()->mapToGlobal(RG.topLeft()));
    QRect  C = _curvesView->viewport()->rect();
    C.moveTo(_curvesView->viewport()->mapToGlobal(C.topLeft()));
    QRectF M = _bookModel()->getPlotMathRect(rootIndex());
    QPainter painter(viewport());
    XAxisLabelLayoutItem layoutItem(fontMetrics(),_bookModel(),rootIndex());
    layoutItem.paint(&painter,R,RG,C,M);
}

QSize XAxisLabelView::minimumSizeHint() const
{
    return sizeHint();
}

QSize XAxisLabelView::sizeHint() const
{
    QSize s;
    XAxisLabelLayoutItem layoutItem(fontMetrics(),_bookModel(),rootIndex());
    s = layoutItem.sizeHint();
    return s;
}

// TODO: Change curve units based on current unit
void XAxisLabelView::wheelEvent(QWheelEvent *e)
{
    Q_UNUSED(e);
    return;
}

void XAxisLabelView::dataChanged(const QModelIndex &topLeft,
                                 const QModelIndex &bottomRight)
{
    if ( !model()) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft.parent() != rootIndex() ) return;
    if ( topLeft != bottomRight ) return; // TODO: support multiple changes
    QModelIndex tagIdx = model()->index(topLeft.row(),0,topLeft.parent());
    QString tag = model()->data(tagIdx).toString();
    if ( tag != "PlotXAxisLabel" ) return;

    viewport()->update(); // important to use viewport()->update(), not update()
                          // since refresh will not be immediate with update()
}

void XAxisLabelView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( rootIndex() != pidx ) return;
    if ( !pidx.isValid() ) return;
    if ( !model()) return;

    for ( int i = start; i <= end; ++i ) {
        QModelIndex idx = model()->index(i,0,pidx);
        if ( model()->data(idx).toString() == "PlotXAxisLabel" ) {
            idx = model()->sibling(i,1,idx);
            viewport()->update();
            break;
        }
    }
}
