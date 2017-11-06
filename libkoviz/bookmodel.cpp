#include "bookmodel.h"
#include <float.h>
#include "unit.h"

PlotBookModel::PlotBookModel(const QStringList& timeNames,
                             MonteModel *monteModel, QObject *parent) :
    QStandardItemModel(parent),
    _timeNames(timeNames),
    _monteModel(monteModel)
{
    _initModel();
}

PlotBookModel::PlotBookModel(const QStringList& timeNames,
                             MonteModel *monteModel,
                             int rows, int columns, QObject *parent) :
    QStandardItemModel(rows,columns,parent),
    _timeNames(timeNames),
    _monteModel(monteModel)
{
    _initModel();
}

PlotBookModel::~PlotBookModel()
{
    foreach ( QPainterPath* path, _curve2path.values() ) {
        if ( path ) {
            delete path;
        }
    }
    _curve2path.clear();
}

bool PlotBookModel::setData(const QModelIndex &idx,
                            const QVariant &value, int role)
{
    // If setting curve data, for speed, cache a painterpath out of curve model
    if ( idx.column() == 1 ) {
        QModelIndex tagIdx = sibling(idx.row(),0,idx);
        QString tag = data(tagIdx).toString();
        if ( tag == "CurveData" ) {
            TrickCurveModel* curveModel =
                    QVariantToPtr<TrickCurveModel>::convert(value);
            if ( curveModel ) {
                if ( _curve2path.contains(curveModel) ) {
                    QPainterPath* currPath = _curve2path.value(curveModel);
                    delete currPath;
                    _curve2path.remove(curveModel);
                }
                QPainterPath* path = _createPainterPath(curveModel);
                _curve2path.insert(curveModel,path);
            }
        }
    }

    return QStandardItemModel::setData(idx,value,role);
}

void PlotBookModel::setPlotMathRect(const QRectF& mathRect,
                                    const QModelIndex& plotIdx)
{
    QModelIndex plotMathRectIdx = getDataIndex(plotIdx, "PlotMathRect","Plot");

    // Flip if y-axis not directed "up" (this happens with bboxes)
    QRectF M = mathRect;
    if ( M.top() < M.bottom() ) {
        M = QRectF(M.bottomLeft(),M.topRight());
    }

    setData(plotMathRectIdx, M);
}

QStandardItem *PlotBookModel::addChild(QStandardItem *parentItem,
                                       const QString &childTitle,
                                       const QVariant &childValue)
{
    QStandardItem *columnZeroItem = new QStandardItem(childTitle);
    QStandardItem *columnOneItem = new QStandardItem(childValue.toString());

    QList<QStandardItem*> items;
    items << columnZeroItem << columnOneItem;
    parentItem->appendRow(items);

    QModelIndex columnOneIdx = indexFromItem(columnOneItem);
    setData(columnOneIdx,childValue);

    return columnZeroItem;
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
    } else {
        fprintf(stderr,
                "koviz [bad scoobs]: PlotBookModel::_plotIdx() failed.\n");
        exit(-1);
    }

    return pltIdx;
}

QModelIndex PlotBookModel::_ancestorIdx(const QModelIndex &startIdx,
                                    const QString& ancestorText,
                                    const QString &expectedStartIdxText) const
{
    QModelIndex idx;

    if ( !startIdx.isValid() ) {
        return idx;
    }

    if ( !expectedStartIdxText.isEmpty() ) {
        if ( !isIndex(startIdx, expectedStartIdxText) ) {
            QStandardItem* startItem = itemFromIndex(startIdx);
            QString startText = startItem->text();
            fprintf(stderr, "koviz [bad scoobies]: _ancestorIdx() received a "
                            "startIdx with tag=\"%s\". The expected start "
                            "item text was \"%s\".  The ancestor tag to "
                            "find was \"%s\".\n",
                        startText.toLatin1().constData(),
                        expectedStartIdxText.toLatin1().constData(),
                        ancestorText.toLatin1().constData());
            exit(-1);
        }
    }

    idx = startIdx;
    while (idx.isValid() && !isIndex(idx, ancestorText)) {
        idx = idx.parent();
    }

    return idx;
}

QModelIndexList PlotBookModel::plotIdxs(const QModelIndex &pageIdx) const
{
    QModelIndexList plotIdxs;

    if ( !pageIdx.isValid() ) {
        return plotIdxs;  // return empty list
    }

    QModelIndex plotsIdx = getIndex(pageIdx,"Plots", "Page");
    return getIndexList(plotsIdx, "Plot", "Plots");
}

QModelIndexList PlotBookModel::curveIdxs(const QModelIndex &curvesIdx) const
{
    return getIndexList(curvesIdx, "Curve", "Curves");
}

QList<QColor> PlotBookModel::createCurveColors(int nCurves)
{
    QList<QColor> colors;

    QColor burntorange(177,79,0); // hook'em!

    if ( nCurves < 10 ) {
        // Handpicked colors for smaller number of curves
        QColor blue(48,85,200);
        QColor red(200,30,30);
        QColor magenta(130,15,120);
        QColor green(60,180,45);
        QColor black(0,0,0);
        QColor gray(145,170,192);
        QColor pink(255,192,255);
        QColor medblue(49,140,250);
        QList<QColor> handPickedColors;
        handPickedColors << blue << red << magenta
                         << green << burntorange << black
                         << gray << pink << medblue;
        for (int i = 0; i < nCurves; ++i ) {
            colors << handPickedColors.at(i);
        }
    } else {
        // This is for "rainbow banding" primarily for many monte carlo curves
        int nBands = 10;
        int hBeg = 10; int hEnd = 230;
        int q = div(nCurves-1,nBands).quot;
        if ( q == 0 ) {
            q = 1.0;
        }
        for (int i = 0; i < nCurves; ++i ) {
            int h = qRound((double)(hEnd-hBeg)/(nBands-1.0))*div(i,q).quot+hBeg;
            int s = qRound(0.75*255);
            int v = qRound(0.87*255);
            colors << QColor::fromHsv(h,s,v);
        }
        colors.removeFirst();
        colors.prepend(burntorange);  // Longhorns #0!

    }

    return colors;
}

QRectF PlotBookModel::getPlotMathRect(const QModelIndex &plotIdx)
{
    QRectF M;

    QModelIndex plotMathRectIdx = getDataIndex(plotIdx, "PlotMathRect","Plot");
    plotMathRectIdx = plotMathRectIdx.sibling(plotMathRectIdx.row(),1);
    M = data(plotMathRectIdx).toRectF();

    // Flip if y-axis not directed "up" (this happens with bboxes)
    if ( M.topLeft().y() < M.bottomLeft().y() ) {
        M = QRectF(M.bottomLeft(),M.topRight());
    }

    return M;
}

void PlotBookModel::_initModel()
{
    setColumnCount(2);
    QStandardItem *rootItem = invisibleRootItem();

    addChild(rootItem, "SessionStartTime",-DBL_MAX);
    addChild(rootItem, "SessionStopTime",DBL_MAX);
    addChild(rootItem, "Pages","");
    addChild(rootItem, "Tables","");
}

//
// Convenience method for obtaining QModelIndexes using search strings (tags)
//
// expectedStartIdxText should match startIdx's text.
// It's more for debugging, but helps ensure that one is in sync with the tree.
//
// StartIdx is normally the immediate parent of the item with searchItemText.
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

    // For Table, the search may go up the tree
    if ( searchItemText == "Table" ) {
        idx = _ancestorIdx(startIdx, "Table", expectedStartIdxText);
        if ( idx.isValid() ) {
            return idx;
        }
    }

    if ( !expectedStartIdxText.isEmpty() ) {
        if ( startIdx.isValid() ) {
            if ( !isIndex(startIdx, expectedStartIdxText) ) {
                QStandardItem* startItem = itemFromIndex(startIdx);
                QString startText = startItem->text();
                fprintf(stderr,"koviz [bad scoobs]:1: getIndex() received a "
                               "startIdx with tag=\"%s\".  The expected start "
                               "item text was \"%s\".\n",
                               startText.toLatin1().constData(),
                               expectedStartIdxText.toLatin1().constData());
                exit(-1);
            }
        } else {
            fprintf(stderr,"koviz [bad scoobs]:2: getIndex() received an "
                           "invalid startIdx. The startIdx "
                           "was expected to have this text \"%s\".\n",
                           expectedStartIdxText.toLatin1().constData());
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
        } else if ( searchItemText == "Tables" ) {
            idx = index(3,0);
        } else if ( searchItemText == "DefaultPageTitles" ) {
            idx = index(4,0);
        } else if ( searchItemText == "LiveCoordTime" ) {
            idx = index(5,0);
        } else if ( searchItemText == "StartTime" ) {
            idx = index(6,0);
        } else if ( searchItemText == "StopTime" ) {
            idx = index(7,0);
        } else if ( searchItemText == "Presentation" ) {
            idx = index(8,0);
        } else if ( searchItemText == "IsShowLiveCoord" ) {
            idx = index(9,0);
        } else if ( searchItemText == "RunToShiftHash" ) {
            idx = index(10,0);
        } else if ( searchItemText == "LegendLabels" ) {
            idx = index(11,0);
        } else if ( searchItemText == "Orientation" ) {
            idx = index(12,0);
        } else if ( searchItemText == "TimeMatchTolerance" ) {
            idx = index(13,0);
        } else if ( searchItemText == "Frequency" ) {
            idx = index(14,0);
        } else {
            fprintf(stderr,"koviz [bad scoobs]:3: getIndex() received "
                           "root as a startIdx and had bad child "
                           "item text of \"%s\".\n",
                           searchItemText.toLatin1().constData());
            exit(-1);
        }
    } else {
        QStringList cStrings;
        int rc = itemFromIndex(startIdx)->rowCount();
        bool isFound = false;
        for ( int i = 0; i < rc; ++i ) {
            QModelIndex cIdx = index(i,0,startIdx);
            QString cText = itemFromIndex(cIdx)->text();
            cStrings << cText;
            if ( cText == searchItemText ) {
                idx = cIdx;
                isFound = true;
                break;
            }
        }
        if ( !isFound ) {
            fprintf(stderr,
                    "koviz [bad scoobs]:4:PlotBookModel::getIndex()\n"
                    "startIdxText=%s\n"
                    "expectedStartIdxText=%s\n"
                    "searchItemText=%s\n"
                    "rowCountStartIdx=%d\n",
                    data(startIdx).toString().toLatin1().constData(),
                    expectedStartIdxText.toLatin1().constData(),
                    searchItemText.toLatin1().constData(),
                    rowCount(startIdx));
            exit(-1);
        }
    }

    return idx;
}

QModelIndex PlotBookModel::getDataIndex(const QModelIndex &startIdx,
                                    const QString &searchItemText,
                                    const QString &expectedStartIdxText) const
{
    QModelIndex tagIdx = getIndex(startIdx,searchItemText,expectedStartIdxText);
    QModelIndex dataIdx = sibling(tagIdx.row(),1,tagIdx);
    return dataIdx;
}

QString PlotBookModel::getDataString(const QModelIndex &startIdx,
                                     const QString &searchItemText,
                                     const QString &expectedStartIdxText) const
{
    QModelIndex tagIdx = getIndex(startIdx,searchItemText,expectedStartIdxText);
    QModelIndex dataIdx = sibling(tagIdx.row(),1,tagIdx);
    return data(dataIdx).toString();
}

double PlotBookModel::getDataDouble(const QModelIndex &startIdx,
                                     const QString &searchItemText,
                                     const QString &expectedStartIdxText) const
{
    QModelIndex tagIdx = getIndex(startIdx,searchItemText,expectedStartIdxText);
    QModelIndex dataIdx = sibling(tagIdx.row(),1,tagIdx);
    bool ok;
    double d = data(dataIdx).toDouble(&ok);
    if ( !ok ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::getDataDouble()\n");
        exit(-1);
    }
    return d;
}

int PlotBookModel::getDataInt(const QModelIndex &startIdx,
                                 const QString &searchItemText,
                                 const QString &expectedStartIdxText) const
{
    QModelIndex dataIdx = getDataIndex(startIdx,searchItemText,
                                      expectedStartIdxText);
    bool ok;
    int i = data(dataIdx).toInt(&ok);
    if ( !ok ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::getDataInt()\n");
        exit(-1);
    }
    return i;
}

QHash<QString,QVariant> PlotBookModel::getDataHash(const QModelIndex &startIdx,
                                 const QString &searchItemText,
                                 const QString &expectedStartIdxText) const
{
    QModelIndex dataIdx = getDataIndex(startIdx,searchItemText,
                                       expectedStartIdxText);
    QHash<QString,QVariant> hash = data(dataIdx).toHash();
    return hash;
}

// no_line, thick_line and x_thick_line currently unsupported
QVector<qreal> PlotBookModel::getLineStylePattern(
                                             const QModelIndex &curveIdx) const
{
    QVector<qreal> pattern;

    QString s =  getDataString(curveIdx,"CurveLineStyle","Curve");
    s = s.toLower();

    if ( s == "plain" || s.isEmpty()) {
        // pattern is empty
    } else if ( s == "dash" ) {
        pattern << 5 << 3;
    } else if ( s == "fine_dash" ) {
        pattern << 1 << 2;
    } else if ( s == "med_fine_dash" ) {
        pattern << 3 << 2;
    } else if ( s == "long_dash" ) {
        pattern << 8 << 5;
    } else if ( s == "x_long_dash" ) {
        pattern << 12 << 8;
    } else if ( s == "dot_dash" ) {
        pattern << 12 << 4 << 1 << 4;
    } else if ( s == "2_dot_dash" ) {
        pattern << 12 << 3 << 1 << 2 << 1 << 3;
    } else if ( s == "3_dot_dash" ) {
        pattern << 12 << 3 << 1 << 2 << 1 << 2 << 1 << 3;
    } else if ( s == "4_dot_dash" ) {
        pattern << 16 << 3 << 1 << 2 << 1 << 2 << 1 << 2 << 1 << 3;
    } else {
        fprintf(stderr,"koviz [error]: PlotBookModel::getLineStylePattern(): "
                       " Unsupported line style \"%s\" given.",
                       s.toLatin1().constData());
    }

    return pattern;
}

// i'th curve under curves idx
TrickCurveModel *PlotBookModel::getTrickCurveModel(const QModelIndex& curvesIdx,
                                                   int i) const
{
    if ( !isIndex(curvesIdx, "Curves") ) {
        fprintf(stderr,"koviz [bad scoobs]:1: "
                       "PlotBookModel::getTrickCurveModel()\n");
        exit(-1);
    }

    QModelIndex curveIdx = index(i,0,curvesIdx);
    TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
    return curveModel;
}

TrickCurveModel *PlotBookModel::getTrickCurveModel(
                                             const QModelIndex &curveIdx) const
{
    if ( !isIndex(curveIdx, "Curve") ) {
        fprintf(stderr,"koviz [bad scoobs]:2: "
                       "PlotBookModel::getTrickCurveModel()\n");
        exit(-1);
    }

    QModelIndex curveDataIdx = getDataIndex(curveIdx,"CurveData");
    QVariant v = data(curveDataIdx);
    TrickCurveModel* curveModel =QVariantToPtr<TrickCurveModel>::convert(v);

    return curveModel;
}

QPainterPath* PlotBookModel::getCurvePainterPath(
                                              const QModelIndex &curveIdx) const
{
    QPainterPath* path;

    TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);

    if ( _curve2path.contains(curveModel) ) {
        path = _curve2path.value(curveModel);
    } else {
        fprintf(stderr,"koviz [bad scoobs]: "
                       "PlotBookModel::getCurvePainterPath()\n");
        exit(-1);
    }

    return path;
}

// TODO: cache error path if it's not changing
QPainterPath *PlotBookModel::getCurvesErrorPath(const QModelIndex &curvesIdx)
{
    QPainterPath* path;
    path = _createCurvesErrorPath(curvesIdx);
    return path;
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
        QModelIndex pageIdx = getIndex(startIdx, "Page", expectedStartIdxText);
        if ( !pageIdx.isValid() ) {
            fprintf(stderr,"koviz [bad scoobs]: getIndexList() received "
                           "startItemText=\"%s\" and searchItemText=\"%s\".  "
                           "Unable to find list for the item text.",
                           startItemText.toLatin1().constData(),
                           searchItemText.toLatin1().constData());
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

bool PlotBookModel::isChildIndex(const QModelIndex &pidx,
                                 const QString& expectedParentItemText,
                                 const QString &childItemText) const
{
    bool isChild = false;

    if (!isIndex(pidx,expectedParentItemText)) return false;

    int rc = itemFromIndex(pidx)->rowCount();
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex cIdx = index(i,0,pidx);
        QString cText = itemFromIndex(cIdx)->text();
        if ( cText == childItemText ) {
            isChild = true;
            break;
        }
    }

    return isChild;
}

double PlotBookModel::xScale(const QModelIndex& curveIdx) const
{
    double xs = 1.0;

    QString tag = data(curveIdx).toString();

    if ( tag != "Curve" ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::xScale() : "
                       "expected tag \"Curve\", instead tag=\"%s\".\n",
                       tag.toLatin1().constData());
        exit(-1);
    }

    TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
    if ( !curveModel ) {
        xs = 0.0;
        return xs;
    }

    // Unit scale
    QModelIndex curveXUnitIdx = getDataIndex(curveIdx, "CurveXUnit","Curve");
    QString bookXUnit = data(curveXUnitIdx).toString();
    if ( !bookXUnit.isEmpty() && bookXUnit != "--" ) {
        QString loggedXUnit = curveModel->x()->unit();
        xs = Unit::scale(loggedXUnit,bookXUnit);
    }

    // Book model x scale
    double k = getDataDouble(curveIdx,"CurveXScale","Curve");
    if ( k != 1.0 ) {
        xs *= k;
    }

    return xs;
}


double PlotBookModel::yScale(const QModelIndex& curveIdx) const
{
    double ys = 1.0;

    QString tag = data(curveIdx).toString();

    if ( tag != "Curve" ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::yScale() : "
                       "expected tag \"Curve\", instead tag=\"%s\".\n",
                       tag.toLatin1().constData());
        exit(-1);
    }

    TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
    if ( !curveModel ) {
        ys = 0.0;
        return ys;
    }

    // Unit scale
    QModelIndex curveYUnitIdx = getDataIndex(curveIdx, "CurveYUnit","Curve");
    QString bookYUnit = data(curveYUnitIdx).toString();
    if ( !bookYUnit.isEmpty() && bookYUnit != "--" ) {
        QString loggedYUnit = curveModel->y()->unit();
        ys = Unit::scale(loggedYUnit, bookYUnit);
    }

    // Book model y scale factor
    double k = getDataDouble(curveIdx,"CurveYScale","Curve");
    if ( k != 1.0 ) {
        ys *= k;
    }

    return ys;
}

double PlotBookModel::xBias(const QModelIndex &curveIdx) const
{
    double xb = 0.0;

    QString tag = data(curveIdx).toString();

    if ( tag != "Curve" ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::xBias() : "
                       "expected tag \"Curve\", instead tag=\"%s\".\n",
                       tag.toLatin1().constData());
        exit(-1);
    }

    TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
    if ( !curveModel ) {
        xb = 0.0;
        return xb;
    }

    // Unit bias (for temperature)
    QModelIndex curveXUnitIdx = getDataIndex(curveIdx, "CurveXUnit","Curve");
    QString bookXUnit = data(curveXUnitIdx).toString();
    if ( !bookXUnit.isEmpty() && bookXUnit != "--" ) {
        QString loggedXUnit = curveModel->x()->unit();
        xb = Unit::bias(loggedXUnit, bookXUnit);
    }

    double b = getDataDouble(curveIdx,"CurveXBias","Curve");
    if ( b != 0.0 ) {
        xb += b;
    }

    return xb;
}

double PlotBookModel::yBias(const QModelIndex &curveIdx) const
{
    double yb = 0.0;

    QString tag = data(curveIdx).toString();

    if ( tag != "Curve" ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::yBias() : "
                       "expected tag \"Curve\", instead tag=\"%s\".\n",
                       tag.toLatin1().constData());
        exit(-1);
    }

    TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
    if ( !curveModel ) {
        yb = 0.0;
        return yb;
    }

    // Unit bias (for temperature)
    QModelIndex curveYUnitIdx = getDataIndex(curveIdx, "CurveYUnit","Curve");
    QString bookYUnit = data(curveYUnitIdx).toString();
    if ( !bookYUnit.isEmpty() && bookYUnit != "--" ) {
        QString loggedYUnit = curveModel->y()->unit();
        yb = Unit::bias(loggedYUnit, bookYUnit);
    }

    double b = getDataDouble(curveIdx,"CurveYBias","Curve");
    if ( b != 0.0 ) {
        yb += b;
    }

    return yb;
}

QRectF PlotBookModel::calcCurvesBBox(const QModelIndex &curvesIdx) const
{
    QRectF bbox;

    QModelIndex plotIdx = curvesIdx.parent();
    QString presentation = getDataString(plotIdx,"PlotPresentation","Plot");
    if ( presentation == "compare" || presentation == "error+compare" ) {
        int rc = rowCount(curvesIdx);
        for (int i = 0; i < rc; ++i) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            QPainterPath* path = getCurvePainterPath(curveIdx);
            double xs = xScale(curveIdx);
            double ys = yScale(curveIdx);
            double xb = xBias(curveIdx);
            double yb = yBias(curveIdx);
            QRectF pathBox = path->boundingRect();
            double w = pathBox.width();
            double h = pathBox.height();
            QPointF topLeft(xs*pathBox.topLeft().x()+xb,
                           (ys*pathBox.topLeft().y()+yb));
            QRectF scaledPathBox(topLeft,QSizeF(xs*w,ys*h));
            bbox = bbox.united(scaledPathBox);
        }
        if ( presentation == "error+compare" ) {
            QPainterPath* errorPath = _createCurvesErrorPath(curvesIdx);
            bbox = bbox.united(errorPath->boundingRect());
            delete errorPath;
        }
    } else if ( presentation == "error" ) {
        QPainterPath* errorPath = _createCurvesErrorPath(curvesIdx);
        bbox = errorPath->boundingRect();
        delete errorPath;
    } else {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::calcCurvesBBox()\n");
        exit(-1);
    }

    // If bbox is flat (e.g. zero error plot) make box around flat curve
    if ( bbox.height() == 0.0 ) {
        double top = bbox.y()+1.0;
        double bot = bbox.y()-1.0;
        bbox.setY(top);
        bbox.setBottom(bot);
    }

    // Flip if y-axis not directed "up" (this happens with bboxes)
    if ( bbox.top() < bbox.bottom() ) {
        bbox = QRectF(bbox.bottomLeft(),bbox.topRight());
    }

    // Fit bbox within PlotXYMinMax bounds
    double plotXMinRange = getDataDouble(plotIdx,"PlotXMinRange","Plot");
    double plotXMaxRange = getDataDouble(plotIdx,"PlotXMaxRange","Plot");
    double plotYMinRange = getDataDouble(plotIdx,"PlotYMinRange","Plot");
    double plotYMaxRange = getDataDouble(plotIdx,"PlotYMaxRange","Plot");
    if ( plotXMinRange != -DBL_MAX ) {
        bbox.setLeft(plotXMinRange);
    }
    if ( plotXMaxRange != DBL_MAX ) {
        bbox.setRight(plotXMaxRange);
    }
    if ( plotYMaxRange != DBL_MAX ) {
        bbox.setTop(plotYMaxRange);
    }
    if ( plotYMinRange != -DBL_MAX ) {
        bbox.setBottom(plotYMinRange);
    }

    return bbox;
}

// Note: No scaling, the path is straight from the logged data
QPainterPath* PlotBookModel::_createPainterPath(TrickCurveModel *curveModel)
{
    QPainterPath* path = new QPainterPath;

    curveModel->map();

    TrickModelIterator it = curveModel->begin();
    const TrickModelIterator e = curveModel->end();

    double f = getDataDouble(QModelIndex(),"Frequency");
    double start = getDataDouble(QModelIndex(),"StartTime");
    double stop = getDataDouble(QModelIndex(),"StopTime");
    bool isFirst = true;
    while (it != e) {
        double t = it.t();
        if ( f > 0.0 ) {
            if ( fabs(t-round(t/f)*f) > 1.0e-9 ) { // t not divisible by f?
                ++it;
                continue;
            }
        }
        if ( t >= start && t <= stop ) {
            if ( isFirst ) {
                path->moveTo(it.x(),it.y());
                isFirst = false;
            } else {
                path->lineTo(it.x(),it.y());
            }
        }
        ++it;
    }

    curveModel->unmap();

    return path;
}

// curveIdx0/1 are child indices of "Curves" with tagname "Curve"
//
// returned path is scaled
//
// Note: error paths do not do CurveYScale (or bias)
QPainterPath* PlotBookModel::_createCurvesErrorPath(
                                            const QModelIndex &curvesIdx) const
{
    QPainterPath* path = new QPainterPath;

    if ( !isIndex(curvesIdx,"Curves") ) {
        fprintf(stderr,"koviz [bad scoobies]:1:"
                       "PlotBookModel::_createErrorPath()\n");
        exit(-1);
    }

    if ( rowCount(curvesIdx) != 2 ) {
        fprintf(stderr,"koviz [bad scoobies]:2:"
                       "PlotBookModel::_createErrorPath(): "
                       "Expected two curves for creating an error path.\n");

        exit(-1);
    }

    TrickCurveModel* c0 = getTrickCurveModel(curvesIdx,0);
    TrickCurveModel* c1 = getTrickCurveModel(curvesIdx,1);

    if ( c0 == 0 || c1 == 0 ) {
        fprintf(stderr,"koviz [bad scoobs]:3: "
                       "PlotBookModel::_createErrorPath(). "
                       "Null curveModel!\n ");
        exit(-1);
    }

    if ( c0->t()->unit() != c1->t()->unit() ) {
        fprintf(stderr,"koviz [bad scoobs]:4: "
                       "PlotBookModel::_createErrorPath().  "
                       "TODO: curveModels time units do not match.\n");
        exit(-1);
    }

    QModelIndex idx0 = index(0,0,curvesIdx);
    QModelIndex idx1 = index(1,0,curvesIdx);

    QString curveXName0 = getDataString(idx0,"CurveXName","Curve");
    QString curveXUnit0 = getDataString(idx0,"CurveXUnit","Curve");
    QString curveYName0 = getDataString(idx0,"CurveYName","Curve");

    QString curveXName1 = getDataString(idx1,"CurveXName","Curve");
    QString curveXUnit1 = getDataString(idx1,"CurveXUnit","Curve");
    QString curveYName1 = getDataString(idx1,"CurveYName","Curve");

    if (!_timeNames.contains(curveXName0)) {
        // TODO: error plot when x is not time e.g. x/y position
        //       d=sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1))
        fprintf(stderr,"koviz [todo]: Handle error plot when x is not time.\n"
                       "Aborting!\n");
        exit(-1);
    }
    if ( curveXName0 != curveXName1 || curveXUnit0 != curveXUnit1 ||
         curveYName0 != curveYName1 ) {
        fprintf(stderr,"koviz [todo]: Handle error plot when xynames or xunits "
                       "are different.\n  Aborting!\n");
        exit(-1);
    }

    QString dpUnits0 = getDataString(idx0,"CurveYUnit","Curve");
    double ys0 = 1.0;
    double ys1 = 1.0;
    double yb0 = 0.0; // bias for temperature
    double yb1 = 0.0;
    if ( !dpUnits0.isEmpty() ) {
        ys0 = Unit::scale(c0->y()->unit(),dpUnits0);
        yb0  = Unit::bias(c0->y()->unit(),dpUnits0);
        ys1 = Unit::scale(c1->y()->unit(),dpUnits0);
        yb1  = Unit::bias(c1->y()->unit(),dpUnits0);
    } else {
        ys1 = Unit::scale(c1->y()->unit(),c0->y()->unit());
        yb1  = Unit::bias(c1->y()->unit(),c0->y()->unit());
    }

    // By default the tolerance is 0.000001
    double tolerance = getDataDouble(QModelIndex(),"TimeMatchTolerance");

    // Frequency of data to show (f=0.0, the default, is all data)
    double f = getDataDouble(QModelIndex(),"Frequency");

    c0->map();
    c1->map();
    TrickModelIterator i0 = c0->begin();
    TrickModelIterator i1 = c1->begin();
    const TrickModelIterator e0 = c0->end();
    const TrickModelIterator e1 = c1->end();
    double start = getDataDouble(QModelIndex(),"StartTime");
    double stop = getDataDouble(QModelIndex(),"StopTime");
    bool isFirst = true;
    while (i0 != e0 && i1 != e1) {
        double t0 = i0.t();
        double t1 = i1.t();
        if ( qAbs(t1-t0) < tolerance ) {
            if ( f > 0.0 ) {
                if (fabs(t0-round(t0/f)*f) > 1.0e-9) { // t0 not divisible by f?
                    ++i0;
                    ++i1;
                    continue;
                }
            }
            double d = (ys0*i0.y()+yb0) - (ys1*i1.y()+yb1);
            if ( t0 >= start && t0 <= stop ) {
                if ( isFirst ) {
                    path->moveTo(t0,d);
                    isFirst = false;
                } else {
                    path->lineTo(t0,d);
                }
            }
            ++i0;
            ++i1;
        } else {
            if ( t0 < t1 ) {
                ++i0;
            } else if ( t1 < t0 ) {
                ++i1;
            } else {
                fprintf(stderr,"koviz [bad scoobs]:5: "
                               "PlotBookModel::_createErrorPath()\n");
                exit(-1);
            }
        }
    }
    c0->unmap();
    c1->unmap();

    return path;
}

// If all curves have same unit, return that, else return "--"
QString PlotBookModel::getCurvesXUnit(const QModelIndex &curvesIdx)
{

    QString xunit;

    if ( !isIndex(curvesIdx,"Curves") ) {
        fprintf(stderr,"koviz [bad scoobs] PlotBookModel::getCurvesXUnit()\n)");
        exit(-1);
    }

    int rc = rowCount(curvesIdx);

    if ( rc >= 1 ) {
        QModelIndex curve0Idx = index(0,0,curvesIdx);
        QString xunit0 = getDataString(curve0Idx,"CurveXUnit","Curve");
        if ( xunit0 == "--" || xunit0.isEmpty() ) {
            xunit0 = getTrickCurveModel(curve0Idx)->x()->unit();
        }
        for (int i = 0; i < rc; ++i) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            xunit = getDataString(curveIdx,"CurveXUnit","Curve");
            if ( xunit == "--" || xunit.isEmpty() ) {
                TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
                xunit = curveModel->x()->unit();
            }
            if ( xunit != xunit0 ) {
                xunit = "--";
                break;
            }
        }

    } else {
        xunit = "--";
    }

    return xunit;
}

// If all curves have same unit, return that, else return "--"
QString PlotBookModel::getCurvesYUnit(const QModelIndex &curvesIdx)
{

    QString yunit;

    if ( !isIndex(curvesIdx,"Curves") ) {
        fprintf(stderr,"koviz [bad scoobs] PlotBookModel::_getCurvesYUnit()\n");
        exit(-1);
    }

    int rc = rowCount(curvesIdx);

    if ( rc == 1 || rc > 2 ) {
        // Normal case
        QModelIndex curve0Idx = index(0,0,curvesIdx);
        QString yunit0 = getDataString(curve0Idx,"CurveYUnit","Curve");
        if ( yunit0 == "--" || yunit0.isEmpty() ) {
            yunit0 = getTrickCurveModel(curve0Idx)->y()->unit();
        }
        for (int i = 0; i < rc; ++i) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            yunit = getDataString(curveIdx,"CurveYUnit","Curve");
            if ( yunit == "--" || yunit.isEmpty() ) {
                TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
                yunit = curveModel->y()->unit();
            }
            if ( yunit != yunit0 ) {
                yunit = "--";
                break;
            }
        }
    } else if ( rc == 2 ) {
        // Error/Compare plot case (use units from first curve)
        QModelIndex idx0 = index(0,0,curvesIdx);
        yunit = getDataString(idx0,"CurveYUnit","Curve");
        if ( yunit == "--" || yunit.isEmpty() ) {
            yunit = getTrickCurveModel(idx0)->y()->unit();
        }
    } else {
        yunit = "--";
    }

    return yunit;
}

// If *any* of the curves in plot has a curve with x being time, return true
bool PlotBookModel::isXTime(const QModelIndex &plotIdx) const
{
    bool isXTime = false;

    bool isExistsCurves = isChildIndex(plotIdx, "Plot", "Curves");

    if ( isExistsCurves ) {
        QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
        int rc = rowCount(curvesIdx);
        for ( int i = 0; i < rc ; ++i ) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            QString xName = getDataString(curveIdx, "CurveXName","Curve");
            QString tName = getDataString(curveIdx, "CurveTimeName","Curve");
            if ( xName == tName ) {
                isXTime = true;
                break;
            }
        }
    }

    return isXTime;
}

// Example:
//
//     labels:
//        "/the/rain/in/spain/falls/on/the/plain:ball.state.pos",
//        "/the/rain/in/spokane/falls/on/the/hills:ball.state.vel",
//        "/the/rain/in/space/falls/on/houston:ball.state.acc"
//
//     returns:
//         "spain/falls/on/the/plain:pos",
//         "spokane/falls/on/the/hills:vel",
//         "space/falls/on/houston:acc"
//
QStringList PlotBookModel::abbreviateLabels(const QStringList &labels) const
{
    QStringList lbls;

    QStringList runs;
    QStringList vars;
    foreach (QString label, labels) {
        if ( !label.contains(":") ) {
            fprintf(stderr,"koviz [bad scoobs]: _abbreviateLabels\n");
            exit(-1);
        }
        runs << label.split(":").at(0);
        vars << label.split(":").at(1);
    }
    QString runsRootName = _commonRootName(runs,"/");
    QString varsRootName = _commonRootName(vars,".");
    int i = runsRootName.size();
    int j = varsRootName.size();

    foreach (QString label, labels) {

        QString run = label.split(":").at(0);
        run = run.mid(i);
        if ( i > 0 && run.startsWith("/") ) {
            run = run.mid(1); // remove prepended '/'
        }

        QString var = label.split(":").at(1);
        var = var.mid(j);
        if ( j > 0 && var.startsWith(".") ) {
            var = var.mid(1); // remove prepended '.'
        }

        QString s;
        if ( !run.isEmpty() && !var.isEmpty() ) {
            s = run + ":" + var;
        } else if ( !run.isEmpty() && var.isEmpty() ) {
            s = run;
        } else if ( run.isEmpty() && !var.isEmpty() ) {
            s = var;
        }

        lbls << s;
    }

    return lbls;
}

// Example:
//     names:
//        name[0]=/the/rain/in/spain/falls/on/the/plain
//        name[1]=/the/rain/in/spokane/falls/on/the/hills
//        name[2]=/the/rain/in/space/falls/on/houston
//        sep = /
//
//     returns "/the/rain/in"
QString PlotBookModel::_commonRootName(const QStringList &names,
                                       const QString &sep) const
{
    QString root;

    if ( names.isEmpty() ) return root;

    root = names.at(0);
    foreach ( QString name, names ) {
        root = __commonRootName(root,name,sep);
    }

    return root;
}

// Example:
//     a = /the/rain/in/spain/falls/on/the/plain
//     b = /the/rain/in/spokane/falls/on/the/hills
//     sep = /
//
//     returns "/the/rain/in"
QString PlotBookModel::__commonRootName(const QString &a, const QString &b,
                                        const QString &sep) const
{
    QString root;

    QStringList as = a.split(sep);
    QStringList bs = b.split(sep);
    QStringList names ;

    for (int i = 0; i < as.size(); ++i) {
        if ( bs.size() <= i ) {
            break;
        }
        if ( as.at(i) != bs.at(i) ) {
            break;
        }
        names << as.at(i);
    }

    root = names.join(sep) ;

    return root;
}

QStringList PlotBookModel::legendSymbols(const QModelIndex &plotIdx) const
{
    QStringList symbols;

    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
    QModelIndexList curveIdxs = this->curveIdxs(curvesIdx);

    foreach ( QModelIndex curveIdx, curveIdxs ) {
        QString symbol = getDataString(curveIdx,"CurveSymbolStyle","Curve");
        symbols << symbol;
    }

    return symbols;
}

QStringList PlotBookModel::legendColors(const QModelIndex &plotIdx) const
{
    QStringList colors;

    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
    QModelIndexList curveIdxs = this->curveIdxs(curvesIdx);

    foreach ( QModelIndex curveIdx, curveIdxs ) {
        QString color = getDataString(curveIdx,"CurveColor","Curve");
        colors << color;
    }

    return colors;
}

QStringList PlotBookModel::legendLinestyles(const QModelIndex &plotIdx) const
{
    QStringList styles;

    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
    QModelIndexList curveIdxs = this->curveIdxs(curvesIdx);

    foreach ( QModelIndex curveIdx, curveIdxs ) {
        QString style = getDataString(curveIdx,"CurveLineStyle","Curve");
        styles << style;
    }

    return styles;
}

QStringList PlotBookModel::legendLabels(const QModelIndex &plotIdx) const
{
    QStringList labels;

    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
    QModelIndexList curveIdxs = this->curveIdxs(curvesIdx);

    foreach ( QModelIndex curveIdx, curveIdxs ) {
        // Label (run:var)
        TrickCurveModel* curveModel = getTrickCurveModel(curveIdx);
        QString trk = curveModel->trkFile();
        QString runName = QFileInfo(trk).dir().absolutePath();

        QString curveName = getDataString(curveIdx,"CurveYLabel","Curve");
        if ( curveName.isEmpty() ) {
            curveName = getDataString(curveIdx,"CurveYName","Curve");
        }
        QString label = runName + ":" + curveName;
        labels << label;
    }

    labels = abbreviateLabels(labels);

    // If commandline -l1-7 option used, override legend labels
    QModelIndex legendIdx = getIndex(QModelIndex(), "LegendLabels","");
    QString l1 = getDataString(legendIdx,"Label1","LegendLabels");
    QString l2 = getDataString(legendIdx,"Label2","LegendLabels");
    QString l3 = getDataString(legendIdx,"Label3","LegendLabels");
    QString l4 = getDataString(legendIdx,"Label4","LegendLabels");
    QString l5 = getDataString(legendIdx,"Label5","LegendLabels");
    QString l6 = getDataString(legendIdx,"Label6","LegendLabels");
    QString l7 = getDataString(legendIdx,"Label7","LegendLabels");
    if (!l1.isEmpty() && labels.size() > 0) labels.replace(0,l1);
    if (!l2.isEmpty() && labels.size() > 1) labels.replace(1,l2);
    if (!l3.isEmpty() && labels.size() > 2) labels.replace(2,l3);
    if (!l4.isEmpty() && labels.size() > 3) labels.replace(3,l4);
    if (!l5.isEmpty() && labels.size() > 4) labels.replace(4,l5);
    if (!l6.isEmpty() && labels.size() > 5) labels.replace(5,l6);
    if (!l7.isEmpty() && labels.size() > 6) labels.replace(6,l7);

    return labels;
}

QList<QPen*> PlotBookModel::legendPens(const QModelIndex &plotIdx) const
{
    QList<QPen*> pens;

    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
    QModelIndexList curveIdxs = this->curveIdxs(curvesIdx);

    foreach ( QModelIndex curveIdx, curveIdxs ) {
        QString penColor = getDataString(curveIdx, "CurveColor", "Curve");
        QPen* pen = new QPen(penColor);
        QVector<qreal> pat = getLineStylePattern(curveIdx);
        pen->setDashPattern(pat);
        pens << pen;
    }

    return pens;
}

bool PlotBookModel::isPlotLegendsSame(const QModelIndex& pageIdx) const
{
    bool ok = false;

    if ( isIndex(pageIdx,"Page")) {

        QModelIndexList plotIdxs = this->plotIdxs(pageIdx);

        QStringList symbols0 = legendSymbols(plotIdxs.at(0));
        QStringList styles0  = legendLinestyles(plotIdxs.at(0));
        QStringList colors0  = legendColors(plotIdxs.at(0));
        QStringList labels0  = legendLabels(plotIdxs.at(0));

        ok = true;
        foreach ( QModelIndex plotIdx, plotIdxs ) {
            QStringList symbols = legendSymbols(plotIdx);
            QStringList styles  = legendLinestyles(plotIdx);
            QStringList colors  = legendColors(plotIdx);
            QStringList labels  = legendLabels(plotIdx);
            if ( symbols0 != symbols || styles0 != styles ||
                 colors0 != colors || labels0 != labels ) {
                ok = false;
                break;
            }
        }
    }

    return ok;
}
