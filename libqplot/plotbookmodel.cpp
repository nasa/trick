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
    QVariant v;

    PlotBookModel::IdxEnum e = indexEnum(idx);

    if ( e == PlotBookModel::CurveData ) {

        QModelIndex pidx = idx.parent();
        QModelIndex rIdx = index(6,0,pidx);
        QModelIndex xIdx = index(1,0,pidx);
        QModelIndex yIdx = index(2,0,pidx);
        int runID = data(rIdx).toInt();
        QString xName = data(xIdx).toString();
        QString yName = data(yIdx).toString();

        TrickCurveModel* curveModel = _monteModel->curve(runID,
                                                         xName,
                                                         yName);

        //
        // xunit and calc x scale if DP unit not equal to model unit
        //
        bool isXScale = false;
        double xScaleFactor = 1.0;
        QString xunit = curveModel->x().unit();
        QString xDPUnit = data(index(4,0,pidx)).toString();
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
        QString yDPUnit = data(index(5,0,pidx)).toString();
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
        v =  QStandardItemModel::data(idx,role);
    }

    return v;
}

// Returns page index for idx
// If idx is a page, it will simply return that idx
// If idx is a child of a page, it will return the parent page index
// If idx is invalid or isn't a child of a page, an invalid index is returned
QModelIndex PlotBookModel::pageIdx(const QModelIndex& idx) const
{
    QModelIndex invalidIndex;

    if ( !idx.isValid() ) return invalidIndex;
    bool isPage = isPageIdx(idx);
    if ( isPage ) return idx; // idx IS a page index
    if ( !idx.parent().isValid() && !isPage) return invalidIndex; //eg sess stop

    QModelIndex pageIndex(idx);
    while ( pageIndex.parent().isValid() ) {
        pageIndex = pageIndex.parent();
    }
    return pageIndex;
}


QModelIndexList PlotBookModel::pageIdxs() const
{
    QModelIndexList idxs;
    QStandardItem* rootItem = this->invisibleRootItem();
    for ( int i = 2; i < rootItem->rowCount(); ++i ) {  // 2 for start/stop time
        QStandardItem* item = rootItem->child(i);
        QModelIndex idx = this->indexFromItem(item);
        idxs.append(idx);
    }
    return idxs;
}

QModelIndexList PlotBookModel::plotIdxs(const QModelIndex &pageIdx) const
{
    QModelIndexList idxs;
    int rc = rowCount(pageIdx);
    // Start at 5 because pageTitle=0, startTime=1, stopTime=2, bg=3, fg=4
    for ( int i = 5 ; i < rc; ++i ) {
        idxs.append(index(i,0,pageIdx));
    }

    return idxs;
}

QModelIndex PlotBookModel::curvesIdx(const QModelIndex &plotIdx) const
{
    QModelIndex idx;
    idx = index(2,0,plotIdx);  // 2 because 0,1 are axis labels
    return idx;
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

QModelIndex PlotBookModel::curveLineColorIdx(const QModelIndex &curveIdx) const
{
    QModelIndex idx;
    idx = index(8,0,curveIdx);
    return idx;
}

QModelIndex PlotBookModel::sessionStartIdx() const
{
    QStandardItem *rootItem = invisibleRootItem();
    QStandardItem *startItem = rootItem->child(0);
    QModelIndex startIdx = indexFromItem(startItem);
    return startIdx;
}

QModelIndex PlotBookModel::sessionStopIdx() const
{
    QStandardItem *rootItem = invisibleRootItem();
    QStandardItem *stopItem = rootItem->child(1);
    QModelIndex stopIdx = indexFromItem(stopItem);
    return stopIdx;
}

QModelIndex PlotBookModel::pageBGColorIndex(const QModelIndex& pageIdx) const
{
    QModelIndex idx;
    if ( !isPageIdx(pageIdx) ) {
        return idx; // invalid
    }

    idx = index(3,0,pageIdx);
    return idx;
}

bool PlotBookModel::isPageIdx(const QModelIndex &idx) const
{
    // rows 0,1 are start/stop time
    return (idx.isValid() && !idx.parent().isValid()
            && idx.row() != 0 && idx.row() != 1 ) ;
}

bool PlotBookModel::isCurveLineColorIdx(const QModelIndex &idx) const
{
    if ( idx.row() != 7) return false;

    QModelIndex gpidx = idx.parent().parent();
    QString gpString = data(gpidx).toString();
    return (gpString == "Curves" );
}

void PlotBookModel::_initModel()
{
    QStandardItem *rootItem = invisibleRootItem();

    QStandardItem *startItem = new QStandardItem("start");
    startItem->setData(-DBL_MAX);
    rootItem->appendRow(startItem);
    QModelIndex startIdx = indexFromItem(startItem);
    setData(startIdx,-DBL_MAX);

    QStandardItem *stopItem = new QStandardItem("stop");
    stopItem->setData(DBL_MAX);
    rootItem->appendRow(stopItem);
    QModelIndex stopIdx = indexFromItem(stopItem);
    setData(stopIdx,DBL_MAX);
}

PlotBookModel::IdxEnum PlotBookModel::indexEnum(const QModelIndex &idx) const
{
    PlotBookModel::IdxEnum ret = Invalid;

    int row = idx.row();
    QModelIndex pidx   = parent(idx);
    QModelIndex gpidx  = parent(pidx);
    QModelIndex g2pidx = parent(gpidx);
    QModelIndex g3pidx = parent(g2pidx);
    QModelIndex g4pidx = parent(g3pidx);

    if ( isPageIdx(idx) ) {
        ret = Page;
    } else if ( !pidx.isValid()  && row == 0 ) {
        ret = SessionStartTime;
    } else if ( !pidx.isValid()  && row == 1 ) {
        ret = SessionStopTime;
    } else if ( ! gpidx.isValid() && row == 0 ) {
        ret = PageTitle;
    } else if ( ! gpidx.isValid() && row == 1 ) {
        ret = PageStartTime;
    } else if ( ! gpidx.isValid() && row == 2 ) {
        ret = PageStopTime;
    } else if ( ! gpidx.isValid() && row == 3 ) {
        ret = PageBGColor;
    } else if ( ! gpidx.isValid() && row == 4 ) {
        ret = PageFGColor;
    } else if ( ! gpidx.isValid() && row >= 5 ) {
        ret = Plot;
    } else if ( ! g2pidx.isValid() ) {
        // Plot elements
        switch (row) {
            case 0: ret = PlotXAxisLabel; break;
            case 1: ret = PlotYAxisLabel; break;
            case 2: ret = Curves; break;
            case 3: ret = PlotTitle; break;
            case 4: ret = PlotXMin; break;
            case 5: ret = PlotXMax; break;
            case 6: ret = PlotYMin; break;
            case 7: ret = PlotYMax; break;
            case 8: ret = PlotStartTime; break;
            case 9: ret = PlotStopTime; break;
            case 10: ret = PlotGrid; break;
            case 11: ret = PlotGridColor; break;
            case 12: ret = PlotBGColor; break;
            case 13: ret = PlotFGColor; break;
            case 14: ret = PlotFont; break;
            default: ret = Invalid;
        }
    } else if ( ! g3pidx.isValid() ) {
        ret = Curve;
    } else if ( ! g4pidx.isValid() ) {
        // Curve Elements
        switch (row) {
            case 0: ret = CurveTime; break;
            case 1: ret = CurveX; break;
            case 2: ret = CurveY; break;
            case 3: ret = CurveTimeUnit; break;
            case 4: ret = CurveXUnit; break;
            case 5: ret = CurveYUnit; break;
            case 6: ret = CurveRunID; break;
            case 7: ret = CurveData; break;
            case 8: ret = CurveLineColor; break;
            case 9: ret = CurveXScale; break;
            case 10: ret = CurveYScale; break;
            case 11: ret = CurveXBias; break;
            case 12: ret = CurveYBias; break;
            case 13: ret = CurveSymbolStyle; break;
            case 14: ret = CurveSymbolSize; break;
            default: ret = Invalid; break;
        }
    }

    return ret;
}
