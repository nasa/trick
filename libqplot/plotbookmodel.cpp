#include "plotbookmodel.h"
#include <float.h>
#include "libsnapdata/unit.h"

PlotBookModel::PlotBookModel(MonteModel *monteModel, QObject *parent) :
    QStandardItemModel(parent),
    _monteModel(monteModel)
{
    _initModel();
}

PlotBookModel::PlotBookModel(MonteModel *monteModel,
                             int rows, int columns, QObject *parent) :
    QStandardItemModel(rows,columns,parent),
    _monteModel(monteModel)
{
    _initModel();
}

QVariant PlotBookModel::data(const QModelIndex &idx, int role) const
{
    Q_UNUSED(role);

    QVariant v;

    if ( isIndex(idx, "CurveData") ) {

        QModelIndex curveIdx = idx.parent();
        QModelIndex rIdx = getIndex(curveIdx, "CurveRunID", "Curve");
        QModelIndex xIdx = getIndex(curveIdx, "CurveXName", "Curve");
        QModelIndex yIdx = getIndex(curveIdx, "CurveYName", "Curve");
        int runID = itemFromIndex(rIdx)->data().toInt();
        QString xName = itemFromIndex(xIdx)->data().toString();
        QString yName = itemFromIndex(yIdx)->data().toString();

        TrickCurveModel* curveModel = _monteModel->curve(runID,
                                                         xName,
                                                         yName);

        //
        // xunit and calc x scale if DP unit not equal to model unit
        //
        bool isXScale = false;
        double xScaleFactor = 1.0;
        QString xunit = curveModel->x().unit();
        QModelIndex xUnitIdx = getIndex(curveIdx, "CurveXUnit", "Curve");
        QString xDPUnit = data(xUnitIdx).toString();
        if ( !xDPUnit.isEmpty() && xunit != xDPUnit &&
             xDPUnit != "--" && xunit != "--" ) {
            isXScale = true;
            xScaleFactor = Unit::convert(1.0,
                                         xunit.toAscii().constData(),
                                         xDPUnit.toAscii().constData());
            xunit = xDPUnit;
        }


        //
        // yunit and calc y scale if DP unit not equal to model unit
        //
        bool isYScale = false;
        double yScaleFactor = 1.0;
        QString yunit = curveModel->y().unit();
        QModelIndex yUnitIdx = getIndex(curveIdx, "CurveYUnit", "Curve");
        QString yDPUnit = data(yUnitIdx).toString();
        if ( !yDPUnit.isEmpty() &&
             yunit != yDPUnit && yDPUnit != "--" && yunit != "--" ) {
            isYScale = true;
            yScaleFactor = Unit::convert(1.0,
                                         yunit.toAscii().constData(),
                                         yDPUnit.toAscii().constData());
            yunit = yDPUnit;
        }

        // Set scale factor for either x or y units
        if ( isXScale || isYScale ) {
            delete curveModel;
            curveModel = _monteModel->curve(runID,
                                            xName, yName,
                                            xScaleFactor, yScaleFactor);
        }

        v = PtrToQVariant<TrickCurveModel>::convert(curveModel);

    } else {
        v = itemFromIndex(idx)->data();
    }

    return v;
}

bool PlotBookModel::setData(const QModelIndex &index,
                            const QVariant &value, int role)
{
    Q_UNUSED(role);
    bool ret = false;

    if ( index.isValid() ) {
        QStandardItem* item = itemFromIndex(index);
        item->setData(value);
        ret = true;
    }

    return ret;
}

QStandardItem *PlotBookModel::addChild(QStandardItem *parentItem,
                                       const QString &childTitle,
                                       const QVariant &childValue)
{
    QStandardItem *childItem = new QStandardItem(childTitle);
    childItem->setData(childValue);
    parentItem->appendRow(childItem);
    return childItem;
}

// Returns page index for idx
// If idx is a child of a page, it will return the parent page index
QModelIndex PlotBookModel::_pageIdx(const QModelIndex& idx) const
{
    QModelIndex pgIdx;

    QModelIndex p1Idx = idx.parent();
    QModelIndex p2Idx = p1Idx.parent();
    QModelIndex p3Idx = p2Idx.parent();
    QModelIndex p4Idx = p3Idx.parent();

    if ( isIndex(idx, "Page" ) ) {
        pgIdx = idx;
    } else if ( isIndex(p1Idx, "Page" ) ) {
        pgIdx = p1Idx;
    } else if ( isIndex(p2Idx, "Page" ) ) {
        pgIdx = p2Idx;
    } else if ( isIndex(p3Idx, "Page" ) ) {
        pgIdx = p3Idx;
    } else if ( isIndex(p4Idx, "Page" ) ) {
        pgIdx = p4Idx;
    }

    return pgIdx;
}


QModelIndexList PlotBookModel::pageIdxs() const
{
    QModelIndexList idxs;
    QModelIndex pgsIdx = getIndex(QModelIndex(), "Pages");
    int rc = rowCount(pgsIdx);
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex idx = index(i,0,pgsIdx);
        idxs.append(idx);
    }
    return idxs;
}

QStandardItem *PlotBookModel::plotsItem(QStandardItem *pageItem) const
{

    QModelIndex pageIdx = indexFromItem(pageItem);
    QModelIndex plotsIdx = getIndex(pageIdx, "Plots", "Page");
    QStandardItem* plotsItem = itemFromIndex(plotsIdx);
    return plotsItem;
}

QModelIndex PlotBookModel::plotIdx(const QModelIndex &idx) const
{
    QModelIndex pltIdx;

    QModelIndex p1Idx = idx.parent();
    QModelIndex p2Idx = p1Idx.parent();
    QModelIndex p3Idx = p2Idx.parent();

    if ( isIndex(idx, "Plot" ) ) {
        pltIdx = idx;
    } else if ( isIndex(p1Idx, "Plot" ) ) {
        pltIdx = p1Idx;
    } else if ( isIndex(p2Idx, "Plot" ) ) {
        pltIdx = p2Idx;
    } else if ( isIndex(p3Idx, "Plot" ) ) {
        pltIdx = p3Idx;
    }

    return pltIdx;
}

QModelIndexList PlotBookModel::plotIdxs(const QModelIndex &pageIdx) const
{
    QModelIndexList idxs;
    QModelIndex pltsIdx = getIndex(pageIdx, "Plots", "Page");
    int rc = rowCount(pltsIdx);
    for ( int i = 0 ; i < rc; ++i ) {
        idxs.append(index(i,0,pltsIdx));
    }

    return idxs;
}

QModelIndexList PlotBookModel::curveIdxs(const QModelIndex &curvesIdx) const
{
    QModelIndexList idxs;
    int rc = rowCount(curvesIdx);
    for ( int i = 0 ; i < rc; ++i ) {
        idxs.append(index(i,0,curvesIdx));
    }

    return idxs;
}

QModelIndex PlotBookModel::pageBGColorIndex(const QModelIndex& idx) const
{
    QModelIndex pgIdx = _pageIdx(idx);
    return getIndex(pgIdx, "PageBackgroundColor", "Page");
}

void PlotBookModel::_initModel()
{
    QStandardItem *rootItem = invisibleRootItem();

    QStandardItem *startItem = new QStandardItem("SessionStartTime");
    startItem->setData(-DBL_MAX);
    rootItem->appendRow(startItem);

    QStandardItem *stopItem = new QStandardItem("SessionStopTime");
    stopItem->setData(DBL_MAX);
    rootItem->appendRow(stopItem);

    QStandardItem *pagesItem = new QStandardItem("Pages");
    pagesItem->setData("Pages");
    rootItem->appendRow(pagesItem);
}

//
// Set expectedStartIdxText in calling routine to check to make sure
// startIdx's text is correct.  It's more for debugging, but helps
// ensure that one is in sync with the tree.
//
// StartIdx is normally the immediate parent of the item with searchItemText
// The odd case is for Pages and Plots.  The search can go ancestorily (sp?).
// For instance:
//     getIndex(curveIdx, "Plot")
// will return the plotIdx that is the parent of curveIdx
//
QModelIndex PlotBookModel::getIndex(const QModelIndex &startIdx,
                                 const QString &searchItemText,
                                 const QString &expectedStartIdxText) const
{
    QModelIndex idx;

    // For Page, if the parent isn't "Pages" the search goes up the tree
    if ( searchItemText == "Page" ) {
        idx = _pageIdx(startIdx);
        return idx;
    }

    if ( !expectedStartIdxText.isEmpty() ) {
        if ( startIdx.isValid() ) {
            if ( !isIndex(startIdx, expectedStartIdxText) ) {
                QStandardItem* startItem = itemFromIndex(startIdx);
                QString startText = startItem->text();
                qDebug() << "snap [bad scoobies]: getIndex() received a "
                            "startIdx of " << startIdx << " with item text "
                         << startText << ".  The expected start item text was "
                         << expectedStartIdxText << ".";
                exit(-1);
            }
        } else {
            qDebug() << "snap [bad scoobies]: getIndex() received an "
                        "invalid first level startIdx. The startIdx "
                        "was expected to have this text "
                     << expectedStartIdxText << ".";
            exit(-1);
        }
    }

    if ( !startIdx.isValid() ) {
        if ( searchItemText == "SessionStartTime" ) {
            idx = index(0,0);
        } else if ( searchItemText == "SessionStopTime" ) {
            idx = index(1,0);
        } else if ( searchItemText == "Pages" ) {
            idx = index(2,0);
        } else {
            qDebug() << "snap [bad scoobies]: getIndex() received "
                        "root as a startIdx and had bad child item text of \""
                     << searchItemText
                     << "\".";
            exit(-1);
        }
    } else {
        int rc = itemFromIndex(startIdx)->rowCount();
        bool isFound = false;
        for ( int i = 0; i < rc; ++i ) {
            QModelIndex cIdx = index(i,0,startIdx);
            QString cText = itemFromIndex(cIdx)->text();
            if ( cText == searchItemText ) {
                idx = cIdx;
                isFound = true;
                break;
            }
        }
        if ( !isFound ) {
            if ( !expectedStartIdxText.isEmpty() ) {
                qDebug()
                    << "snap [bad scoobies]: getIndex() received a start item "
                    << expectedStartIdxText
                    << ".  Unable to find a child with the item text "
                    << searchItemText << " for that parent.";
            } else {
                qDebug()
                    << "snap [bad scoobies]: getIndex() received a startIdx of "
                    << startIdx
                    << ".  Unable to find a child with the item text "
                    << searchItemText << ".";
            }
            exit(-1);
        }
    }

    return idx;
}

bool PlotBookModel::isIndex(const QModelIndex &idx, const QString &itemText) const
{
    if ( !idx.isValid() ) return false;
    return ( itemFromIndex(idx)->text() == itemText );
}
