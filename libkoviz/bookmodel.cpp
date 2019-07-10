#include "bookmodel.h"
#include <float.h>
#include "unit.h"

PlotBookModel::PlotBookModel(const QStringList& timeNames,
                             Runs *runs, QObject *parent) :
    QStandardItemModel(parent),
    _timeNames(timeNames),
    _runs(runs)
{
    _initModel();
}

PlotBookModel::PlotBookModel(const QStringList& timeNames,
                             Runs *runs,
                             int rows, int columns, QObject *parent) :
    QStandardItemModel(rows,columns,parent),
    _timeNames(timeNames),
    _runs(runs)
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

    foreach ( QModelIndex pageIdx, pageIdxs() ) {
        foreach ( QModelIndex plotIdx, plotIdxs(pageIdx) ) {
            QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
            foreach (QModelIndex curveIdx, curveIdxs(curvesIdx)) {
                CurveModel* c =  getCurveModel(curveIdx);
                delete c;
            }
        }
    }
}

bool PlotBookModel::setData(const QModelIndex &idx,
                            const QVariant &value, int role)
{
    // If setting curve data, for speed, cache a painterpath out of curve model
    if ( idx.column() == 1 ) {
        QModelIndex tagIdx = sibling(idx.row(),0,idx);
        QString tag = data(tagIdx).toString();
        if ( tag == "CurveData" ) {
            CurveModel* curveModel = QVariantToPtr<CurveModel>::convert(value);
            QModelIndex curveIdx = idx.parent();
            _createPainterPath(curveIdx,
                               false,0,false,0,false,0,
                               false,0,false,0,false,0,
                               "","","","",curveModel);
        } else if ( tag == "StartTime" || tag == "StopTime") {
            double start = -DBL_MAX;
            double stop = DBL_MAX;
            if ( tag == "StartTime" ) {
                if ( isChildIndex(QModelIndex(),"","StopTime") ) {
                    stop = getDataDouble(QModelIndex(),"StopTime");
                }
                bool ok = false;
                start = value.toDouble(&ok);
                if ( !ok ) {
                    fprintf(stderr, "koviz [bad scoobs]: PlotBookModel::setData"
                                    " bad value for start time\n");
                    exit(-1);
                }
            } else if ( tag == "StopTime" ) {
                if ( isChildIndex(QModelIndex(),"","StartTime") ) {
                    start = getDataDouble(QModelIndex(),"StartTime");
                }
                bool ok = false;
                stop = value.toDouble(&ok);
                if ( !ok ) {
                    fprintf(stderr, "koviz [bad scoobs]: PlotBookModel::setData"
                                    " bad value for stop time\n");
                    exit(-1);
                }
            } else {
                fprintf(stderr, "koviz [bad scoobs]: "
                                "PlotBookModel::setData()\n");
                exit(-1);
            }
            QModelIndexList pages = pageIdxs();
            foreach ( QModelIndex pageIdx, pages ) {
                foreach ( QModelIndex plotIdx, plotIdxs(pageIdx) ) {
                    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
                    foreach ( QModelIndex curveIdx, curveIdxs(curvesIdx) ) {
                        _createPainterPath(curveIdx,
                                           true,start,true,stop,
                                           false,0,false,0,false,0,false,0);
                    }
                }
            }
        } else if ( tag == "PlotXScale" || tag == "PlotYScale" ) {
            QString plotXScale = "linear";
            QString plotYScale = "linear";
            QModelIndex plotIdx = idx.parent();
            if ( isChildIndex(plotIdx,"Plot","Curves") ) {
                QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
                if ( tag == "PlotXScale" ) {
                    plotXScale = value.toString();
                    if ( isChildIndex(plotIdx,"Plot","PlotYScale") ) {
                        plotYScale = getDataString(plotIdx,"PlotYScale","Plot");
                    }
                } else if ( tag == "PlotYScale" ) {
                    plotYScale = value.toString();
                    if ( isChildIndex(plotIdx,"Plot","PlotXScale" ) ) {
                        plotXScale = getDataString(plotIdx,"PlotXScale","Plot");
                    }
                } else {
                    fprintf(stderr, "koviz [bad scoobs]: "
                                    "PlotBookModel::setData()\n");
                    exit(-1);
                }
                foreach ( QModelIndex curveIdx, curveIdxs(curvesIdx) ) {
                    _createPainterPath(curveIdx,
                                       false,0,false,0,false,0,
                                       false,0,false,0,false,0,
                                       "","",plotXScale,plotYScale);
                }
            }
        } else if ( tag == "CurveYUnit" ) {
            QModelIndex curveIdx = idx.parent();
            if ( isChildIndex(curveIdx,"Curve","CurveData" ) ) {
                QModelIndex plotIdx = idx.parent().parent().parent();
                QString plotXScale = getDataString(plotIdx,"PlotXScale","Plot");
                QString plotYScale = getDataString(plotIdx,"PlotYScale","Plot");
                if ( plotXScale == "log" || plotYScale == "log" ) {
                    QString yUnit = value.toString();
                    _createPainterPath(curveIdx,
                                       false,0,false,0,false,0,
                                       false,0,false,0,false,0,
                                       "",yUnit,plotXScale,plotYScale);
                }
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
    QModelIndexList idxs;
    if ( isChildIndex(QModelIndex(),"","Pages") ) {
        idxs = getIndexList(QModelIndex(), "Page");
    }
    return idxs;
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

    if ( !isChildIndex(pageIdx,"Page","Plots") ) {
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

    QColor burntorange(177,77,0); // hook'em!
    QColor red(200,30,30);
    QColor green(35,106,26);
    QColor magenta(130,15,120);
    QColor lightpink("#E5ADE5");
    QColor darkpink(157,118,157);
    QColor black(0,0,0);
    QColor white("#b9bbbd");
    QColor darkblue(48,85,200);
    QColor medblue(49,140,250);
    QColor darkyellow("#808000");
    QColor lightyellow("#C3C300");

    // Some curve colors depend on background color
    QColor blue(darkblue);
    QColor bw(black);
    QColor yellow(darkyellow);
    QColor pink(darkpink);
    QString bg = getDataString(QModelIndex(),"BackgroundColor");
    if ( !bg.isEmpty() ) {
        QColor bgColor(bg);
        if ( bgColor.value() < 128 ) {
            bw = white;
            blue = medblue;
            yellow = lightyellow;
            pink = lightpink;
        }
    }

    if ( nCurves == 1 ) {
        colors << blue ;
    } else if ( nCurves == 2 ) {
        colors << blue << red;
    } else if ( 2 < nCurves && nCurves < 9 ) {
        // Handpicked colors for smaller number of curves
        QList<QColor> handPickedColors;
        handPickedColors << blue << red << green
                         << burntorange << magenta
                         << yellow << pink << bw;
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

    // If c1-7 cmdline color option used, override color
    QModelIndex legendColorsIdx = getIndex(QModelIndex(),"LegendColors");
    for (int i = 0; i < 7; ++i) {
        QModelIndex legendColorIdx = index(i,1,legendColorsIdx);
        QString color = data(legendColorIdx).toString();
        if ( !color.isEmpty() && i < colors.size()) {
            colors.replace(i,QColor(color));
        }
    }

    return colors;
}

QColor PlotBookModel::errorLineColor() const
{
    QColor magenta(130,15,120);
    return magenta;
}

QColor PlotBookModel::flatLineColor() const
{
    QColor green(35,106,26);
    return green;
}

QRectF PlotBookModel::getPlotMathRect(const QModelIndex &plotIdx) const
{
    QRectF M;
    if ( !isChildIndex(plotIdx,"Plot","PlotMathRect") ) {
        return M;
    }
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
        if ( searchItemText == "Pages" ) {
            idx = index(0,0);
        } else if ( searchItemText == "Tables" ) {
            idx = index(1,0);
        } else if ( searchItemText == "DefaultPageTitles" ) {
            idx = index(2,0);
        } else if ( searchItemText == "LiveCoordTime" ) {
            idx = index(3,0);
        } else if ( searchItemText == "StartTime" ) {
            idx = index(4,0);
        } else if ( searchItemText == "StopTime" ) {
            idx = index(5,0);
        } else if ( searchItemText == "Presentation" ) {
            idx = index(6,0);
        } else if ( searchItemText == "IsShowLiveCoord" ) {
            idx = index(7,0);
        } else if ( searchItemText == "RunToShiftHash" ) {
            idx = index(8,0);
        } else if ( searchItemText == "LegendLabels" ) {
            idx = index(9,0);
        } else if ( searchItemText == "Orientation" ) {
            idx = index(10,0);
        } else if ( searchItemText == "TimeMatchTolerance" ) {
            idx = index(11,0);
        } else if ( searchItemText == "Frequency" ) {
            idx = index(12,0);
        } else if ( searchItemText == "IsLegend" ) {
            idx = index(13,0);
        } else if ( searchItemText == "LegendColors" ) {
            idx = index(14,0);
        } else if ( searchItemText == "ForegroundColor" ) {
            idx = index(15,0);
        } else if ( searchItemText == "BackgroundColor" ) {
            idx = index(16,0);
        } else if ( searchItemText == "Linestyles" ) {
            idx = index(17,0);
        } else if ( searchItemText == "Symbolstyles" ) {
            idx = index(18,0);
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

// no_line currently unsupported
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
    } else if ( s == "thick_line" || s == "x_thick_line" ) {
        // No pattern for thick lines
    } else {
        fprintf(stderr,"koviz [error]: PlotBookModel::getLineStylePattern(): "
                       " Unsupported line style \"%s\" given.",
                       s.toLatin1().constData());
    }

    return pattern;
}

QStringList PlotBookModel::lineStyles() const
{
    QStringList styles;

    styles << "plain" << "dash" << "fine_dash" << "med_fine_dash"
           << "long_dash" << "x_long_dash" << "dot_dash"
           << "2_dot_dash" << "3_dot_dash" << "4_dot_dash";

    // If commandline -ls1-7 option used, override linestyle
    QModelIndex legendIdx = getIndex(QModelIndex(), "Linestyles","");
    QString ls1 = getDataString(legendIdx,"Linestyle1","Linestyles");
    QString ls2 = getDataString(legendIdx,"Linestyle2","Linestyles");
    QString ls3 = getDataString(legendIdx,"Linestyle3","Linestyles");
    QString ls4 = getDataString(legendIdx,"Linestyle4","Linestyles");
    QString ls5 = getDataString(legendIdx,"Linestyle5","Linestyles");
    QString ls6 = getDataString(legendIdx,"Linestyle6","Linestyles");
    QString ls7 = getDataString(legendIdx,"Linestyle7","Linestyles");
    if (!ls1.isEmpty() && styles.size() > 0) styles.replace(0,ls1);
    if (!ls2.isEmpty() && styles.size() > 1) styles.replace(1,ls2);
    if (!ls3.isEmpty() && styles.size() > 2) styles.replace(2,ls3);
    if (!ls4.isEmpty() && styles.size() > 3) styles.replace(3,ls4);
    if (!ls5.isEmpty() && styles.size() > 4) styles.replace(4,ls5);
    if (!ls6.isEmpty() && styles.size() > 5) styles.replace(5,ls6);
    if (!ls7.isEmpty() && styles.size() > 6) styles.replace(6,ls7);

    return styles;
}

CurveModel *PlotBookModel::createCurve(int row,
                                       const QString &tName,
                                       const QString &xName,
                                       const QString &yName)
{
    return _runs->curveModel(row,tName,xName,yName);
}

// i'th curve under curves idx
CurveModel *PlotBookModel::getCurveModel(const QModelIndex& curvesIdx,
                                         int i) const
{
    if ( !isIndex(curvesIdx, "Curves") ) {
        fprintf(stderr,"koviz [bad scoobs]:1: "
                       "PlotBookModel::getCurveModel()\n");
        exit(-1);
    }

    QModelIndex curveIdx = index(i,0,curvesIdx);
    CurveModel* curveModel = getCurveModel(curveIdx);
    return curveModel;
}

CurveModel *PlotBookModel::getCurveModel(const QModelIndex &curveIdx) const
{
    if ( !isIndex(curveIdx, "Curve") ) {
        fprintf(stderr,"koviz [bad scoobs]:2: "
                       "PlotBookModel::getCurveModel()\n");
        exit(-1);
    }

    QModelIndex curveDataIdx = getDataIndex(curveIdx,"CurveData");
    QVariant v = data(curveDataIdx);
    CurveModel* curveModel =QVariantToPtr<CurveModel>::convert(v);

    return curveModel;
}

QPainterPath* PlotBookModel::getPainterPath(const QModelIndex &curveIdx) const
{
    QPainterPath* path;

    CurveModel* curveModel = getCurveModel(curveIdx);

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

bool PlotBookModel::isIndex(const QModelIndex &idx, const QString &tag) const
{
    bool ret = false;

    if ( !idx.isValid() && tag.isEmpty() && idx.model() != 0 ) {
        ret = true;
    } else if ( data(idx).toString() == tag ) {
        ret = true;
    }

    return ret;
}

bool PlotBookModel::isChildIndex(const QModelIndex &pidx,
                                 const QString& expectedParentItemText,
                                 const QString &childItemText) const
{
    bool isChild = false;

    if (!isIndex(pidx,expectedParentItemText)) return false;

    int rc = rowCount(pidx);
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex cIdx = index(i,0,pidx);
        QString cText = data(cIdx).toString();
        if ( cText == childItemText ) {
            isChild = true;
            break;
        }
    }

    return isChild;
}

double PlotBookModel::xScale(const QModelIndex& curveIdx,
                             CurveModel *curveModelIn) const
{
    double xs = 1.0;

    QString tag = data(curveIdx).toString();

    if ( tag != "Curve" ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::xScale() : "
                       "expected tag \"Curve\", instead tag=\"%s\".\n",
                       tag.toLatin1().constData());
        exit(-1);
    }

    CurveModel* curveModel = curveModelIn;
    if ( !curveModel ) {
        curveModel = getCurveModel(curveIdx);
    }
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

    CurveModel* curveModel = getCurveModel(curveIdx);
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

double PlotBookModel::xBias(const QModelIndex &curveIdx,
                            CurveModel *curveModelIn) const
{
    double xb = 0.0;

    QString tag = data(curveIdx).toString();

    if ( tag != "Curve" ) {
        fprintf(stderr,"koviz [bad scoobs]: PlotBookModel::xBias() : "
                       "expected tag \"Curve\", instead tag=\"%s\".\n",
                       tag.toLatin1().constData());
        exit(-1);
    }

    CurveModel* curveModel = curveModelIn;
    if ( !curveModel ) {
        curveModel = getCurveModel(curveIdx);
    }
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

    CurveModel* curveModel = getCurveModel(curveIdx);
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
    QString plotXScale = getDataString(plotIdx,"PlotXScale","Plot");
    QString plotYScale = getDataString(plotIdx,"PlotYScale","Plot");
    QString presentation = getDataString(plotIdx,"PlotPresentation","Plot");
    if ( presentation == "compare" || presentation == "error+compare" ) {
        int rc = rowCount(curvesIdx);
        for (int i = 0; i < rc; ++i) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            QPainterPath* path = getPainterPath(curveIdx);
            double xb = 0.0;
            double yb = 0.0;
            double xs = 1.0;
            double ys = 1.0;
            if ( plotXScale == "linear" ) {
                xb = xBias(curveIdx);
                xs = xScale(curveIdx);
            }
            if ( plotYScale == "linear" ) {
                yb = yBias(curveIdx);
                ys = yScale(curveIdx);
            }
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

    // Margin around box
    double mw = bbox.width()*0.02;
    double left = bbox.left()-mw;
    double right = bbox.right()+mw;
    double top;
    double bot;
    if ( bbox.height() == 0.0 ) {
        // Curve is flat
        top = bbox.y()+1.0;
        bot = bbox.y()-1.0;
    } else {
        // Add 2% margin
        double mh = bbox.height()*0.02;
        top = bbox.top()-mh;
        bot = bbox.bottom()+mh;
    }
    bbox.setTop(top);
    bbox.setBottom(bot);
    bbox.setLeft(left);
    bbox.setRight(right);

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
        if ( plotXScale == "log" ) {
            if ( plotXMinRange > 0 ) {
                bbox.setLeft(log10(plotXMinRange));
            }
        } else {
            bbox.setLeft(plotXMinRange);
        }
    }
    if ( plotXMaxRange != DBL_MAX ) {
        if ( plotXScale == "log" ) {
            if ( plotXMaxRange > 0 ) {
                bbox.setRight(log10(plotXMaxRange));
            }
        } else {
            bbox.setRight(plotXMaxRange);
        }
    }
    if ( plotYMaxRange != DBL_MAX ) {
        if ( plotYScale == "log" ) {
            if ( plotYMaxRange > 0 ) {
                bbox.setTop(log10(plotYMaxRange));
            }
        } else {
            bbox.setTop(plotYMaxRange);
        }
    }
    if ( plotYMinRange != -DBL_MAX ) {
        if ( plotYScale == "log" ) {
            if ( plotYMinRange > 0 ) {
                bbox.setBottom(log10(plotYMinRange));
            }
        } else {
            bbox.setBottom(plotYMinRange);
        }
    }

    return bbox;
}

// Note:
//   No scaling or bias is done linear plot scale since it is done
//   via the paint transform. For log scale, the path is scaled/biased.
QPainterPath* PlotBookModel::__createPainterPath(CurveModel *curveModel,
                                               double startTime,double stopTime,
                                               double xs, double xb,
                                               double ys, double yb,
                                               const QString &plotXScale,
                                               const QString &plotYScale)
{
    QPainterPath* path = new QPainterPath;

    curveModel->map();

    ModelIterator* it = curveModel->begin();

    bool isXLogScale = ( plotXScale == "log" ) ? true : false;
    bool isYLogScale = ( plotYScale == "log" ) ? true : false;

    double f = getDataDouble(QModelIndex(),"Frequency");
    bool isFirst = true;
    while ( !it->isDone() ) {
        double t = it->t();
        if ( f > 0.0 ) {
            if ( fabs(t-round(t/f)*f) > 1.0e-9 ) { // t not divisible by f?
                it->next();
                continue;
            }
        }
        if ( t < startTime || t > stopTime ) {
            it->next();
            continue;
        }

        double x = it->x();
        double y = it->y();

        if ( isXLogScale ) {
            x = x*xs + xb;
            if ( x > 0 ) {
                x = log10(x);
            } else if ( x < 0 ) {
                x = log10(-x);
            } else if ( x == 0 ) {
                it->next();
                continue; // skip log(0) since -inf
            }
        }

        if ( isYLogScale ) {
            y = y*ys + yb;
            if ( y > 0 ) {
                y = log10(y);
            } else if ( y < 0 ) {
                y = log10(-y);
            } else if ( y == 0 ) {
                it->next();
                continue; // skip log(0) since -inf
            }
        }

        if ( isFirst ) {
            path->moveTo(x,y);
            isFirst = false;
        } else {
            path->lineTo(x,y);
        }

        it->next();
    }
    delete it;
    curveModel->unmap();

    return path;
}

void PlotBookModel::_createPainterPath(const QModelIndex &curveIdx,
                                      bool isUseStartTimeIn, double startTimeIn,
                                      bool isUseStopTimeIn, double stopTimeIn,
                                      bool isUseXScaleIn, double xScaleIn,
                                      bool isUseYScaleIn, double yScaleIn,
                                      bool isUseXBiasIn, double xBiasIn,
                                      bool isUseYBiasIn, double yBiasIn,
                                      const QString &xUnitIn,
                                      const QString &yUnitIn,
                                      const QString &plotXScaleIn,
                                      const QString &plotYScaleIn,
                                      CurveModel *curveModelIn)
{
    QModelIndex plotIdx = curveIdx.parent().parent();

    // Get curve model
    CurveModel* curveModel = curveModelIn;
    if ( !curveModel ) {
        curveModel = getCurveModel(curveIdx);
    }
    if ( !curveModel ) {
        fprintf(stderr, "koviz [scoobs]:1: Book::_createPainterPath()\n");
        exit(-1);
    }

    // Get plot scale
    QString plotXScale = plotXScaleIn;
    QString plotYScale = plotYScaleIn;
    if ( plotXScale.isEmpty() ) {
        if ( isChildIndex(plotIdx,"Plot","PlotXScale") ) {
            plotXScale = getDataString(plotIdx,"PlotXScale","Plot");
        } else {
            fprintf(stderr, "koviz [scoobs]:2: Book::_createPainterPath()\n");
            exit(-1);
        }
    }
    if ( plotYScale.isEmpty() ) {
        if ( isChildIndex(plotIdx,"Plot","PlotYScale") ) {
            plotYScale = getDataString(plotIdx,"PlotYScale","Plot");
        } else {
            fprintf(stderr, "koviz [scoobs]:3: Book::_createPainterPath()\n");
            exit(-1);
        }
    }

    // Get time shift (and scale)
    double tb = 0.0;
    double ts = 1.0;
    if ( isXTime(plotIdx) ) {
        tb = xBias(curveIdx,curveModel);
        ts = xScale(curveIdx,curveModel);
    }

    // X Curve Scale/bias
    double xs = 1.0;
    double xb = 0.0;
    QString bookXUnit = xUnitIn;
    if ( bookXUnit.isEmpty() ) {
        QModelIndex curveXUnitIdx = getDataIndex(curveIdx,"CurveXUnit","Curve");
        bookXUnit = data(curveXUnitIdx).toString();
    }
    if ( !bookXUnit.isEmpty() && bookXUnit != "--" ) {
        QString loggedXUnit = curveModel->x()->unit();
        xs = Unit::scale(loggedXUnit, bookXUnit);
        xb = Unit::bias(loggedXUnit, bookXUnit);
    }
    double j = xScaleIn;
    if ( !isUseXScaleIn ) {
        j = getDataDouble(curveIdx,"CurveXScale","Curve");
    }
    if ( j != 1.0 ) {
        xs *= j;
    }
    double a = xBiasIn;
    if ( !isUseXBiasIn ) {
        a = getDataDouble(curveIdx,"CurveXBias","Curve");
    }
    if ( a != 0.0 ) {
        xb += a;
    }

    // Y Curve Scale/Bias
    double ys = 1.0;
    double yb = 0.0;
    QString bookYUnit = yUnitIn;
    if ( bookYUnit.isEmpty() ) {
        QModelIndex curveYUnitIdx = getDataIndex(curveIdx,"CurveYUnit","Curve");
        bookYUnit = data(curveYUnitIdx).toString();
    }
    if ( !bookYUnit.isEmpty() && bookYUnit != "--" ) {
        QString loggedYUnit = curveModel->y()->unit();
        ys = Unit::scale(loggedYUnit, bookYUnit);
        yb = Unit::bias(loggedYUnit, bookYUnit);
    }
    double k = yScaleIn;
    if ( !isUseYScaleIn ) {
        k = getDataDouble(curveIdx,"CurveYScale","Curve");
    }
    if ( k != 1.0 ) {
        ys *= k;
    }
    double b = yBiasIn;
    if ( !isUseYBiasIn ) {
        b = getDataDouble(curveIdx,"CurveYBias","Curve");
    }
    if ( b != 0.0 ) {
        yb += b;
    }

    // Get start/stop time
    double start = startTimeIn;
    double stop = stopTimeIn;
    if ( !isUseStartTimeIn ) {
        if ( isChildIndex(QModelIndex(),"","StartTime") ) {
            start = getDataDouble(QModelIndex(),"StartTime");
        } else {
            fprintf(stderr, "koviz [scoobs]:4: Book::_createPainterPath()\n");
            exit(-1);
        }
    }
    if ( !isUseStopTimeIn ) {
        if ( isChildIndex(QModelIndex(),"","StopTime") ) {
            stop = getDataDouble(QModelIndex(),"StopTime");
        } else {
            fprintf(stderr, "koviz [scoobs]:5: Book::_createPainterPath()\n");
            exit(-1);
        }
    }

    // Create path and cache it
    if ( _curve2path.contains(curveModel) ) {
        QPainterPath* currPath = _curve2path.value(curveModel);
        delete currPath;
        _curve2path.remove(curveModel);
    }
    QPainterPath* path = __createPainterPath(curveModel,
                                            (start-tb)/ts,(stop-tb)/ts,
                                             xs, xb, ys, yb,
                                            plotXScale, plotYScale);
    _curve2path.insert(curveModel,path);
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

    CurveModel* c0 = getCurveModel(curvesIdx,0);
    CurveModel* c1 = getCurveModel(curvesIdx,1);

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
    QString curveYUnit0 = getDataString(idx0,"CurveYUnit","Curve");
    if ( curveXUnit0.isEmpty() || curveXUnit0 == "--" ) {
        curveXUnit0 = c0->x()->unit();
    }
    if ( curveYUnit0.isEmpty() || curveYUnit0 == "--" ) {
        curveYUnit0 = c0->y()->unit();
    }

    QString curveXName1 = getDataString(idx1,"CurveXName","Curve");
    QString curveXUnit1 = getDataString(idx1,"CurveXUnit","Curve");
    QString curveYUnit1 = getDataString(idx1,"CurveYUnit","Curve");
    if ( curveXUnit1.isEmpty() || curveXUnit1 == "--" ) {
        curveXUnit1 = c1->x()->unit();
    }
    if ( curveYUnit1.isEmpty() || curveYUnit1 == "--" ) {
        curveYUnit1 = c1->y()->unit();
    }

    bool canConvert = Unit::canConvert(curveYUnit0,curveYUnit1);

    if (!_timeNames.contains(curveXName0)) {
        // TODO: error plot when x is not time e.g. x/y position
        //       d=sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1))
        fprintf(stderr,"koviz [todo]: Handle error plot when x is not time.\n"
                       "Aborting!\n");
        exit(-1);
    }
    if ( curveXName0 != curveXName1 || curveXUnit0 != curveXUnit1 ) {
        fprintf(stderr,"koviz [todo]: Handle error plot when xynames or xunits "
                       "are different.\n  Aborting!\n");
        exit(-1);
    }
    if ( !canConvert ) {
        fprintf(stderr,"koviz [error]: Attempting to error plot two variables "
                       "with incompatible units.\n");
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

    // Plot X/Y Scale (log/linear)
    QModelIndex plotIdx = curvesIdx.parent();
    QString plotXScale = getDataString(plotIdx,"PlotXScale","Plot");
    QString plotYScale = getDataString(plotIdx,"PlotYScale","Plot");
    bool isXLogScale = ( plotXScale == "log" ) ? true : false;
    bool isYLogScale = ( plotYScale == "log" ) ? true : false;

    c0->map();
    c1->map();
    ModelIterator* i0 = c0->begin();
    ModelIterator* i1 = c1->begin();
    double start = getDataDouble(QModelIndex(),"StartTime");
    double stop = getDataDouble(QModelIndex(),"StopTime");
    bool isFirst = true;
    while ( !i0->isDone() && !i1->isDone() ) {
        double t0 = i0->t();
        double t1 = i1->t();
        if ( qAbs(t1-t0) < tolerance ) {
            if ( f > 0.0 ) {
                if (fabs(t0-round(t0/f)*f) > 1.0e-9) { // t0 not divisible by f?
                    i0->next();
                    i1->next();
                    continue;
                }
            }
            double d = (ys0*i0->y()+yb0) - (ys1*i1->y()+yb1);
            if ( isYLogScale ) {
                if ( d > 0 ) {
                    d = log10(d);
                } else if ( d < 0 ) {
                    d = log10(-d);
                } else if ( d == 0 ) {
                    i0->next();
                    i1->next();
                    continue; // skip log(0) since -inf
                }
            }
            if ( t0 >= start && t0 <= stop ) {
                if ( isXLogScale && t0 == 0.0 ) {
                    i0->next();
                    i1->next();
                    continue;
                }
                if ( isXLogScale ) {
                    t0 = log10(t0);
                }
                if ( isFirst ) {
                    path->moveTo(t0,d);
                    isFirst = false;
                } else {
                    path->lineTo(t0,d);
                }
            }
            i0->next();
            i1->next();
        } else {
            if ( t0 < t1 ) {
                i0->next();
            } else if ( t1 < t0 ) {
                i1->next();
            } else {
                fprintf(stderr,"koviz [bad scoobs]:5: "
                               "PlotBookModel::_createErrorPath()\n");
                exit(-1);
            }
        }
    }
    delete i0;
    delete i1;
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
            xunit0 = getCurveModel(curve0Idx)->x()->unit();
        }
        for (int i = 0; i < rc; ++i) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            xunit = getDataString(curveIdx,"CurveXUnit","Curve");
            if ( xunit == "--" || xunit.isEmpty() ) {
                CurveModel* curveModel = getCurveModel(curveIdx);
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

    QString yunit("--");

    if ( !isIndex(curvesIdx,"Curves") ) {
        fprintf(stderr,"koviz [bad scoobs] PlotBookModel::_getCurvesYUnit()\n");
        exit(-1);
    }

    int rc = rowCount(curvesIdx);

    if ( rc > 0 ) {
        QModelIndex curve0Idx = index(0,0,curvesIdx);
        QString yunit0 = getDataString(curve0Idx,"CurveYUnit","Curve");
        if ( yunit0 == "--" || yunit0.isEmpty() ) {
            yunit0 = getCurveModel(curve0Idx)->y()->unit();
        }
        for (int i = 0; i < rc; ++i) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            yunit = getDataString(curveIdx,"CurveYUnit","Curve");
            if ( yunit == "--" || yunit.isEmpty() ) {
                CurveModel* curveModel = getCurveModel(curveIdx);
                yunit = curveModel->y()->unit();
            }
            if ( yunit != yunit0 ) {
                yunit = "--";
                break;
            }
        }
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
//        "/the/rain/in/spain/falls/on/the/plain/good/grief:ball.state.pos",
//        "/the/rain/in/spokane/falls/on/the/hills/good/grief:ball.state.vel",
//        "/the/rain/in/space/falls/on/houston/good/grief:ball.state.acc"
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
    QString runsPrefix = Runs::commonPrefix(runs,"/");
    QString runsSuffix = Runs::commonSuffix(runs,"/");
    QString varsPrefix = Runs::commonPrefix(vars,".");
    int i = runsPrefix.size();
    int j = varsPrefix.size();

    foreach (QString label, labels) {

        QString run = label.split(":").at(0);
        run = run.mid(i);
        if ( i > 0 && run.startsWith("/") ) {
            run = run.mid(1); // remove prepended '/'
        }
        run = run.remove(runsSuffix);
        if ( run.endsWith("/") ) {
            run.chop(1);
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
        CurveModel* curveModel = getCurveModel(curveIdx);
        QString fname = curveModel->fileName();
        QString runName = QFileInfo(fname).dir().absolutePath();

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

QList<QPen*> PlotBookModel::legendPens(const QModelIndex &plotIdx,
                                    enum QPaintEngine::Type painterType) const
{
    QList<QPen*> pens;

    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
    QModelIndexList curveIdxs = this->curveIdxs(curvesIdx);

    foreach ( QModelIndex curveIdx, curveIdxs ) {
        QString penColor = getDataString(curveIdx, "CurveColor", "Curve");
        QPen* pen = new QPen(penColor);
        QVector<qreal> pat = getLineStylePattern(curveIdx);
        pen->setDashPattern(pat);
        QString style = getDataString(curveIdx,"CurveLineStyle","Curve");
        style = style.toLower();
        if ( style == "thick_line" ) {
            if ( painterType == QPaintEngine::Pdf) {
                pen->setWidthF(32.0);
            } else {
                pen->setWidthF(3.0);
            }
        } else if ( style == "x_thick_line" ) {
            if ( painterType == QPaintEngine::Pdf) {
                pen->setWidthF(48.0);
            } else {
                pen->setWidthF(5.0);
            }
        } else {
            if ( painterType == QPaintEngine::Pdf ) {
                pen->setWidthF(16.0);
            }
        }
        pens << pen;
    }

    return pens;
}

bool PlotBookModel::isPageLegend(const QModelIndex &pageIdx) const
{
    bool isLegend = false;

    // Check to make sure CurvesIdx exists before calculating
    bool isCurves = true;
    QModelIndexList plotIdxs = this->plotIdxs(pageIdx);
    foreach ( QModelIndex plotIdx, plotIdxs ) {
        if ( !isChildIndex(plotIdx,"Plot","Curves") ) {
            isCurves = false;
            break;
        }
    }

    if ( isCurves && isPlotLegendsSame(pageIdx) ) {
        int maxCurves = 0;
        QModelIndexList plotIdxs = this->plotIdxs(pageIdx);
        foreach ( QModelIndex plotIdx, plotIdxs ) {
            QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
            int nCurves = curveIdxs(curvesIdx).size();
            if ( nCurves > maxCurves ) {
                maxCurves = nCurves;
            }
        }

        if ( maxCurves > 1 && maxCurves < 8 ) {
            if ( isChildIndex(QModelIndex(),"","IsLegend") ) {
                QModelIndex isLegendIdx =getDataIndex(QModelIndex(),"IsLegend");
                isLegend = data(isLegendIdx).toBool() ;
            }
        }
    }

    return isLegend;
}

bool PlotBookModel::isPlotLegendsSame(const QModelIndex& pageIdx) const
{
    bool ok = false;

    if ( isIndex(pageIdx,"Page")) {

        QModelIndexList plotIdxs = this->plotIdxs(pageIdx);

        if ( !plotIdxs.isEmpty() ) {
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
    }

    return ok;
}

// If PageBackgroundColor dne under pageIdx, returns white
QColor PlotBookModel::pageBackgroundColor(const QModelIndex &pageIdx) const
{
    if ( !isIndex(pageIdx,"Page") ) {
        fprintf(stderr, "koviz [bad scoobs]: "
                "PlotBookModel::pageBackgroundColor() pageIdx is bad.\n");
        exit(-1);
    }

    QString bgName("white");
    if ( isChildIndex(pageIdx,"Page","PageBackgroundColor") ) {
        QString bg = getDataString(pageIdx, "PageBackgroundColor", "Page");
        if ( !bg.isEmpty() ) {
            bgName = bg;
        }
    }

    return QColor(bgName);
}

// If PageForegroundColor dne under pageIdx, returns black
QColor PlotBookModel::pageForegroundColor(const QModelIndex &pageIdx) const
{
    if ( !isIndex(pageIdx,"Page") ) {
        fprintf(stderr, "koviz [bad scoobs]: "
                "PlotBookModel::pageForegroundColor() pageIdx is bad.\n");
        exit(-1);
    }

    QString fgName("black");
    if ( isChildIndex(pageIdx,"Page","PageForegroundColor") ) {
        QString fg = getDataString(pageIdx, "PageForegroundColor", "Page");
        if ( !fg.isEmpty() ) {
            fgName = fg;
        }
    }

    return QColor(fgName);
}

QList<double> PlotBookModel::majorXTics(const QModelIndex& plotIdx) const
{
    QList<double> X;
    QRectF r = getPlotMathRect(plotIdx);
    double a = r.left();
    double b = r.right();
    X = _calcTicSet(a,b,1.0,10.0);
    return X;
}

QList<double> PlotBookModel::minorXTics(const QModelIndex &plotIdx) const
{
    QList<double> X;
    QRectF r = getPlotMathRect(plotIdx);
    double a = r.left();
    double b = r.right();
    QString plotScale = getDataString(plotIdx,"PlotXScale","Plot");
    X = _calcMinorTicSet(a,b,plotScale);
    return X;
}

QList<double> PlotBookModel::majorYTics(const QModelIndex &plotIdx) const
{
    QList<double> Y;
    QRectF r = getPlotMathRect(plotIdx);
    double a = r.bottom();
    double b = r.top();
    Y = _calcTicSet(a,b,1.0,10.0);
    return Y;
}

QList<double> PlotBookModel::minorYTics(const QModelIndex &plotIdx) const
{
    QList<double> Y;
    QRectF r = getPlotMathRect(plotIdx);
    double a = r.bottom();
    double b = r.top();
    QString plotScale = getDataString(plotIdx,"PlotYScale","Plot");
    Y = _calcMinorTicSet(a,b,plotScale);
    return Y;
}

//
// Return set X which is smallest tic set between [a,b] for n,u.
//
// Given [a,b],n,u:
// Return X = {x: x=i*n^k*u in [a,b]
//                i is some ordered list of ints
//                and k minimizes size of X}
//
//
// TODO: test n==2, n==e.  At this point, n==10 is the only tested value
// TODO: Need to bail if n not 2,10 or e
// TODO: Haven't tested u > 1 or u < 0
QList<double> PlotBookModel::_calcTicSet(double aIn, double bIn,
                                       double u, double n) const
{
    QList<double> X;

    // Make sure inputs are finite
    if ( !std::isfinite(aIn) || !std::isfinite(bIn) ||
         !std::isfinite(n)   || !std::isfinite(u) ) {
        return X;
    }

    // If [a,b] is a singleton, return {aIn}
    if ( aIn == bIn ) {
        X << aIn;
        return X;
    }

    // Ensure a <= b
    double a = aIn;
    double b = bIn;
    if ( bIn < aIn ) {
        a = bIn;
        b = aIn;
    }

    // k in i*n^k*u is power of n for making set X
    double k;
    if ( n == 10.0 ) {
        k = qMax(  floor(log10(qAbs(a))), floor(log10(qAbs(b))) );
    } else if ( n == 2.0 ) {
        k = qMax(  floor(log2(qAbs(a))), floor(log2(qAbs(b))) );
    } else if ( qAbs(n-exp(1)) < 1.0e-9 ) {
        k = qMax(  floor(log(qAbs(a))), floor(log(qAbs(b))) );
    } else {
        // bad scoobs
        return X;
    }

    double d = pow(n,k);

    //
    // Given d = n^k, find x0 such that x0*d <= a
    //
    double x0 = 0.0;
    double x = x0;
    if ( _isEqual(a,0) ) {
        // if (a == 0) then x0 = 0, because 0*d == 0 <= a == 0
    } else if ( x < a ) {
        while ( 1 ) {
            if ( _isEqual(x,a) ) {
                break;
            } else if ( x < a ) {
                // keep going
                x0 = x;
                x += d;
                if ( std::fpclassify(x) == FP_INFINITE ) {
                    break;
                }
            } else if ( x > a ) {
                break;
            } else {
                X.clear(); return X; // bad scoobs
            }
        }
    } else if ( x > a ) {
        while ( 1 ) {
            if ( _isEqual(x,a) ) {
                break;
            } else if ( x < a ) {
                break;
            } else if ( x > a ) {
                // keep going
                x -= d;
                if ( std::fpclassify(x) == FP_INFINITE ) {
                    break;
                }
                x0 = x;
            } else {
                X.clear(); return X; // bad scoobs
            }
        }
    } else {
        X.clear(); return X; // bad scoobs
    }

    // From last block, x0 <= a
    //
    // Find set X = {x: x in [a,b] and x = q*n^k for q in Z (ints)}
    // |X| > 0, and smallest of all such sets
    //
    // This algorithm (possibly returning from the *rabbithole):
    //
    //       bunny starts at x0 (x0 stays <= a)
    //       Hops the distance d (or d*u) until it either
    //
    //       Lands in [a,b]
    //       OR
    //       Hops OVER [a,b]
    //
    //       If hop inside  [a,b]:
    //              Add hop spot to a list
    //              Keep hopping and appending hop spots to the list
    //              until I hop past b or hit infinity
    //
    //       If hops is OVER [a,b]
    //              Hop back to x0!
    //              Refine hop distance scale from n^k to (n^(k-1))
    //              Try again (go back to x0 (rabbithole))
    //
    while ( X.size() < 2 ) {  // |X| >= 2 (unless a problem occurs)

        X.clear();
        double x = x0;  // Start search x <= a (keeps X ordered)
        d = pow(n,k);

        while ( 1 ) {
            if ( _isEqual(x,a) ) {
                X << x;
                x += d*u;
                if ( std::fpclassify(x) == FP_INFINITE ) break; else continue;
            } else if ( x < a ) { // x not in [a,b]... continue search
                x0 = x;
                double xu = x+d*u;
                if ( _isEqual(xu,a) ) {
                    x += d*u;
                } else if ( xu > a && xu < b ) {
                    double xd = x+d;
                    if ( _isEqual(xd,a) ) {
                        x += d;
                    } else if ( xd > a && xd < b ) {
                        if ( xd < xu ) {
                            x += d;
                        } else {
                            x += d*u;
                        }
                    } else {
                        x += d;
                    }
                } else {
                    x += d;
                }
                if ( std::fpclassify(x) == FP_INFINITE ) break; else continue;
            } else if ( _isEqual(x,b) ) { // x in [a,b]... and no more
                X << x;
                break;
            } else if ( x < b ) { // x in [a,b]... try to find another
                X << x;
                x += d*u;
                if ( std::fpclassify(x) == FP_INFINITE ) break; else continue;
                continue;
            } else if ( x > b ) { // x > [a,b]
                break;
            } else {
                // this should not happen
                X.clear(); return X; // bad scoobs
            }
        }

        k -= 1.0;   // smaller d = n^(k), n^(k-1), n^(k-2)...
    }

    // Clean up the set by removing any points not strictly within [a,b]
    for ( int i = X.size()-1; i >= 0; --i ) {
        double x = X.at(i);
        if ( x < a || x > b ) {
            X.removeAt(i);
        }
    }

    // Shrink tic set until |X| <=7
    // Tics are removed in favor of keeping tics "even" in order
    // to keep them the same while panning
    while ( X.size() > 7 ) {
        double x = X.at(0);
        QString s;
        s = s.sprintf("%g",x);
        if ( s.contains('e') ) {
            int n = s.indexOf('e');
            s = s.left(n);
        }
        bool isEven = false;
        if ( s.endsWith('0') || s.endsWith('2') || s.endsWith('4') ||
             s.endsWith('6') || s.endsWith('8') ) {
            isEven = true;
        }

        for ( int i = X.size()-1; i >= 0; --i ) {
            if ( isEven ) {
                if ( i%2 != 0 ) {
                    X.removeAt(i);
                }
            } else {
                if ( i%2 == 0 ) {
                    X.removeAt(i);
                }
            }
        }
    }

    if ( X.isEmpty() ) {
        X << a;
    }

    return X;
}

// I read "if two floating-point numbers in the same format are ordered
// , then they are ordered the same way when their bits are reinterpreted
// as Sign-Magnitude integers."
// This means, I think,
// for double x, y;
//
// if x > 0, y > 0 and (x < y)
// then *(ulong)&x < *(ulong)&y
//
// if we let i=*(ulong)&x and j=*(ulong)&y
// let d = qAbs(j-i)
// d is, I think,the number of "neighbors" in (i,j] for i
// A neighbor doesn't include itself, so if i==j, the number is 0
//
// Equality is then defined as:
//      if ( numNeighborsIn (a,b] < maxD ) return true; else return false;
bool PlotBookModel::_isEqual(double a, double b, ulong maxD,
                           bool isNeighborMethod) const
{
    bool ok = false;

    if ( a == b ) return true;

    if ( isNeighborMethod && sizeof(double) == sizeof(ulong)) {
        ulong n = maxD;
        if ( a == 0 ) {
            if ( b > 0 ) {
                n = (*(ulong*)&b);
            } else if ( b < 0 ) {
                double bb = qAbs(b);
                n = *(ulong*)&bb;
            }
        } else if ( a > 0 ) {
            if ( b == 0 ) {
                n = (*(ulong*)&a);
            } else if ( b > 0 ) {
                if ( b > a ) {
                    n = (*(ulong*)&b - *(ulong*)&a);
                } else {
                    n = (*(ulong*)&a - *(ulong*)&b);
                }
            } else if ( b < 0 ) {
                double bb = qAbs(b);
                n = (*(ulong*)&a + *(ulong*)&bb);
            }
        } else if ( a < 0 ) {
            if ( b == 0 ) {
                n = (*(ulong*)&a);
            } else if ( b > 0 ) {
                double aa = qAbs(a);
                n = (*(ulong*)&aa + *(ulong*)&b);
            } else if ( b < 0 ) {
                double aa = qAbs(a);
                double bb = qAbs(b);
                if ( bb > aa ) {
                    n = (*(ulong*)&bb - *(ulong*)&aa);
                } else {
                    n = (*(ulong*)&aa - *(ulong*)&bb);
                }
            }
        } else {
            // bad scoobies
            fprintf(stderr, "koviz [bad scoobs] in _isEqual(a,b)\n");
            return false;
        }

        if ( n < maxD ) {
            ok = true;
        }
    } else {

        // Assuming a != b from above

        double k;
        if ( a != 0.0 && b != 0.0 ) {
            k = qMax(log2(qAbs(a)),log2(qAbs(b)));
        } else if ( a == 0 ) {
            k = log2(qAbs(b));
        } else if ( b == 0 ) {
            k = log2(qAbs(a));
        } else {
            // bad scoobs
            fprintf(stderr, "koviz [bad scoobs] in _isEqual(a,b)\n");
            return false;
        }
        k = floor(k);

        double e;
        if ( k >= -1074 + 32 ) {
            e = pow(2.0,k-2*32);
        } else {
            e = pow(2.0,-1074);
        }

        ok = (qAbs(a-b) < e) ? true : false  ;
    }

    return ok;
}

QList<double> PlotBookModel::_calcMinorTicSet(double a, double b,
                                              const QString &plotScale) const
{
    QList<double> Minors;

    QList<double> Majors = _calcTicSet(a,b,1.0,10.0);

    if ( Majors.size() >= 2 ) {
        if ( plotScale == "linear" ) {
            double d = (Majors.at(1)-Majors.at(0))/4.0;
            double x = Majors.at(0);
            if ( x-d-d-d >= a ) Minors << x-d-d-d;
            if ( x-d-d >= a )   Minors << x-d-d;
            if ( x-d >= a )     Minors << x-d;
            foreach (double x, Majors) {
                if ( x+d <= b )     Minors << x+d;
                if ( x+d+d <= b )   Minors << x+d+d;
                if ( x+d+d+d <= b ) Minors << x+d+d+d;
            }
        } else if ( plotScale == "log" ) {
            double d = Majors.at(1)-Majors.at(0);
            QList<double> majors;
            majors << Majors.at(0)-d;
            majors.append(Majors);
            for ( int i = 0; i < majors.size(); ++i ) {
                double m = majors.at(i);
                for (int j = 2; j <= 9; ++j) {
                    double tic = m+log10(j)*d;
                    if ( tic >= a && tic <= b ) {
                        Minors << tic;
                    }
                }
            }
        } else {
            fprintf(stderr, "koviz [bad scoobs]: _calcMinorTicSet()\n");
            exit(-1);
        }
    }

    return Minors;
}
