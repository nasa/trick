#include "bookview_table.h"

BookTableView::BookTableView(QWidget *parent) :
    QAbstractItemView(parent),
    _mTop(3),
    _mBot(3),
    _mLft(3),
    _mRgt(3)
{
    setFrameShape(QFrame::Box);
}

void BookTableView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QRect W = viewport()->rect();
    if ( W.width() == 0.0 || W.height() == 0.0 ) {
        return;
    }

    QPainter painter(viewport());
    painter.save();
    QPen penOrig = painter.pen();
    QFontMetrics fm = painter.fontMetrics();

#if QT_VERSION >= 0x050000
    QPalette palette = QGuiApplication::palette();
#else
    QPalette palette = QApplication::palette();
#endif
    QPen penLight(palette.midlight().color());
    QPen penTxt(palette.text().color());

    // Per table variable, make list of curve models, units, scalefactors etc.
    QList<CurveModel*> curveModels;
    QStringList units;
    QModelIndex tableVarsIdx = _bookModel()->getIndex(rootIndex(),
                                                      "TableVars","Table");
    QModelIndexList tableVarIdxs = _bookModel()->getIndexList(tableVarsIdx,
                                                        "TableVar","TableVars");
    QList<double> scaleFactors;
    QList<double> biases;
    foreach (QModelIndex tableVarIdx, tableVarIdxs) {
        QModelIndex curveIdx = _bookModel()->getDataIndex(tableVarIdx,
                                                     "TableVarData","TableVar");
        QVariant v = _bookModel()->data(curveIdx);
        CurveModel* curveModel = QVariantToPtr<CurveModel>::convert(v);
        curveModels << curveModel;

        double sf = _bookModel()->getDataDouble(tableVarIdx,
                                                "TableVarScale","TableVar");

        QString unit = _bookModel()->getDataString(tableVarIdx,
                                                   "TableVarUnit","TableVar");
        if ( unit.isEmpty() ) {
            unit = curveModel->y()->unit();
        } else {
            sf *= Unit::scale(curveModel->y()->unit(),unit);
        }
        units << unit;
        scaleFactors << sf;

        double bias = _bookModel()->getDataDouble(tableVarIdx,
                                                  "TableVarBias","TableVar");
        bias += Unit::bias(curveModel->y()->unit(),unit);  // for temperature
        biases << bias;
    }
    QStringList labels = _columnLabels();

    // Calculate column width
    int w = fm.width("0123456789");
    foreach ( QString label, labels ) {
        if ( fm.width(label) > w ) {
            w = fm.width(label);
        }
    }
    w = _mLft + w + _mRgt;

    int nCols = W.width()/w;
    if ( nCols > labels.size() ) {
        nCols = labels.size();
    }

    int h = _mTop + fm.height() + _mBot;
    int nRows = W.height()/h;
    if ( nRows > _timeStamps.size() ) {
        nRows = _timeStamps.size()+1;   //+1 for header
    }

    double x   = verticalScrollBar()->value();
    double xmax = verticalScrollBar()->maximum();
    int p = (x/xmax)*_timeStamps.size();
    if ( p > _timeStamps.size()-nRows+1 ) {
        p = _timeStamps.size()-nRows+1;
    }

    double y   = horizontalScrollBar()->value();
    double ymax = horizontalScrollBar()->maximum();
    int q = (y/ymax)*labels.size();
    if ( q > labels.size()-nCols ) {
        q = labels.size()-nCols;
    }

    // Make list of values (per column)
    QHash<int,QStringList> col2svals;
    for (int j = 0; j < nCols; ++j) {
        double sf   = scaleFactors.at(q+j);
        double bias = biases.at(q+j);
        CurveModel* curveModel = curveModels.at(q+j);
        curveModel->map();
        ModelIterator* it = curveModel->begin();
        QList<double> vals;
        for ( int i = 1; i < nRows; ++i ) {
            double t = _timeStamps.at(p+i-1);
            if ( j == 0 ) {
                vals << t*sf + bias;
            } else {
                int k = curveModel->indexAtTime(t);
                if ( it->at(k)->t() == t ) {
                    vals << it->at(k)->y()*sf + bias;
                } else {
                    vals << 0; // place holder for blank data
                }
            }
        }
        QStringList svals = _format(vals);
        col2svals.insert(j,svals);
        delete it;
        curveModel->unmap();
    }

    // Draw the table
    for (int j = 0; j < nCols; ++j) {
        CurveModel* curveModel = curveModels.at(q+j);
        curveModel->map();
        ModelIterator* it = curveModel->begin();
        for ( int i = 0; i < nRows; ++i ) {
            int hline = h*(i+1);
            int baseline = hline - _mBot - fm.descent();
            painter.setPen(penLight);
            painter.drawLine(0,hline,W.width(),hline);
            painter.setPen(penTxt);
            QString s;
            if ( i == 0 ) {
                s = labels.at(q+j);
                int l = fm.width(labels.at(q+j));
                painter.drawText(w*j+(w-l),baseline,s);
            } else {
                if ( j == 0 ) {
                    s = col2svals.value(j).at(i-1);
                } else {
                    double t = _timeStamps.at(p+i-1);
                    int k = curveModel->indexAtTime(t);
                    if ( it->at(k)->t() == t ) {
                        s = col2svals.value(j).at(i-1);
                    } else {
                        // s is an empty string since no corresponding time
                    }
                }
                int l = fm.width(s);
                painter.drawText(w*j+(w-l),baseline,s);
            }
        }
        int vline = w*(j+1);
        painter.setPen(penLight);
        painter.drawLine(vline,0,vline,W.height());
        delete it;
        curveModel->unmap();
    }

    painter.setPen(penOrig);
    painter.restore();
    painter.end();
}

QStringList BookTableView::_format(const QList<double> &vals)
{
    QStringList list;

    list = __format(vals,"%g");

    bool isExponentialNotation = false;
    foreach ( QString s, list ) {
        if (s.contains("e")) {
            isExponentialNotation = true;
            break;
        }
    }

    if ( !isExponentialNotation ) {
        int maxPrecision = 0;
        foreach ( QString s, list ) {
            int prec = 0;
            if ( s.contains(".") ) {
                int i = s.indexOf(".");
                prec = s.length()-i-1;
            }
            if ( prec > maxPrecision ) {
                maxPrecision = prec;
            }
        }
        QString fmt = QString("%.%1lf").arg(maxPrecision);
        list = __format(vals,fmt);

        // If adjacent string values are the same, but the values are not
        // - try to reformat with higher precision
        // This is mainly to make timestamps different
        int i = 0;
        while (i < 9) {
            bool isFormat = false;
            for (int j = 0; j < list.size()-1; ++j) {
                if (list.at(j) == list.at(j+1) && vals.at(j) != vals.at(j+1)) {
                    fmt = QString("%.%1lf").arg(maxPrecision+i);
                    list = __format(vals,fmt);
                    isFormat = true;
                    break;
                }
            }
            if ( !isFormat ) {
                break;
            }
            ++i;
        }

    } else {
        int minExponent = INT_MAX;
        for ( int i = 0; i < list.size(); ++i ) {
            QString s = list.at(i);
            if ( s.contains("e") ) {
                int j = s.indexOf("e");
                int exponent = s.mid(j+1).toInt();
                if ( exponent < minExponent ) {
                     minExponent = exponent;
                }
            }
        }
        for (int i = 0; i < vals.size(); ++i) {
            double v = vals.at(i);
            if ( v >= 1.0e-9 && v < 1.0 ) {
                QString s = list.at(i);
                QString fmt = QString("%.%1lf").arg(qAbs(minExponent));
                s = s.asprintf(fmt.toLatin1().constData(),v);
                list.replace(i,s);
            }
        }
    }

    return list;
}

QStringList BookTableView::__format(const QList<double> &vals,
                                    const QString& format)
{
    QStringList list;

    foreach ( double v, vals ) {
        QString s;
        s = s.asprintf(format.toLatin1().constData(),v);
        list << s;
    }

    return list;
}

PlotBookModel* BookTableView::_bookModel() const
{
    PlotBookModel* bookModel = dynamic_cast<PlotBookModel*>(model());

    if ( !bookModel ) {
        fprintf(stderr,"koviz [bad scoobs]: BookTableView::_bookModel() "
                       "could not cast model() to a PlotBookModel*.\n");
        exit(-1);
    }

    return bookModel;
}

QStringList BookTableView::_columnLabels() const
{
    // Make list of table column header labels
    QStringList columnLabels;
    QModelIndex tableVarsIdx = _bookModel()->getIndex(rootIndex(),
                                                      "TableVars","Table");
    QModelIndexList tableVarIdxs = _bookModel()->getIndexList(tableVarsIdx,
                                                        "TableVar","TableVars");
    foreach (QModelIndex tableVarIdx, tableVarIdxs) {
        QString varName = _bookModel()->getDataString(tableVarIdx,
                                                     "TableVarName","TableVar");
        QString varLabel = _bookModel()->getDataString(tableVarIdx,
                                                    "TableVarLabel","TableVar");
        QString unit = _bookModel()->getDataString(tableVarIdx,
                                                   "TableVarUnit","TableVar");
        QModelIndex curveIdx = _bookModel()->getDataIndex(tableVarIdx,
                                                     "TableVarData","TableVar");
        QVariant v = _bookModel()->data(curveIdx);
        CurveModel* curveModel = QVariantToPtr<CurveModel>::convert(v);

        if ( unit.isEmpty() ) {
            unit = curveModel->y()->unit();
        }

        if ( varLabel.isEmpty() ) {
            columnLabels << varName  + " {" + unit + "}";
        } else {
            columnLabels << varLabel + " {" + unit + "}";
        }
    }
    return columnLabels;
}

QSize BookTableView::minimumSizeHint() const
{
    QSize s = sizeHint();
    return s;
}

QSize BookTableView::sizeHint() const
{
    QSize s(0,0);
    // TODO
    return s;
}

void BookTableView::dataChanged(const QModelIndex &topLeft,
                                const QModelIndex &bottomRight,
                                const QVector<int> &roles)
{
    Q_UNUSED(roles);

    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return;

    QModelIndex dataIdx = topLeft;
    QModelIndex tagIdx = model()->index(topLeft.row(),0,topLeft.parent());
    QString tag = model()->data(tagIdx).toString();

    if ( tag == "TableVarData") {

        // Get curve model
        QVariant v = _bookModel()->data(dataIdx);
        CurveModel* curveModel =QVariantToPtr<CurveModel>::convert(v);

        if ( curveModel ) {

            double startTime = _bookModel()->getDataDouble(QModelIndex(),
                                                           "StartTime");
            double stopTime = _bookModel()->getDataDouble(QModelIndex(),
                                                          "StopTime");

            curveModel->map();
            ModelIterator* it = curveModel->begin();

            int i = 0;
            while ( !it->isDone() ) {

                double t = it->t();

                if ( t < startTime ) {
                    it->next();
                    continue;
                }
                if ( t > stopTime ) {
                    break;
                }
                if ( i >= _timeStamps.size() ) {
                    _timeStamps.append(t);
                    it->next();
                    ++i;
                    continue;
                }

                double timeStamp = _timeStamps.at(i);
                if ( t == timeStamp ) {
                    it->next();
                } else if ( t < timeStamp ) {
                    _timeStamps.insert(i,t);
                    it->next();
                }
                ++i;
            }

            delete it;
            curveModel->unmap();
        }

        // Based on number of _timeStamps, set vertical scrollbar range
        int max = _timeStamps.count()+1; // +1 for header
        verticalScrollBar()->setRange(0,max);

        // Based on column labels, set horizontal scrollbar range
        int nCols = _columnLabels().size();
        horizontalScrollBar()->setRange(0,nCols);

    } else if ( tag == "LiveCoordTime" ) {

        QModelIndex liveCoordIdx = _bookModel()->getDataIndex(QModelIndex(),
                                                            "LiveCoordTime","");
        if ( !_bookModel()->data(liveCoordIdx).toString().isEmpty() ) {

            double liveTime = _bookModel()->getDataDouble(QModelIndex(),
                                                          "LiveCoordTime");
            int i = TimeStamps::idxAtTime(_timeStamps,liveTime);
            verticalScrollBar()->setValue(i+1);
        }
    }

    viewport()->update();
}

void BookTableView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    if ( start != end ) {
        fprintf(stderr,"koviz [bad scoobs]: TablePageView::rowsInserted(). "
                       "Attempting to insert more than a single table.\n");
        exit(-1);
    }

    int nVars = model()->rowCount(pidx);

    if ( start != nVars-1 ) {
        fprintf(stderr,"koviz [bad scoobs]: TablePageView::rowsInserted(). "
                       "Attempting to insert a row instead of appending it.\n");
        exit(-1);
    }
}

QModelIndex BookTableView::indexAt(const QPoint &point) const
{
    Q_UNUSED(point);
    QModelIndex idx;

    QRect W = viewport()->rect();
    if ( W.width() == 0.0 || W.height() == 0.0 ) {
        return idx; // invalid idx
    }

    // Calculate column width (TODO: this is cut-n-paste code from paintEvent)
    QFontMetrics fm = fontMetrics();
    QStringList labels = _columnLabels();
    int w = fm.width("0123456789");
    foreach ( QString label, labels ) {
        if ( fm.width(label) > w ) {
            w = fm.width(label);
        }
    }
    w = _mLft + w + _mRgt;

    int nCols = W.width()/w;
    if ( nCols > labels.size() ) {
        nCols = labels.size();
    }

    double hsb    = horizontalScrollBar()->value();
    double hsbmax = horizontalScrollBar()->maximum();
    int q = (hsb/hsbmax)*labels.size();
    if ( q > labels.size()-nCols ) {
        q = labels.size()-nCols;
    }

    QModelIndex tableVarsIdx = _bookModel()->getIndex(rootIndex(),
                                                      "TableVars","Table");
    QModelIndexList tableVarIdxs = _bookModel()->getIndexList(tableVarsIdx,
                                                        "TableVar","TableVars");

    int x = point.x();
    int j = x/w + q;

    if ( j < tableVarIdxs.size() ) {
        idx = tableVarIdxs.at(j);
    }

    return idx;
}


QRect BookTableView::visualRect(const QModelIndex &index) const
{
    Q_UNUSED(index);
    QRect rect;
    return rect;
}

void BookTableView::scrollTo(const QModelIndex &index,
                             QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(index);
    Q_UNUSED(hint);
}

QModelIndex BookTableView::moveCursor(
                                 QAbstractItemView::CursorAction cursorAction,
                                 Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(cursorAction);
    Q_UNUSED(modifiers);

    QModelIndex idx;
    return idx;
}


int BookTableView::horizontalOffset() const
{
    return 0;
}

int BookTableView::verticalOffset() const
{
    return 0;
}

bool BookTableView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return false;
}

void BookTableView::setSelection(const QRect &rect,
                                 QItemSelectionModel::SelectionFlags command)
{
    Q_UNUSED(rect);
    Q_UNUSED(command);
}

QRegion BookTableView::visualRegionForSelection(
                                          const QItemSelection &selection) const
{
    Q_UNUSED(selection);
    QRegion region;
    return region;
}

void BookTableView::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_PageUp ) {
        int step = verticalScrollBar()->pageStep();
        int val = verticalScrollBar()->value();
        verticalScrollBar()->setValue(val-step);
    } else if ( event->key() == Qt::Key_PageDown ) {
        int step = verticalScrollBar()->pageStep();
        int val = verticalScrollBar()->value();
        verticalScrollBar()->setValue(val+step);
    } else if ( event->key() == Qt::Key_Up ) {
        int step = verticalScrollBar()->singleStep();
        int val = verticalScrollBar()->value();
        verticalScrollBar()->setValue(val-step);
    } else if ( event->key() == Qt::Key_Down ) {
        int step = verticalScrollBar()->singleStep();
        int val = verticalScrollBar()->value();
        verticalScrollBar()->setValue(val+step);
    } else if ( event->key() == Qt::Key_Right ) {
        int step = horizontalScrollBar()->singleStep();
        int val = horizontalScrollBar()->value();
        horizontalScrollBar()->setValue(val+step);
    } else if ( event->key() == Qt::Key_Left ) {
        int step = horizontalScrollBar()->singleStep();
        int val = horizontalScrollBar()->value();
        horizontalScrollBar()->setValue(val-step);
    }
}

void BookTableView::wheelEvent(QWheelEvent *e)
{
    QModelIndex tableVarIdx = indexAt(e->pos());

    if ( !tableVarIdx.isValid() ) return;

    QString s = _bookModel()->getDataString(tableVarIdx,
                                            "TableVarName","TableVar");
    QString unit = _bookModel()->getDataString(tableVarIdx,
                                               "TableVarUnit","TableVar");
    if ( unit.isEmpty() ) {
        QModelIndex curveIdx = _bookModel()->getDataIndex(tableVarIdx,
                                                     "TableVarData","TableVar");
        QVariant v = _bookModel()->data(curveIdx);
        CurveModel* curveModel = QVariantToPtr<CurveModel>::convert(v);
        unit = curveModel->y()->unit();
    }

    if ( e->delta() > 0 ) {
        unit = Unit::next(unit);
    } else if ( e->delta() < 0 ) {
        unit = Unit::prev(unit);
    }

    QModelIndex tableVarUnitIdx = _bookModel()->getDataIndex(tableVarIdx,
                                                     "TableVarUnit","TableVar");
    model()->setData(tableVarUnitIdx,unit);
    viewport()->update();
}
