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
            bool isAppend = false;
            if ( role == PlotBookModel::AppendData ) {
                isAppend = true;
            }
            _createPainterPath(curveIdx,
                               false,0,false,0,false,0,
                               false,0,false,0,false,0,
                               "","","","",curveModel,isAppend);

            // Instead of using QStandardItemModel::setData(idx,value,role)
            // at the bottom of this method, set the data here and emit the
            // dataChanged() signal.  This is necessary since calling
            // QStandardItemModel::setData(idx,value,role);
            // doesn't emit the dataChanged() signal since, I guess, the value
            // is an unchanging pointer.  If the dataChanged() signal is
            // not emitted, the views will not refresh correctly.
            QStandardItem* item = itemFromIndex(idx);
            if ( !item ) {
                return false;
            }
            item->setData(value,role);
            emit dataChanged(idx,idx,QVector<int>()<<role);
            return true;

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
        QModelIndex pagesIdx = getIndex(QModelIndex(),"Pages","");
        idxs = getIndexList(pagesIdx, "Page", "Pages");
    }
    return idxs;
}

QModelIndexList PlotBookModel::tableIdxs() const
{
    QModelIndexList idxs;
    if ( isChildIndex(QModelIndex(),"","Tables") ) {
        QModelIndex tablesIdx = getIndex(QModelIndex(),"Tables","");
        idxs = getIndexList(tablesIdx,"Table", "Tables");
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
        int hBeg = 10; int hEnd = 230;
        for (int i = 0; i < nCurves; ++i ) {
            int h = qRound((double)i*(hEnd-hBeg)/(nCurves-1.0)) + hBeg;
            int s = qRound(0.75*255);
            int v = qRound(0.87*255);
            colors << QColor::fromHsv(h,s,v);
        }
        colors.removeFirst();
        colors.prepend(burntorange);  // Longhorns #0!
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
        } else if ( searchItemText == "LiveCoordTimeIndex" ) {
            idx = index(4,0);
        } else if ( searchItemText == "StartTime" ) {
            idx = index(5,0);
        } else if ( searchItemText == "StopTime" ) {
            idx = index(6,0);
        } else if ( searchItemText == "Presentation" ) {
            idx = index(7,0);
        } else if ( searchItemText == "IsShowLiveCoord" ) {
            idx = index(8,0);
        } else if ( searchItemText == "RunToShiftHash" ) {
            idx = index(9,0);
        } else if ( searchItemText == "LegendLabels" ) {
            idx = index(10,0);
        } else if ( searchItemText == "Orientation" ) {
            idx = index(11,0);
        } else if ( searchItemText == "TimeMatchTolerance" ) {
            idx = index(12,0);
        } else if ( searchItemText == "Frequency" ) {
            idx = index(13,0);
        } else if ( searchItemText == "IsLegend" ) {
            idx = index(14,0);
        } else if ( searchItemText == "LegendColors" ) {
            idx = index(15,0);
        } else if ( searchItemText == "ForegroundColor" ) {
            idx = index(16,0);
        } else if ( searchItemText == "BackgroundColor" ) {
            idx = index(17,0);
        } else if ( searchItemText == "Linestyles" ) {
            idx = index(18,0);
        } else if ( searchItemText == "Symbolstyles" ) {
            idx = index(19,0);
        } else if ( searchItemText == "Symbolends" ) {
            idx = index(20,0);
        } else if ( searchItemText == "Groups" ) {
            idx = index(21,0);
        } else if ( searchItemText == "StatusBarMessage" ) {
            idx = index(22,0);
        } else if ( searchItemText == "IsShowPageTitle" ) {
            idx = index(23,0);
        } else if ( searchItemText == "IsShowPlotLegend" ) {
            idx = index(24,0);
        } else if ( searchItemText == "PlotLegendPosition" ) {
            idx = index(25,0);
        } else if ( searchItemText == "ButtonSelectAndPan" ) {
            idx = index(26,0);
        } else if ( searchItemText == "ButtonZoom" ) {
            idx = index(27,0);
        } else if ( searchItemText == "ButtonReset" ) {
            idx = index(28,0);
        } else if ( searchItemText == "XAxisLabel" ) {
            idx = index(29,0);
        } else if ( searchItemText == "YAxisLabel" ) {
            idx = index(30,0);
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

bool PlotBookModel::getDataBool(const QModelIndex &startIdx,
                                const QString &searchItemText,
                                const QString &expectedStartIdxText) const
{
    QModelIndex dataIdx = getDataIndex(startIdx,searchItemText,
                                       expectedStartIdxText);
    bool ret = data(dataIdx).toBool();
    return ret;
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

QRectF PlotBookModel::getDataRectF(const QModelIndex &startIdx,
                                   const QString &searchItemText,
                                   const QString &expectedStartIdxText) const
{
    QModelIndex dataIdx = getDataIndex(startIdx,searchItemText,
                                       expectedStartIdxText);
    QRectF R = data(dataIdx).toRectF();
    return R;

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

QVariantList PlotBookModel::getDataList(const QModelIndex &startIdx,
                                      const QString &searchItemText,
                                      const QString &expectedStartIdxText) const
{
    QModelIndex dataIdx = getDataIndex(startIdx,searchItemText,
                                       expectedStartIdxText);
    QVariantList list = data(dataIdx).toList();
    return list;

}

// no_line currently unsupported
QVector<qreal> PlotBookModel::getLineStylePattern(
                                             const QString &linestyle) const
{
    QVector<qreal> pattern;

    QString s = linestyle.toLower();

    if ( s == "plain" || s == "scatter" || s.isEmpty()) {
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

// TODO: If getCurveModel is called with an idx that corresponds
// to an error curve, a new curve model is created and sent back
// to the client; however, if an idx that corresponds to a
// Curve is given, a cached curve is given back.
//
// In the errorplot case, the client must delete the curve,
// but not in the curve case since it is held in the CurveData idx
// in the book model
//
CurveModel *PlotBookModel::getCurveModel(const QModelIndex &idx) const
{
    if ( !idx.isValid() ) {
        return 0;
    }

    QString tag = data(idx.sibling(idx.row(),0)).toString();
    CurveModel* curveModel = 0;
    if ( tag == "Curve" ) {
        QModelIndex curveDataIdx = getDataIndex(idx,"CurveData");
        QVariant v = data(curveDataIdx);
        curveModel = QVariantToPtr<CurveModel>::convert(v);
    } else if ( tag == "Plot" ) {
        QString pres = getDataString(idx,"PlotPresentation","Plot");
        if ( pres == "error" ) {
            QModelIndex curvesIdx = getIndex(idx,"Curves", "Plot");
            QPainterPath* path = _createCurvesErrorPath(curvesIdx);
            curveModel = new CurveModelPainterPath(path,
                                                   "sys.exec.out.time", "s",
                                                   "errorplot","--");
        } else {
            fprintf(stderr, "koviz [bad scoobs]: PlotBookModel::getCurveModel()"
                            ". Plot index given but plot presentation not "
                            "\"error\" as expected.\n");
            exit(-1);
        }
    } else {
        fprintf(stderr, "koviz [bad scoobs]: PlotBookModel::getCurveModel bad "
                        "index with tag=%s\n", tag.toLatin1().constData());
        exit(-1);
    }

    return curveModel;
}

void PlotBookModel::appendDataToCurves(const QList<CurveModel *> curveModels)
{
    foreach (QModelIndex pageIdx, pageIdxs()) {
        foreach (QModelIndex plotIdx, plotIdxs(pageIdx)) {
            QModelIndex curvesIdx = getIndex(plotIdx, "Curves","Plot");
            foreach (QModelIndex curveIdx, curveIdxs(curvesIdx)) {
                QModelIndex dataIdx = getDataIndex(curveIdx,
                                                   "CurveData","Curve");
                CurveModel* curveModel = getCurveModel(curveIdx);
                if ( curveModels.contains(curveModel) ) {
                    QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
                    setData(dataIdx,v,PlotBookModel::AppendData);
                }
            }
            QRectF bbox = calcCurvesBBox(curvesIdx);
            setPlotMathRect(bbox,plotIdx);
        }
    }
}

void PlotBookModel::replaceCurveModelsWithCopies(
                                          const QList<CurveModel *> curveModels)
{
    foreach (QModelIndex pageIdx, pageIdxs()) {
        foreach (QModelIndex plotIdx, plotIdxs(pageIdx)) {
            QModelIndex curvesIdx = getIndex(plotIdx, "Curves","Plot");
            foreach (QModelIndex curveIdx, curveIdxs(curvesIdx)) {
                QModelIndex dataIdx = getDataIndex(curveIdx,
                                                   "CurveData","Curve");
                CurveModel* curveModel = getCurveModel(curveIdx);
                if ( curveModels.contains(curveModel) ) {
                    CurveModel* curveModelCopy = new CurveModelCopy(curveModel);
                    QVariant v = PtrToQVariant<CurveModel>::convert(
                                                                curveModelCopy);
                    setData(dataIdx,v);
                    delete curveModel;
                }
            }
            QRectF bbox = calcCurvesBBox(curvesIdx);
            setPlotMathRect(bbox,plotIdx);
        }
    }

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

double PlotBookModel::xScale(const QModelIndex& idx,
                             CurveModel *curveModelIn) const
{
    double xs = 1.0;

    QString tag = data(idx).toString();

    if ( tag != "Curve" ) {
        return xs;
    }

    CurveModel* curveModel = curveModelIn;
    if ( !curveModel ) {
        curveModel = getCurveModel(idx);
    }
    if ( !curveModel ) {
        xs = 0.0;
        return xs;
    }

    // Unit scale
    QModelIndex curveXUnitIdx = getDataIndex(idx, "CurveXUnit","Curve");
    QString bookXUnit = data(curveXUnitIdx).toString();
    if ( !bookXUnit.isEmpty() && bookXUnit != "--" ) {
        QString loggedXUnit = curveModel->x()->unit();
        xs = Unit::scale(loggedXUnit,bookXUnit);
    }

    // Book model x scale
    double k = getDataDouble(idx,"CurveXScale","Curve");
    if ( k != 1.0 ) {
        xs *= k;
    }

    return xs;
}


double PlotBookModel::yScale(const QModelIndex& idx) const
{
    double ys = 1.0;

    QString tag = data(idx).toString();

    if ( tag != "Curve" ) {
        return ys;
    }

    CurveModel* curveModel = getCurveModel(idx);
    if ( !curveModel ) {
        ys = 0.0;
        return ys;
    }

    // Unit scale
    QModelIndex curveYUnitIdx = getDataIndex(idx, "CurveYUnit","Curve");
    QString bookYUnit = data(curveYUnitIdx).toString();
    if ( !bookYUnit.isEmpty() && bookYUnit != "--" ) {
        QString loggedYUnit = curveModel->y()->unit();
        ys = Unit::scale(loggedYUnit, bookYUnit);
    }

    // Book model y scale factor
    double k = getDataDouble(idx,"CurveYScale","Curve");
    if ( k != 1.0 ) {
        ys *= k;
    }

    return ys;
}

double PlotBookModel::xBias(const QModelIndex &idx,
                            CurveModel *curveModelIn) const
{
    double xb = 0.0;

    QString tag = data(idx).toString();

    if ( tag != "Curve" ) {
        return xb;
    }

    CurveModel* curveModel = curveModelIn;
    if ( !curveModel ) {
        curveModel = getCurveModel(idx);
    }
    if ( !curveModel ) {
        xb = 0.0;
        return xb;
    }

    // Unit bias (for temperature)
    QModelIndex curveXUnitIdx = getDataIndex(idx, "CurveXUnit","Curve");
    QString bookXUnit = data(curveXUnitIdx).toString();
    if ( !bookXUnit.isEmpty() && bookXUnit != "--" ) {
        QString loggedXUnit = curveModel->x()->unit();
        xb = Unit::bias(loggedXUnit, bookXUnit);
    }

    double b = getDataDouble(idx,"CurveXBias","Curve");
    if ( b != 0.0 ) {
        xb += b;
    }

    return xb;
}

double PlotBookModel::yBias(const QModelIndex &idx) const
{
    double yb = 0.0;

    QString tag = data(idx).toString();

    if ( tag != "Curve" ) {
        return yb;
    }

    CurveModel* curveModel = getCurveModel(idx);
    if ( !curveModel ) {
        yb = 0.0;
        return yb;
    }

    // Unit bias (for temperature)
    QModelIndex curveYUnitIdx = getDataIndex(idx, "CurveYUnit","Curve");
    QString bookYUnit = data(curveYUnitIdx).toString();
    if ( !bookYUnit.isEmpty() && bookYUnit != "--" ) {
        QString loggedYUnit = curveModel->y()->unit();
        yb = Unit::bias(loggedYUnit, bookYUnit);
    }

    double b = getDataDouble(idx,"CurveYBias","Curve");
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
            if ( w == 0 && h == 0 && path->elementCount() > 0 ) {
                // If single point (could be duplicated), make 1x1 box around pt
                double x = pathBox.x()-0.5;
                double y = pathBox.y()-0.5;
                w = 1.0;
                h = 1.0;
                pathBox.setRect(x,y,w,h);
            }
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
        double w = bbox.width();
        double h = bbox.height();
        if ( w == 0 && h == 0 && errorPath->elementCount() > 0 ) {
            // If single point (could be duplicated), make 1x1 box around pt
            double x = bbox.x()-0.5;
            double y = bbox.y()-0.5;
            w = 1.0;
            h = 1.0;
            bbox.setRect(x,y,w,h);
        }
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

// Append new data in curveModel to given QPainterPath path.
//
// If the path is empty all of curveModel's points will be added to the path.
// If the path has elements, the curveModel and path size are compared.  If
// the curveModel size is greater than the path size, it is assumed that
// the extra points in the curveModel are to be appended to the path.
// This is used to optimize loading the path from a curveModel that is
// receiving new data from a sim.
//
// Note 1:
//   No scaling or bias is done for linear plot scale since it is done
//   via the paint transform. For log scale, the path is scaled/biased.
// Note 2:
//   For the live coord to work, every point in the curve model
//   must have an associated point in the path.  There are two issues.
//         1) If the model point is close to or the same as the previously
//            added point, Qt will not add the point to the path
//         2) If the model point is a NAN, Qt will not add the point to
//            the path
//   A trick I did to force Qt to draw successive identical points
//   or points that are very close to one another is to
//   add a different point (+1.0) and then change the point i.e.:
//                    // Add (x,y) twice to path in succession
//                    path->lineTo(x,y);
//                    path->lineTo(x+1.0,y+1.0);
//                    int n = path->elementCount();
//                    path->setElementPositionAt(n-1,x,y);
//   The trick for NANs is to add a point to the path that is already on the
//   path.  If the model has points [(0,7),(1,3),(2,4),(3,nan),(4,8)], the path
//   should skip from (2,4) to (4,8).  To make that happen, replace (3,nan)
//   with (2,4).  The extra (2,4) point keeps model to path one-to-one and is
//   effectively the same path (a drawback is that the duplicated point is
//   sticky).
//
//   If all points in the curve model are nans, no points will be added to
//   the path - the plot will show up as "Empty". This happens, for example,
//   when a motion capture marker is never viewable, the plot of the marker
//   position will show up as "Empty" since there are no valid points in
//   the marker trajectory.
void PlotBookModel::__appendDataToPainterPath(CurveModel *curveModel,
                                              QPainterPath* path,
                                              double startTime,double stopTime,
                                              double xs, double xb,
                                              double ys, double yb,
                                              const QString &plotXScale,
                                              const QString &plotYScale)
{
    int nels = path->elementCount();
    int nrows = curveModel->rowCount();
    if ( nels == nrows ) {
        // Nothing to append
        return;
    }

    curveModel->map();

    ModelIterator* it = curveModel->begin();

    if ( nels < nrows ) {
        it = it->at(nels);
    } else if ( nels > nrows ) {
        // If the model has less rows than the path, the model (like TV)
        // has been been reset.  In this case, simply do not append
        return;
    }

    bool isXLogScale = ( plotXScale == "log" ) ? true : false;
    bool isYLogScale = ( plotYScale == "log" ) ? true : false;

    double f = getDataDouble(QModelIndex(),"Frequency");
    int cntNANs = 0;
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

        if ( path->elementCount() == 0 ) {
            path->moveTo(x,y);
            if ( path->elementCount() == 1 ) {
                for ( int i = 0; i < cntNANs; ++i ) {
                    // Beginning of path was nans
                    // Add first good point to beginning of path cntNANs times
                    int m = path->elementCount();
                    path->lineTo(x+1.0,y+1.0); // see Note 2 at top of method
                    int n = path->elementCount();
                    if ( n > m ) {
                        path->setElementPositionAt(n-1,x,y);
                    } else {
                        fprintf(stderr, "koviz [error]: __createPainterPath:1: "
                                "could not add point (%g,%g)\n", x,y);
                    }
                }
            } else {
                // Point not added, probably a nan
                // Count number nans that are at beginning of path
                ++cntNANs;
            }
        } else {
            int m = path->elementCount();
            path->lineTo(x,y);
            int n = path->elementCount();
            if ( m == n ) {
                /* When points are very close to one another,
                 * it looks like Qt will skip adding a lineTo(x,y).
                 * This bit of code tries to force Qt to add the
                 * lineTo(x,y) no matter how close the two points
                 * are to one another.
                 * See Note 2 at top of method for more details
                 */
                path->lineTo(x+1.0,y+1.0);
                int o = path->elementCount();
                if ( o > 0 ) {
                    if ( o > m ) {
                        path->setElementPositionAt(o-1,x,y);
                    } else {
                        // If that fails, more than likely x or y is nan
                        // Draw a line from the last point to itself
                        // See Note 2 at top of method for more details
                        QPainterPath::Element el = path->elementAt(o-1);
                        int i = path->elementCount();
                        path->lineTo(el.x+1,el.y+1);
                        int j = path->elementCount();
                        if ( i < j ) {
                            path->setElementPositionAt(j-1,el.x,el.y);
                        } else {
                            fprintf(stderr,
                                    "koviz [error] : __createPainterPath:2:"
                                    " could not add point (%g,%g)\n",el.x,el.y);
                        }
                    }
                }
            }
        }

        it->next();
    }
    delete it;
    curveModel->unmap();
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
                                      CurveModel *curveModelIn,
                                      bool isAppendDataToPainterPath)
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

    // Create (or append to) path and cache it
    QPainterPath* path = 0;
    if ( _curve2path.contains(curveModel) ) {
        path = _curve2path.value(curveModel);
        if ( ! isAppendDataToPainterPath ) {
            delete path;
            path = new QPainterPath;
            _curve2path.insert(curveModel,path);
        }
    } else {
        path = new QPainterPath;
        _curve2path.insert(curveModel,path);
    }
    __appendDataToPainterPath(curveModel, path,
                              (start-tb)/ts,(stop-tb)/ts,
                              xs, xb, ys, yb,
                              plotXScale, plotYScale);
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
    double ys0 = getDataDouble(idx0,"CurveYScale","Curve");
    double ys1 = getDataDouble(idx1,"CurveYScale","Curve");
    double yb0 = getDataDouble(idx0,"CurveYBias","Curve");
    double yb1 = getDataDouble(idx1,"CurveYBias","Curve");
    double xb0 = getDataDouble(idx0,"CurveXBias","Curve");
    double xb1 = getDataDouble(idx1,"CurveXBias","Curve");
    double xs0 = getDataDouble(idx0,"CurveXScale","Curve");
    double xs1 = getDataDouble(idx1,"CurveXScale","Curve");
    if ( !dpUnits0.isEmpty() ) {
        ys0 *= Unit::scale(c0->y()->unit(),dpUnits0);
        yb0 += Unit::bias(c0->y()->unit(),dpUnits0);
        ys1 *= Unit::scale(c1->y()->unit(),dpUnits0);
        yb1 += Unit::bias(c1->y()->unit(),dpUnits0);
    } else {
        ys1 *= Unit::scale(c1->y()->unit(),c0->y()->unit());
        yb1 += Unit::bias(c1->y()->unit(),c0->y()->unit());
    }

    // By default the tolerance is 0.000001
    double tolerance = getDataDouble(QModelIndex(),"TimeMatchTolerance");

    // Frequency of data to show (f=0.0, the default, is all data)
    double f = getDataDouble(QModelIndex(),"Frequency");
    if ( f != 0.0 ) {
        fprintf(stderr, "koviz [error]: Frequency spec unsupported with "
                        "error plots.\n");
        exit(-1);
    }

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
        double t0 = xs0*i0->t()+xb0;
        double t1 = xs1*i1->t()+xb1;
        double yy = (ys0*i0->y()+yb0) - (ys1*i1->y()+yb1);
        // Match timestamps as close as possible (freq not used)
        if ( t0 == t1 ) {
            i0->next();
            i1->next();
        } else if ( t0 < t1 ) {
            i0->next();
            while ( !i0->isDone() ) {
                double t00 = xs0*i0->t()+xb0;
                double dtt = qAbs(t1-t00);
                if ( dtt < qAbs(t0-t1) ) {
                    t0 = t00;
                    yy = (ys0*i0->y()+yb0) - (ys1*i1->y()+yb1);
                    i0->next();
                } else {
                    break;
                }
            }
            i1->next();
        } else if ( t0 > t1 ) {
            i1->next();
            while ( !i1->isDone() ) {
                double t11 = xs1*i1->t()+xb1;
                double dtt = qAbs(t0-t11);
                if ( dtt < qAbs(t1-t0) ) {
                    t1 = t11;
                    yy = (ys0*i0->y()+yb0) - (ys1*i1->y()+yb1);
                    i1->next();
                } else {
                    break;
                }
            }
            i0->next();
        } else {
            // bad scoobs, but step to avoid inf loop
            i0->next();
            i1->next();
        }
        if ( qAbs(t1-t0) <= tolerance ) {
            if ( isYLogScale ) {
                if ( yy > 0 ) {
                    yy = log10(yy);
                } else if ( yy < 0 ) {
                    yy = log10(-yy);
                } else if ( yy == 0 ) {
                    continue; // skip log(0) since -inf
                }
            }
            if ( t0 >= start && t0 <= stop ) {
                if ( isXLogScale && t0 == 0.0 ) {
                    continue;
                }
                if ( isXLogScale ) {
                    t0 = log10(t0);
                }
                if ( isFirst ) {
                    path->moveTo(t0,yy);
                    isFirst = false;
                } else {
                    path->lineTo(t0,yy);
                }
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
        if ( yunit0.isEmpty() ) {
            yunit0 = getCurveModel(curve0Idx)->y()->unit();
        }
        for (int i = 0; i < rc; ++i) {
            QModelIndex curveIdx = index(i,0,curvesIdx);
            yunit = getDataString(curveIdx,"CurveYUnit","Curve");
            if ( yunit.isEmpty() ) {
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
            return labels;  // No abbreviation since not colon delimited
        }
        int i = label.indexOf(':');
        QString run = label.left(i);
        QString var = label.mid(i+1);
        runs << run;
        vars << var;
    }
    QString runsPrefix = Runs::commonPrefix(runs,"/");
    QString runsSuffix = Runs::commonSuffix(runs,"/");
    QString varsPrefix = Runs::commonPrefix(vars,".");
    int i = runsPrefix.size();
    int j = varsPrefix.size();

    foreach (QString label, labels) {

        int k = label.indexOf(':');
        QString run = label.left(k);
        QString var = label.mid(k+1);

        run = run.mid(i);
        if ( i > 0 && run.startsWith("/") ) {
            run = run.mid(1); // remove prepended '/'
        }
        run = run.remove(runsSuffix);
        if ( run.endsWith("/") ) {
            run.chop(1);
        }

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

    // If any abbreviated label is empty, return original labels (no abbrev)
    bool isEmptyLabels = false;
    foreach ( QString lbl, lbls ) {
        if ( lbl.isEmpty() ) {
            isEmptyLabels = true;
            break;
        }
    }
    if ( isEmptyLabels ) {
        lbls = labels;
    }

    return lbls;
}

QStringList PlotBookModel::legendSymbols(const QModelIndex &plotIdx) const
{
    QStringList symbols;
    QList<PlotBookModel::LegendElement> els = _legendElements(plotIdx);
    foreach (PlotBookModel::LegendElement el, els ) {
        symbols << el.symbolstyle;
    }
    return symbols;
}

QStringList PlotBookModel::legendColors(const QModelIndex &plotIdx) const
{
    QStringList colors;

    QList<PlotBookModel::LegendElement> els = _legendElements(plotIdx);
    foreach (PlotBookModel::LegendElement el, els ) {
        colors << el.color;
    }
    return colors;
}

QStringList PlotBookModel::legendLinestyles(const QModelIndex &plotIdx) const
{
    QStringList styles;

    QList<PlotBookModel::LegendElement> els = _legendElements(plotIdx);
    foreach (PlotBookModel::LegendElement el, els ) {
        styles << el.linestyle;
    }
    return styles;
}

QStringList PlotBookModel::legendLabels(const QModelIndex &plotIdx) const
{
    QStringList labels;

    QList<PlotBookModel::LegendElement> els = _legendElements(plotIdx);
    foreach (PlotBookModel::LegendElement el, els ) {
        labels << el.label;
    }
    return labels;
}

QList<QPen*> PlotBookModel::legendPens(const QModelIndex &plotIdx,
                                    enum QPaintEngine::Type painterType) const
{
    QList<QPen*> pens;

    QList<PlotBookModel::LegendElement> els = _legendElements(plotIdx);

    foreach ( PlotBookModel::LegendElement el, els ) {

        QString penColor = el.color;
        QPen* pen = new QPen(penColor);
        QVector<qreal> pat = getLineStylePattern(el.linestyle);
        pen->setDashPattern(pat);
        QString style = el.linestyle;
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

    // If -legend option off, return false in any case
    // If -legend option on, it doesn't necessarily mean there is a legend
    if ( isChildIndex(QModelIndex(),"","IsLegend") ) {
        QModelIndex isLegendIdx =getDataIndex(QModelIndex(),"IsLegend");
        bool isLeg = data(isLegendIdx).toBool() ;
        if ( !isLeg ) {
            return isLegend;
        }
    }

    // If page title is off, return false
    if ( isChildIndex(QModelIndex(),"","IsShowPageTitle") ) {
        bool isPageTitle = getDataBool(QModelIndex(),"IsShowPageTitle");
        if ( !isPageTitle ) {
            return isLegend;
        }
    }

    // Check to make sure CurvesIdx exists before calculating
    bool isCurves = true;
    QModelIndexList plotIdxs = this->plotIdxs(pageIdx);
    foreach ( QModelIndex plotIdx, plotIdxs ) {
        if ( !isChildIndex(plotIdx,"Plot","Curves") ) {
            isCurves = false;
            break;
        }
    }

    // Is commandline legend labels used?
    bool isLegendLabelOverride = false;
    QModelIndex legendIdx = getIndex(QModelIndex(), "LegendLabels","");
    QStringList labels;
    labels << getDataString(legendIdx,"Label1","LegendLabels");
    labels << getDataString(legendIdx,"Label2","LegendLabels");
    labels << getDataString(legendIdx,"Label3","LegendLabels");
    labels << getDataString(legendIdx,"Label4","LegendLabels");
    labels << getDataString(legendIdx,"Label5","LegendLabels");
    labels << getDataString(legendIdx,"Label6","LegendLabels");
    labels << getDataString(legendIdx,"Label7","LegendLabels");
    foreach ( QString label, labels ) {
        if ( ! label.isEmpty() ) {
            isLegendLabelOverride = true;
            break;
        }
    }

    if ( isCurves && isPlotLegendsSame(pageIdx) && isLegendLabelOverride ) {
        isLegend = true;
    } else if ( isCurves && isPlotLegendsSame(pageIdx) ) {
        QModelIndexList plotIdxs = this->plotIdxs(pageIdx);
        if ( !plotIdxs.isEmpty() ) {
            QModelIndex plotIdx = plotIdxs.at(0);
            if ( !legendLabels(plotIdx).isEmpty() ) {
                isLegend = true;
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

        if ( plotIdxs.size() == 1 ) {
            ok = true;
        } else if ( !plotIdxs.isEmpty() ) {
            QModelIndex plotIdx0 = plotIdxs.at(0);
            QList<LegendElement> els0 = _legendElements(plotIdx0);

            bool isFirst = true;
            ok = true;
            foreach ( QModelIndex plotIdx, plotIdxs ) {
                if ( isFirst ) {
                    // Skip first element since els0 is first element
                    isFirst = false;
                    continue;
                }
                QList<LegendElement> els = _legendElements(plotIdx);
                if ( els0.size() == els.size() ) {
                    int i = 0;
                    foreach ( LegendElement el, els ) {
                        if ( el != els0.at(i++) ) {
                            ok = false;
                            break;
                        }
                    }
                    if ( !ok ) {
                        break;
                    }
                } else {
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

// Does string match expression
// expression can be a regexp or may be a range
// This is a helper thing for views
// e.g. isMatch("RUN_U_dog","_U_") returns true (by regexp)
//      isMatch("RUN_a_dyn","a|b") returns true (by regexp)
//      isMatch("RUN_499","250,1000") returns true (by range)
//      isMatch("RUN_499","500,1000") returns false (by range)
bool PlotBookModel::isMatch(const QString &str, const QString &exp) const
{
    bool ok = false;

#if QT_VERSION >= 0x050000
    bool isRange = false;
    if ( str.contains("RUN_") && exp.contains(',') ) {
        QStringList els = exp.split(',',QString::SkipEmptyParts);
        if ( els.size() == 2 ) {
            QString el0 = els.at(0).trimmed();
            QString el1 = els.at(1).trimmed();
            int a = el0.toInt(&ok);
            if ( ok ) {
                int b = el1.toInt(&ok) ;
                if ( ok ) {
                    int i = str.lastIndexOf("RUN_");
                    QString s = str.mid(i+4);
                    int runNum = s.toInt(&ok);
                    if ( ok && a <= b ) {
                        isRange = true;
                        if ( a <= runNum && runNum <= b ) {
                            ok = true;
                        } else {
                            ok = false;
                        }
                    }
                }
            }
        }
    }

    if ( !isRange ) {
        // Try regular expression match
        QRegularExpression rx(exp);
        QRegularExpressionMatch match = rx.match(str);
        ok = match.hasMatch();
    }
#endif

    return ok;
}

QList<double> PlotBookModel::majorXTics(const QModelIndex& plotIdx) const
{
    QList<double> X;

    QRectF r = getPlotMathRect(plotIdx);
    double a = r.left();
    double b = r.right();
    QVariantList customTics;
    if ( isChildIndex(plotIdx, "Plot", "PlotMajorXTics") ) {
        customTics = getDataList(plotIdx,"PlotMajorXTics","Plot");
    }
    QString plotScale;
    if ( isChildIndex(plotIdx, "Plot", "PlotXScale") ) {
        plotScale = getDataString(plotIdx,"PlotXScale","Plot");
    }
    X = _majorTics(a,b,customTics,plotScale);

    return X;
}

QList<double> PlotBookModel::minorXTics(const QModelIndex &plotIdx) const
{
    QList<double> X;

    QVariantList customMinorTics;
    if ( isChildIndex(plotIdx, "Plot", "PlotMinorXTics") ) {
        customMinorTics = getDataList(plotIdx,"PlotMinorXTics","Plot");
    }

    QVariantList customMajorTics;
    if ( isChildIndex(plotIdx, "Plot", "PlotMajorXTics") ) {
        customMajorTics = getDataList(plotIdx,"PlotMajorXTics","Plot");
    }

    QRectF r = getPlotMathRect(plotIdx);
    double a = r.left();
    double b = r.right();
    QString plotScale = getDataString(plotIdx,"PlotXScale","Plot");

    X = _minorTics(a,b,customMajorTics,customMinorTics,plotScale);

    return X;
}

QList<double> PlotBookModel::majorYTics(const QModelIndex &plotIdx) const
{
    QList<double> Y;

    QRectF r = getPlotMathRect(plotIdx);
    double a = r.bottom();
    double b = r.top();
    QVariantList customTics;
    if ( isChildIndex(plotIdx, "Plot", "PlotMajorYTics") ) {
        customTics = getDataList(plotIdx,"PlotMajorYTics","Plot");
    }
    QString plotScale;
    if ( isChildIndex(plotIdx, "Plot", "PlotYScale") ) {
        plotScale = getDataString(plotIdx,"PlotYScale","Plot");
    }
    Y = _majorTics(a,b,customTics,plotScale);

    return Y;
}

QList<double> PlotBookModel::minorYTics(const QModelIndex &plotIdx) const
{
    QList<double> Y;

    QVariantList customMinorTics;
    if ( isChildIndex(plotIdx, "Plot", "PlotMinorYTics") ) {
        customMinorTics = getDataList(plotIdx,"PlotMinorYTics","Plot");
    }

    QVariantList customMajorTics;
    if ( isChildIndex(plotIdx, "Plot", "PlotMajorYTics") ) {
        customMajorTics = getDataList(plotIdx,"PlotMajorYTics","Plot");
    }

    QRectF r = getPlotMathRect(plotIdx);
    double a = r.bottom();
    double b = r.top();
    QString plotScale = getDataString(plotIdx,"PlotYScale","Plot");

    Y = _minorTics(a,b,customMajorTics,customMinorTics,plotScale);

    return Y;
}

QStandardItem *PlotBookModel::createPageItem()
{
    QModelIndex pagesIdx = getIndex(QModelIndex(), "Pages");
    QStandardItem* pagesItem = itemFromIndex(pagesIdx);
    QStandardItem* pageItem = addChild(pagesItem, "Page");

    QString fg = getDataString(QModelIndex(),"ForegroundColor");
    if ( fg.isEmpty() ) {
        fg = "#000000";
    }
    QString bg = getDataString(QModelIndex(),"BackgroundColor");
    if ( bg.isEmpty() ) {
        bg = "#FFFFFF";
    }

    static int pageId = 0 ;
    QString pageName = QString("Page_%0").arg(pageId++);
    addChild(pageItem, "PageName", pageName);
    addChild(pageItem, "PageTitle", "Koviz");
    addChild(pageItem, "PageStartTime", -DBL_MAX);
    addChild(pageItem, "PageStopTime",   DBL_MAX);
    addChild(pageItem, "PageBackgroundColor", bg);
    addChild(pageItem, "PageForegroundColor", fg);
    addChild(pageItem, "Plots");

    return pageItem;
}

QStandardItem* PlotBookModel::createPlotItem(QStandardItem *pageItem,
                                             const QString& timeName,
                                             const QString &yName,
                                             const QStringList& unitOverrides,
                                             QWidget* parent)
{
    QModelIndex pageIdx = indexFromItem(pageItem);
    QModelIndex plotsIdx = getIndex(pageIdx, "Plots", "Page");
    QStandardItem* plotsItem = itemFromIndex(plotsIdx);
    QStandardItem* plotItem = addChild(plotsItem, "Plot");

    int plotId = plotItem->row();
    QString plotName = QString("Plot_%0").arg(plotId);

    addChild(plotItem, "PlotName", plotName);
    addChild(plotItem, "PlotTitle", "");
    addChild(plotItem, "PlotMathRect", QRectF());
    addChild(plotItem, "PlotStartTime", -DBL_MAX);
    addChild(plotItem, "PlotStopTime",   DBL_MAX);
    addChild(plotItem, "PlotGrid", true);
    addChild(plotItem, "PlotRatio", "");
    addChild(plotItem, "PlotXScale", "linear");
    addChild(plotItem, "PlotYScale", "linear");
    addChild(plotItem, "PlotXMinRange", -DBL_MAX);
    addChild(plotItem, "PlotXMaxRange",  DBL_MAX);
    addChild(plotItem, "PlotYMinRange", -DBL_MAX);
    addChild(plotItem, "PlotYMaxRange",  DBL_MAX);
    addChild(plotItem, "PlotBackgroundColor", "#FFFFFF");
    addChild(plotItem, "PlotForegroundColor", "#000000");
    addChild(plotItem, "PlotPresentation", "compare");
    addChild(plotItem, "PlotXAxisLabel", timeName);
    addChild(plotItem, "PlotYAxisLabel", yName);
    addChild(plotItem, "PlotRect", QRect(0,0,0,0));

    QStandardItem *curvesItem = addChild(plotItem,"Curves");
    QModelIndex curvesIdx = indexFromItem(curvesItem);
    createCurves(curvesIdx,timeName,yName,unitOverrides,0,parent);

    return plotItem;
}

void PlotBookModel::createCurves(QModelIndex curvesIdx,
                                 const QString& timeName,
                                 const QString &yName,
                                 const QStringList &unitOverrides,
                                 QAbstractItemModel* monteModel,
                                 QWidget *parent)
{
    // Turn off model signals when adding children for significant speedup
    bool block = blockSignals(true);

    int rc = _runs->runDirs().count();
    QList<QColor> colors = createCurveColors(rc);

    QHash<int,QString> run2color;
    if ( monteModel ) {
        for ( int r = 0; r < rc; ++r ) {
            QModelIndex runIdx = monteModel->index(r,0);
            int runId = monteModel->data(runIdx).toInt();
            run2color.insert(runId, colors.at(r).name());
        }
    }

    // Setup progress bar dialog for time intensive loads
    QProgressDialog progress("Loading curves...", "Abort", 0, rc, parent);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(500);

#ifdef __linux
    TimeItLinux timer;
    timer.start();
#endif

    bool isGroups = false;
    QStringList groups;
    QModelIndex groupsIdx = getIndex(QModelIndex(),"Groups","");
    int nGroups = rowCount(groupsIdx);
    for ( int i = 0; i < nGroups; ++i ) {
        QModelIndex groupIdx = index(i,1,groupsIdx);
        QString group = data(groupIdx).toString();
        groups << group;
        if ( !group.isEmpty() ) {
            isGroups = true;
        }
    }

    int nCurves = rowCount(curvesIdx);
    int ii = nCurves; // When alt+click adding curves, this is needed
    int jj = 0;
    QString u0;
    QString r0;
    for ( int r = 0; r < rc; ++r) {

        // Update progress dialog
        progress.setValue(r);
        if (progress.wasCanceled()) {
            break;
        }

        CurveModel* curveModel = createCurve(r,timeName,timeName,yName);
        if ( !curveModel ) {
            // This should not happen
            fprintf(stderr, "koviz [bad scoobs]: bookmodel.cpp\n"
                            "curve(%d,%s,%s,%s) failed.  Aborting!!!\n",
                    r,
                    timeName.toLatin1().constData(),
                    timeName.toLatin1().constData(),
                    yName.toLatin1().constData());
            exit(-1);
        }

        QStandardItem* curvesItem = itemFromIndex(curvesIdx);
        QStandardItem *curveItem = addChild(curvesItem,"Curve");

        addChild(curveItem, "CurveRunID", r);
        addChild(curveItem, "CurveTimeName", timeName);
        addChild(curveItem, "CurveTimeUnit", curveModel->t()->unit());
        addChild(curveItem, "CurveXName", timeName);
        addChild(curveItem, "CurveXUnit", curveModel->t()->unit()); // yes,t
        addChild(curveItem, "CurveYName", yName);
        addChild(curveItem, "CurveXMinRange", -DBL_MAX);
        addChild(curveItem, "CurveXMaxRange",  DBL_MAX);
        addChild(curveItem, "CurveYMinRange", -DBL_MAX);
        addChild(curveItem, "CurveYMaxRange",  DBL_MAX);
        addChild(curveItem, "CurveSymbolSize", "");

        QString yunit;
        if ( r == 0 ) {
            if ( rowCount(curvesIdx) > 1 ) {
                // Multiple vars on plot with single run
                QModelIndex curveIdx0 = getIndex(curvesIdx,"Curve", "Curves");
                u0 = getDataString(curveIdx0,"CurveYUnit","Curve");
                QString u1 = curveModel->y()->unit();
                if ( Unit::canConvert(u0,u1) ) {
                    yunit = u0;
                } else {
                    yunit = u1;
                }
            } else {
                u0 = curveModel->y()->unit();
                yunit = u0;
            }
            r0 = QFileInfo(curveModel->fileName()).dir().dirName();
        } else {
            yunit = curveModel->y()->unit();
            QString u1 = yunit;
            if ( Unit::canConvert(u0,u1) ) {
                // Use unit of first curve
                yunit = u0;
            }
        }
        if ( !unitOverrides.isEmpty() ) {
            foreach ( QString overrideUnit, unitOverrides ) {
                Unit mUnit = Unit::map(yunit,overrideUnit);
                if ( !mUnit.isEmpty() ) {
                    // No break if found, so last override in list used
                    yunit = mUnit.name();
                }
            }
        }
        addChild(curveItem, "CurveYUnit", yunit);

        addChild(curveItem, "CurveXScale", curveModel->x()->scale());
        QHash<QString,QVariant> shifts = getDataHash(QModelIndex(),
                                                     "RunToShiftHash");
        QString curveRunDir = QFileInfo(curveModel->fileName()).absolutePath();
        if ( shifts.contains(curveRunDir) ) {
            double shiftVal = shifts.value(curveRunDir).toDouble();
            addChild(curveItem, "CurveXBias", shiftVal);
        } else {
            // x bias can be set in a mapfile
            addChild(curveItem, "CurveXBias", curveModel->x()->bias());
        }
        addChild(curveItem, "CurveYScale", curveModel->y()->scale());
        addChild(curveItem, "CurveYBias", curveModel->y()->bias());

        // Color
        QString color;
        nCurves = rowCount(curvesIdx);
        if ( rc == 1 ) {
            // Color each variable differently
            QList<QColor> curveColors = createCurveColors(nCurves);
            color = curveColors.at(nCurves-1).name();
        } else {
            // Color RUNs the same
            if ( monteModel ) {
                color = run2color.value(r);
            } else {
                color = colors.at(r).name();
            }
        }
        QModelIndex lcIdx = getIndex(QModelIndex(),"LegendColors","");
        if ( ii < rowCount(lcIdx) ) {
            // Possible commandline color override
            QModelIndex legendColorIdx = index(ii,1,lcIdx);
            QString legendColor = data(legendColorIdx).toString();
            if ( !legendColor.isEmpty() ) {
                QString group;
                if ( ii < groups.size() ) {
                    group = groups.at(ii);
                }
                if ( group.isEmpty() ) {
                    color = legendColor;
                }
            }
        }

        // Linestyle
        QString style;
        QStringList styles = lineStyles();
        nCurves = rowCount(curvesIdx);
        if ( rc == 1 ) {
            style = styles.at(0);
        } else {
            // Style each variable differently
            div_t q = div(nCurves-1,rc);
            style = styles.at((q.quot)%(styles.size()));
        }
        QModelIndex lsIdx = getIndex(QModelIndex(),"Linestyles","");
        if ( ii < rowCount(lsIdx) ) {
            // Possible commandline linestyle override
            QModelIndex legendStyleIdx = index(ii,1,lsIdx);
            QString legendStyle = data(legendStyleIdx).toString();
            if ( !legendStyle.isEmpty() ) {
                QString group;
                if ( ii < groups.size() ) {
                    group = groups.at(ii);
                }
                if ( group.isEmpty() ) {
                    // E.g. if -l4 dog -ls4 thick_line -g4 99,
                    //      then override handled in the group logic below
                    style = legendStyle;
                }
            }
        }

        // Symbol Style
        QString symbolStyle = "none";
        QModelIndex ssIdx = getIndex(QModelIndex(),"Symbolstyles","");
        if ( ii < rowCount(ssIdx) ) {
            QModelIndex symbolStyleIdx = index(ii,1,ssIdx);
            QString ss = data(symbolStyleIdx).toString();
            if ( !ss.isEmpty() ) {
                QString group;
                if ( ii < groups.size() ) {
                    group = groups.at(ii);
                }
                if ( group.isEmpty() ) {
                    // Use symbolstyle from commandline
                    symbolStyle = ss;
                }
            }
        }

        // Symbol End
        QString symbolEnd = "none";
        QModelIndex seIdx = getIndex(QModelIndex(),"Symbolends","");
        if ( ii < rowCount(seIdx) ) {
            QModelIndex symbolEndIdx = index(ii,1,seIdx);
            QString se = data(symbolEndIdx).toString();
            if ( !se.isEmpty() ) {
                QString group;
                if ( ii < groups.size() ) {
                    group = groups.at(ii);
                }
                if ( group.isEmpty() ) {
                    // Use symbolEnd from commandline
                    symbolEnd = se;
                }
            }
        }

        // Label
        QString yLabel = yName;
        QModelIndex llIdx = getIndex(QModelIndex(),"LegendLabels","");
        if ( ii < rowCount(llIdx) ) {
            QString llTag = QString("Label%1").arg(ii+1);
            QString ll = getDataString(llIdx,llTag,"LegendLabels");
            if ( !ll.isEmpty() ) {
                QString group;
                if ( ii < groups.size() ) {
                    group = groups.at(ii);
                }
                if ( group.isEmpty() ) {
                    // Use commandline label
                    yLabel = ll;
                }
            }
        }

        // Handle groups (-g1, -g2... -g7 options)
        if ( isGroups ) {
            int i = 0;
            foreach ( QString group, groups ) {
                if ( !group.isEmpty() ) {
                    if ( isMatch(curveRunDir,group) ) {
                        // Color
                        QModelIndex idx = index(i,1,lcIdx);
                        QString cc = data(idx).toString();
                        if ( !cc.isEmpty() ) {
                            color = cc ;
                        }

                        // Label
                        QString llTag = QString("Label%1").arg(i+1);
                        QString ll = getDataString(llIdx,llTag, "LegendLabels");
                        if ( !ll.isEmpty() ) {
                            yLabel = ll ;
                        } else {
                            yLabel = group;
                        }

                        // Linestyle
                        idx = index(i,1,lsIdx);
                        QString ls = data(idx).toString();
                        if ( !ls.isEmpty() ) {
                            style = ls;
                        }

                        // Symbolstyle
                        idx = index(i,1,ssIdx);
                        QString ss = data(idx).toString();
                        if ( !ss.isEmpty() ) {
                            symbolStyle = ss;
                        }

                        // Symbolend
                        idx = index(i,1,seIdx);
                        QString se = data(idx).toString();
                        if ( !se.isEmpty() ) {
                            symbolEnd = se;
                        }

                        // Match found and handled
                        break;
                    }
                }
                ++i;
            }
        }

        addChild(curveItem, "CurveYLabel", yLabel);
        addChild(curveItem, "CurveColor", color);
        addChild(curveItem, "CurveLineStyle",style);
        addChild(curveItem, "CurveSymbolStyle", symbolStyle);
        addChild(curveItem, "CurveSymbolEnd", symbolEnd);

        // Add actual curve model data
        if ( r == rc-1) {
            // Turn signals on for last curve for pixmap update
            blockSignals(false);
        }
        QVariant v = PtrToQVariant<CurveModel>::convert(curveModel);
        addChild(curveItem, "CurveData", v);
        if ( r == rc-1) {
            blockSignals(true);
        }

#ifdef __linux
        int secs = qRound(timer.stop()/1000000.0);
        div_t d = div(secs,60);
        QString msg = QString("Loaded %1 of %2 curves (%3 min %4 sec)")
                             .arg(r+1).arg(rc).arg(d.quot).arg(d.rem);
        progress.setLabelText(msg);
#endif

        ++ii;
        ++jj;
    }

    // Turn signals back on before adding curveModel
    blockSignals(block);

    // Update progress dialog
    progress.setValue(rc);

    // Initialize plot math rect
    QRectF bbox = calcCurvesBBox(curvesIdx);
    QModelIndex plotIdx = curvesIdx.parent();
    QModelIndex pageIdx = plotIdx.parent().parent();
    QModelIndex plotMathRectIdx = getDataIndex(plotIdx,
                                               "PlotMathRect",
                                               "Plot");
    QModelIndexList siblingPlotIdxs = plotIdxs(pageIdx);
    foreach ( QModelIndex siblingPlotIdx, siblingPlotIdxs ) {
        if ( isXTime(siblingPlotIdx) ) {
            QRectF sibPlotRect = getPlotMathRect(siblingPlotIdx);
            if ( sibPlotRect.width() > 0 ) {
                bbox.setLeft(sibPlotRect.left());
                bbox.setRight(sibPlotRect.right());
            }
            break;
        }
    }
    setData(plotMathRectIdx,bbox);
}

void PlotBookModel::liveTimeNext(const QModelIndex& idx)
{
    if ( !idx.isValid() ) {
        return;
    }

    CurveModel* curveModel = getCurveModel(idx);

    if ( curveModel ) {

        curveModel->map();

        ModelIterator* it = curveModel->begin();

        // Curve x bias and x scale
        double xs = xScale(idx);
        double xb = xBias(idx);

        // Calculate curve time index
        int i = 0;
        QModelIndex liveIdx = getDataIndex(QModelIndex(), "LiveCoordTime");
        double liveTime = data(liveIdx).toDouble();
        i = curveModel->indexAtTime((liveTime-xb)/xs);

        /* Timestamps may duplicate, go to first in series */
        double itTime = it->at(i)->t();
        while ( i > 0 ) {
            if ( it->at(i-1)->t() == itTime ) {
                --i;
            } else {
                break;
            }
        }

        QModelIndex lctIdx = getDataIndex(QModelIndex(),
                                          "LiveCoordTimeIndex","");
        int ii = getDataInt(QModelIndex(), "LiveCoordTimeIndex","");

        // Calculate nextTime after liveTime
        bool isXTime = (curveModel->x()->name() == curveModel->t()->name());
        double nextTime = liveTime;
        it = it->at(i+1+ii);
        while ( !it->isDone() ) {
            if ( isXTime ) {
                nextTime = it->x()*xs+xb;
            } else {
                nextTime = it->t();
            }
            double dt = qAbs(nextTime-liveTime);
            if ( dt == 0.0 ) {
                // Multiple points for same timestamp,
                // move to next point on this curve for this same timestamp
                setData(lctIdx,++ii);
                break;
            } else {
                if ( ii != 0 ) {
                    setData(lctIdx,0);
                }
            }
            if ( dt > 1.0e-16 ) {
                break;
            }
            it->next();
        }
        delete it;

        // nextTime should not exceed stop time
        double stop  = getDataDouble(QModelIndex(),"StopTime");
        nextTime = (nextTime > stop)  ? stop  : nextTime;

        // Update liveTime to nextTime
        setData(liveIdx,nextTime);

        curveModel->unmap();

        QString tag = data(idx.sibling(idx.row(),0)).toString();
        if ( tag == "Plot" ) {
            delete curveModel;
        }
    }
}

void PlotBookModel::liveTimePrev(const QModelIndex &idx)
{
    if ( !idx.isValid() ) {
        return;
    }

    CurveModel* curveModel = getCurveModel(idx);

    if ( curveModel ) {

        curveModel->map();

        // Curve x bias and x scale
        double xs = xScale(idx);
        double xb = xBias(idx);

        // Calculate curve time index
        int i = 0;
        QModelIndex liveIdx = getDataIndex(QModelIndex(), "LiveCoordTime");
        double liveTime = data(liveIdx).toDouble();
        bool isXTime = (curveModel->x()->name() == curveModel->t()->name());
        i = curveModel->indexAtTime((liveTime-xb)/xs);

        double prevTime = liveTime;
        ModelIterator* it = curveModel->begin();

        /* Timestamps may be identical, set i to first duplicate */
        double itTime = it->at(i)->t();
        while ( i >= 1 ) {
            if ( it->at(i-1)->t() == itTime ) {
                --i;
            } else {
                break;
            }
        }

        /* Get current index for possible duplicate timestamps (ii) */
        QModelIndex idx = getDataIndex(QModelIndex(), "LiveCoordTimeIndex","");
        int ii = getDataInt(QModelIndex(), "LiveCoordTimeIndex","");

        while ( i+ii > 0 ) {
            it = it->at(i+ii-1);
            if ( isXTime ) {
                prevTime = it->x()*xs+xb;
            } else {
                prevTime = it->t();
            }
            double dt = qAbs(liveTime-prevTime);
            if ( dt == 0 ) {
                // Multiple points for same timestamp,
                // move to prev point on this curve for this same timestamp
                setData(idx,--ii);
                break;
            } else {
                int jj = 0;  // Last index of possible duplicate timestamps
                int j = i-1;
                double jTime = it->at(j+ii)->t();
                while ( j+ii-1 >= 0 ) {
                    if ( it->at(j+ii-1)->t() == jTime ) {
                        ++jj;
                        --j;
                    } else {
                        break;
                    }
                }
                setData(idx,jj);
            }
            if ( dt > 1.0e-16 ) {
                break;
            }
            --i;
        }
        delete it;

        // prevTime should not precede start time
        double start  = getDataDouble(QModelIndex(),"StartTime");
        prevTime = (prevTime < start)  ? start  : prevTime;

        // Update liveTime to prevTime
        setData(liveIdx,prevTime);

        curveModel->unmap();

        QString tag = data(idx.sibling(idx.row(),0)).toString();
        if ( tag == "Plot" ) {
            delete curveModel;
        }
    }
}

QList<double> PlotBookModel::_majorTics(double a, double b,
                                        const QList<QVariant> &customTics,
                                        const QString &plotScale) const
{
    QList<double> T;

    if ( customTics.isEmpty() || plotScale == "log") {  // normal case
        // Note: no custom tics in logscale
        T = _calcTicSet(a,b,1.0,10.0);
    } else {
        foreach ( QVariant customTic, customTics ) {
            bool ok;
            double tic = customTic.toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [badscoobs]:PlotBookModel::majorTics\n");
                exit(-1);
            }
            if ( a <= tic && tic <= b ) {
                T << tic;
            }
        }
    }
    return T;
}

QList<double> PlotBookModel::_minorTics(double a, double b,
                                        const QList<QVariant>& customMajorTics,
                                        const QList<QVariant>& customMinorTics,
                                        const QString& plotScale) const
{
    QList<double> T;

    if ( customMinorTics.isEmpty() || plotScale == "log" ) {
        QList<double> majorTics;
        if ( customMajorTics.isEmpty() || plotScale == "log" ) {
            majorTics = _calcTicSet(a,b,1.0,10.0);
            T = __minorTics(a,b,majorTics,plotScale);
        } else {
            foreach ( QVariant vtic, customMajorTics ) {
                bool ok;
                double tic = vtic.toDouble(&ok);
                if ( !ok ) {
                    fprintf(stderr, "koviz [bad scoobs]:1 _minorTics\n");
                    exit(-1);
                }
                majorTics << tic;
            }
            if ( majorTics.size() >= 2 ) {
                double firstMajor = majorTics.first();
                double lastMajor = majorTics.last();
                foreach ( double tic, __minorTics(a,b,majorTics,plotScale) ) {
                    // If custom majors, no minors outside majors
                    if ( firstMajor < tic && tic < lastMajor ) {
                        T << tic;
                    }
                }
            }
        }
    } else {
        foreach ( QVariant vtic, customMinorTics ) {
            bool ok;
            double tic = vtic.toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr, "koviz [bad scoobs]:2 _minorTics\n");
                exit(-1);
            }
            T << tic;
        }
    }
    return T;
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
        ulong na = maxD;
        ulong nb = maxD;
        if ( a == 0 ) {
            if ( b > 0 ) {
                memcpy(&n,&b,sizeof(ulong));
            } else if ( b < 0 ) {
                double bb = qAbs(b);
                memcpy(&n,&bb,sizeof(ulong));
            }
        } else if ( a > 0 ) {
            if ( b == 0 ) {
                memcpy(&n,&a,sizeof(ulong));
            } else if ( b > 0 ) {
                memcpy(&na,&a,sizeof(ulong));
                memcpy(&nb,&b,sizeof(ulong));
                if ( b > a ) {
                    n = nb - na;
                } else {
                    n = na - nb;
                }
            } else if ( b < 0 ) {
                double bb = qAbs(b);
                memcpy(&na,&a,sizeof(ulong));
                memcpy(&nb,&bb,sizeof(ulong));
                n = na + nb;
            }
        } else if ( a < 0 ) {
            if ( b == 0 ) {
                memcpy(&n,&a,sizeof(ulong));
            } else if ( b > 0 ) {
                double aa = qAbs(a);
                memcpy(&na,&aa,sizeof(ulong));
                memcpy(&nb,&b,sizeof(ulong));
                n = na + nb;
            } else if ( b < 0 ) {
                double aa = qAbs(a);
                double bb = qAbs(b);
                memcpy(&na,&aa,sizeof(ulong));
                memcpy(&nb,&bb,sizeof(ulong));
                if ( bb > aa ) {
                    n = nb - na;
                } else {
                    n = na - nb;
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

QList<PlotBookModel::LegendElement> PlotBookModel::_legendElements(
        const QModelIndex &plotIdx) const
{
    QList<PlotBookModel::LegendElement> els;

    QModelIndex curvesIdx = getIndex(plotIdx,"Curves","Plot");
    QModelIndexList curveIdxs = this->curveIdxs(curvesIdx);
    int nCurves = curveIdxs.size();

    QList<PlotBookModel::LegendElement> overrides = _legendOverrides();
    bool isOverrides = false;
    foreach ( PlotBookModel::LegendElement el, overrides ) {
        if ( !el.isEmpty() ) {
            isOverrides = true;
            break;
        }
    }

    if ( nCurves <= 1 && !isOverrides ) {
        return els;
    }

    bool isGroups = false;
    QStringList groups;
    QModelIndex groupsIdx = getIndex(QModelIndex(),"Groups","");
    int nGroups = rowCount(groupsIdx);
    for ( int i = 0; i < nGroups; ++i ) {
        QModelIndex groupIdx = index(i,1,groupsIdx);
        QString group = data(groupIdx).toString();
        groups << group;
        if ( !group.isEmpty() ) {
            isGroups = true;
        }
    }

    QStringList labels;
    foreach (QModelIndex curveIdx, curveIdxs) {
        QString lbl = getDataString(curveIdx,"CurveYLabel","Curve");
        if ( lbl.isEmpty() ) {
            lbl = getDataString(curveIdx,"CurveYName","Curve");
        }
        if ( !isGroups ) {
            CurveModel* curve = getCurveModel(curveIdx);
            QString runDir = QFileInfo(curve->fileName()).absolutePath();
            lbl = runDir + ":" + lbl;
        }
        labels << lbl;
        if ( !isOverrides && labels.size() > overrides.size() ) {
            // Too many labels for legend, return empty!
            return els;
        }
    }
    labels = abbreviateLabels(labels);

#if QT_VERSION >= 0x050000
    QHash<PlotBookModel::LegendElement,int> elHash;
#endif
    int i = 0;
    foreach (QModelIndex curveIdx, curveIdxs) {
        PlotBookModel::LegendElement el;
        el.label = labels.at(i);
        el.color = getDataString(curveIdx,"CurveColor","Curve");
        el.linestyle = getDataString(curveIdx, "CurveLineStyle","Curve");
        el.symbolstyle = getDataString(curveIdx,"CurveSymbolStyle","Curve");
#if QT_VERSION >= 0x050000
        if ( !elHash.contains(el) ) {
            elHash.insert(el,1);
#else
        if ( !els.contains(el) ) {
#endif
            els << el;
            if ( isGroups ) {
                int runid = getDataInt(curveIdx,"CurveRunID","Curve");
                QString runDir = _runs->runDirs().at(runid);
                int j = 0;
                foreach ( QString group, groups ) {
                    if ( !group.isEmpty() ) {
                        if ( isMatch(runDir,group) ) {
                            if ( j < els.size() ) {
                                // Move element to match group number (-g#)
                                els.move(els.size()-1,j);
                            }
                            break;
                        }
                    }
                    ++j;
                }
            }
        }
        ++i;
    }

    if ( isOverrides ) {
       // Resize els to num overrides
        if ( els.size() > overrides.size() ) {

            // Resize els to match overrides
            while ( els.size() > overrides.size() ) {
                els.removeLast();
            }

            // Remove trailing elements that do not have override
            int nels = overrides.size();
            for ( int i = nels-1; i >= 0; --i ) {
                PlotBookModel::LegendElement override = overrides.at(i);
                if ( override.isEmpty()  ) {
                    els.removeLast();
                } else {
                    break;
                }
            }
        }
    }

    if ( isOverrides ) { // Replace label with override label
        for ( int i = 0; i < els.size(); ++i ) {
            PlotBookModel::LegendElement override = overrides.at(i);
            if ( !override.label.isEmpty() ) {
                PlotBookModel::LegendElement el = els.at(i);
                el.label = override.label;
                els.replace(i,el);
            }
        }
    }

    return els;
}

QList<PlotBookModel::LegendElement> PlotBookModel::_legendOverrides() const
{
    QList<PlotBookModel::LegendElement> els;
    QModelIndex labelsIdx = getIndex(QModelIndex(), "LegendLabels","");
    QModelIndex colorsIdx = getIndex(QModelIndex(), "LegendColors","");
    QModelIndex linestylesIdx = getIndex(QModelIndex(), "Linestyles","");
    QModelIndex symbolstylesIdx = getIndex(QModelIndex(), "Symbolstyles","");

    int rc = rowCount(labelsIdx);
    for ( int i = 0; i < rc; ++i ) {
        QModelIndex labelIdx = index(i,1,labelsIdx);
        QModelIndex colorIdx = index(i,1,colorsIdx);
        QModelIndex linestyleIdx = index(i,1,linestylesIdx);
        QModelIndex symbolstyleIdx = index(i,1,symbolstylesIdx);

        PlotBookModel::LegendElement el;
        el.label = data(labelIdx).toString();
        el.color = data(colorIdx).toString();
        el.linestyle = data(linestyleIdx).toString();
        el.symbolstyle = data(symbolstyleIdx).toString();

        els << el;
    }

    return els;
}

QList<double> PlotBookModel::__minorTics(double a, double b,
                                               QList<double> &Majors,
                                               const QString &plotScale) const
{
    QList<double> Minors;

    if ( Majors.size() >= 2 ) {
        if ( plotScale == "linear" ) {
            double d = (Majors.at(1)-Majors.at(0))/4.0;
            double x = Majors.at(0);
            if ( x-d-d-d >= a ) Minors << x-d-d-d;
            if ( x-d-d >= a )   Minors << x-d-d;
            if ( x-d >= a )     Minors << x-d;

            for ( int ii = 0 ; ii < Majors.size(); ++ii ) {
                double d;
                if ( ii == 0 ) {
                    continue;
                } else {
                    d = (Majors.at(ii)-Majors.at(ii-1))/4.0;
                }
                double x = Majors.at(ii-1);
                if ( x+d <= b )     Minors << x+d;
                if ( x+d+d <= b )   Minors << x+d+d;
                if ( x+d+d+d <= b ) Minors << x+d+d+d;
            }

            int n = Majors.size()-1;
            d = (Majors.at(n)-Majors.at(n-1))/4.0;
            x = Majors.at(n);
            if ( x+d <= b )     Minors << x+d;
            if ( x+d+d <= b )   Minors << x+d+d;
            if ( x+d+d+d <= b ) Minors << x+d+d+d;
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
