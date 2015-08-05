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
        QModelIndex tIdx = getIndex(curveIdx, "CurveTime", "Curve");
        QModelIndex xIdx = getIndex(curveIdx, "CurveXName", "Curve");
        QModelIndex yIdx = getIndex(curveIdx, "CurveYName", "Curve");
        int runID = itemFromIndex(rIdx)->data().toInt();
        QString tName = itemFromIndex(tIdx)->data().toString();
        QString xName = itemFromIndex(xIdx)->data().toString();
        QString yName = itemFromIndex(yIdx)->data().toString();

        TrickCurveModel* curveModel = _monteModel->curve(runID,
                                                         tName, xName, yName);
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
                                            tName, xName, yName,
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
    return getIndexList(QModelIndex(), "Page");
}

QModelIndex PlotBookModel::_plotIdx(const QModelIndex &idx) const
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
    return getIndexList(pageIdx, "Plot");
}

QModelIndexList PlotBookModel::curveIdxs(const QModelIndex &curvesIdx) const
{
    return getIndexList(curvesIdx, "Curve", "Curves");
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

    // For Page, the search may go up the tree
    if ( searchItemText == "Page" ) {
        idx = _pageIdx(startIdx);
        return idx;
    }

    // For Plot, the search may go up the tree
    if ( searchItemText == "Plot" ) {
        idx = _plotIdx(startIdx);
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

QModelIndexList PlotBookModel::getIndexList(const QModelIndex &startIdx,
                                  const QString &searchItemText,
                                  const QString &expectedStartIdxText) const
{
    QModelIndexList idxs;

    QString startItemText;
    if ( startIdx.isValid() ) {
        startItemText = itemFromIndex(startIdx)->text();
    }

    //
    // Get parent idx (pidx) for index list
    //
    QModelIndex pidx = startIdx;
    if ( searchItemText == "Page" ) {
        pidx = getIndex(QModelIndex(), "Pages");
    } else if ( searchItemText == "Plot" ) {
        // Made so startIdx doesn't have to be the parent PageIdx
        QModelIndex pageIdx = getIndex(startIdx, "Page");
        if ( !pageIdx.isValid() ) {
            qDebug() << "snap [bad scoobies]: getIndexList() received a "
                        "startIdx of \"" << startItemText << "\""
                    << startIdx << " with search item text " << searchItemText
                    << ".  Unable to find list for the item text.";
            exit(-1);
        }
        pidx = getIndex(pageIdx, "Plots", "Page");
    }

    int rc = rowCount(pidx);
    for ( int i = 0; i < rc; ++i ) {
        idxs.append(index(i,0,pidx));
    }

    return idxs;
}

bool PlotBookModel::isIndex(const QModelIndex &idx, const QString &itemText) const
{
    if ( !idx.isValid() ) return false;
    return ( itemFromIndex(idx)->text() == itemText );
}
