#include "dp.h"
#include "libopts/options.h"

DPProduct* product;// hack to pass to yacc parser

QString DPProduct::_err_string;
QTextStream DPProduct::_err_stream(&DPProduct::_err_string);

DPProduct::DPProduct(const QString &fileName) :
    _fileName(fileName),
    _doc(0)
{
    QFileInfo finfo(fileName);

    if ( finfo.suffix() == "xml" ) {
        _handleDPXMLFile(fileName);
    } else {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            _err_stream << "snap [error]: could not open "
                        << file.fileName();
            throw std::runtime_error(_err_string.toAscii().constData());
        }
        QTextStream in(&file);
        QString inString = in.readAll();
        file.close();

        if ( inString.contains("xml version") ) {
            // DP file without .xml suffix
            _handleDPXMLFile(fileName);
        } else {
            _handleDP05File(inString);
        }
    }

}

DPProduct::~DPProduct()
{
    if ( _doc ) delete _doc;
    product = 0 ;
}

QString DPProduct::title()
{
    return _title;
}

void DPProduct::_handleDP05File(QString &contents)
{
    // Remove all characters up to PLOTS: key word
    int i = contents.indexOf("PLOTS:",0,Qt::CaseInsensitive);
    contents = contents.remove(0,i-1);

    product = this; // TODO: product is global, need to fix the hack

    YY_BUFFER_STATE bufferState = yy_scan_string(contents.toAscii().constData());
    Q_UNUSED(bufferState);
    yyparse();
}

void DPProduct::_handleDPXMLFile(const QString &xmlfile)
{
    _doc = new QDomDocument(xmlfile);
    QFile file(xmlfile);
    if (!file.open(QIODevice::ReadOnly)) {
        _err_stream << "snap [error]: could not open "
                    << xmlfile << "\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }
    if (!_doc->setContent(&file)) {
        file.close();
        _err_stream << "snap [error]: could not parse "
                    << xmlfile << "\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }
    file.close();

    QDomElement pel = _doc->documentElement();

    QDomNode n = pel.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString tag = e.tagName();
            if ( tag == "title" ) {
                _title = e.text();
            } else if ( tag == "page" ) {
                DPPage* page = new DPPage(e);
                _pages.append(page);
            }
        }
        n = n.nextSibling();
    }
}


DPPage *DPProduct::addPage(const char *title)
{
    DPPage* page = new DPPage(title);
    _pages.append(page);
    return page;
}

DPTable *DPProduct::addTable(const char *title)
{
    DPTable* table = new DPTable(title);
    _tables.append(table);
    return table;
}

// Used when filtering for params in DP
QStringList DPProduct::paramList(const QString &fileName)
{
    QStringList params;

    QFileInfo finfo(fileName);

    if ( finfo.suffix() == "xml" ) {

        DPProduct dp(fileName);
        QHash<QString,int> paramHash;
        foreach (DPPage* page, dp.pages() ) {
            foreach ( DPPlot* plot, page->plots() ) {
                foreach ( DPCurve* curve, plot->curves() ) {
                    paramHash.insert(curve->t()->name(),0);
                    paramHash.insert(curve->x()->name(),0);
                    paramHash.insert(curve->y()->name(),0);
                }
            }
        }

        params = paramHash.keys();

    } else if ( finfo.isFile() && finfo.suffix().isEmpty() &&
                finfo.fileName().startsWith("DP_") ) {

        // Trick 05/07 DP_Product files

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            _err_stream << "snap [error]: could not open "
                        << file.fileName();
            throw std::runtime_error(_err_string.toAscii().constData());
        }

        QTextStream in(&file);
        QString inString = in.readAll();

        int idx = 0;
        QRegExp rx("([XY]_Variable)|(Variable)\\s*:",Qt::CaseInsensitive);
        while ( 1 ) {
            idx = inString.indexOf(rx,idx);
            if ( idx < 0 ) break;
            int idxStart = inString.indexOf('\"',idx);
            idx = idxStart+1;
            int idxEnd   = inString.indexOf('\"',idx);
            idx = idxEnd;
            params.append(inString.mid(idxStart+1,idxEnd-idxStart-1));
        }

        file.close();
    }

    return params;
}

QStringList DPProduct::paramList(const QStringList &dpFileNames)
{
    QStringList params;

    QHash<QString,int> paramHash;
    foreach ( QString dpFileName, dpFileNames ) {
        foreach ( QString param, DPProduct::paramList(dpFileName) ) {
            paramHash.insert(param,0);
        }
    }

    params = paramHash.keys();
    params.sort();

    int timeIdx = params.indexOf("sys.exec.out.time");
    if ( timeIdx > 0 ) {
        params.move(timeIdx,0);
    } else {
        timeIdx = params.indexOf("time");
        if ( timeIdx > 0 ) {
            params.move(timeIdx,0);
        } else {
            QRegExp rx("*time$") ;
            rx.setCaseSensitivity(Qt::CaseInsensitive);
            timeIdx = params.indexOf(rx);
            if ( timeIdx > 0 ) {
                params.move(timeIdx,0);
            } else {
                // Last resort, add sys.exec.out.time
                params.insert(0,"sys.exec.out.time");
            }
        }
    }

    return params;
}

DPPage::DPPage(const QDomElement &e) :
    _startTime(-DBL_MAX),
    _stopTime(DBL_MAX),
    _backgroundColor("#FFFFFF"),
    _foregroundColor("#000000")
{
    QDomElement el = e;

    if ( el.hasAttribute("background_color") ) {
        _backgroundColor = el.attributeNode("background_color")
                             .value().simplified();
    }
    if ( el.hasAttribute("foreground_color") ) {
        _foregroundColor = el.attributeNode("foreground_color")
                             .value().simplified();
    }

    QDomNode n = e.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString tag = e.tagName();
            if ( tag == "title" ) {
                _title = e.text();
            } else if ( tag == "plot" ) {
                DPPlot* plot = new DPPlot(e);
                _plots.append(plot);
            } else if ( tag == "tstart" ) {
                _startTime = e.text().toDouble();
            } else if ( tag == "tstop" ) {
                _stopTime = e.text().toDouble();
            }
        }
        n = n.nextSibling();
    }
}

DPPage::DPPage(const char *title) :
    _title(title),
    _startTime(-DBL_MAX),
    _stopTime(DBL_MAX),
    _backgroundColor("#FFFFFF"),
    _foregroundColor("#000000")
{
}

DPPlot *DPPage::addPlot(const char *title)
{
    DPPlot* plot = new DPPlot(title);
    _plots.append(plot);
    return plot;
}

double DPPage::startTime()
{
    return _startTime;
}

double DPPage::stopTime()
{
    return _stopTime;
}

QString DPPage::backgroundColor()
{
    return _backgroundColor;
}

QString DPPage::foregroundColor()
{
    return _foregroundColor;
}

void DPPage::setStartTime(double startTime)
{
    _startTime = startTime;
}

void DPPage::setStopTime(double stopTime)
{
    _stopTime = stopTime;
}

void DPPage::setBackgroundColor(const QString &color)
{
    _backgroundColor = color;
}

void DPPage::setForegroundColor(const QString &color)
{
    _foregroundColor = color;
}

DPPlot::DPPlot(const QDomElement &e) :
    _xMinRange(-DBL_MAX),
    _xMaxRange(DBL_MAX),
    _yMinRange(-DBL_MAX),
    _yMaxRange(DBL_MAX),
    _startTime(-DBL_MAX),
    _stopTime(DBL_MAX),
    _isGrid(true),
    _gridColor("#E1E1E1"),
    _backgroundColor("#FFFFFF"),
    _foregroundColor(""),
    _font("")
{
    QDomElement el = e;

    if ( el.hasAttribute("xmin") ) {
        _xMinRange = el.attributeNode("xmin").value().simplified().toDouble();
    }
    if ( el.hasAttribute("xmax") ) {
        _xMaxRange = el.attributeNode("xmax").value().simplified().toDouble();
    }
    if ( el.hasAttribute("ymin") ) {
        _yMinRange = el.attributeNode("ymin").value().simplified().toDouble();
    }
    if ( el.hasAttribute("ymax") ) {
        _yMaxRange = el.attributeNode("ymax").value().simplified().toDouble();
    }
    if ( el.hasAttribute("grid") ) {
        QString isGridStr = el.attributeNode("grid").value().simplified();
        bool ok;
        _isGrid = Options::stringToBool(isGridStr, &ok);
    }
    if ( el.hasAttribute("grid_color") ) {
        _gridColor = el.attributeNode("grid_color").value().simplified();
    }
    if ( el.hasAttribute("background_color") ) {
        _backgroundColor = el.attributeNode("background_color")
                             .value().simplified();
    }
    if ( el.hasAttribute("foreground_color") ) {
        _foregroundColor = el.attributeNode("foreground_color")
                             .value().simplified();
    }
    if ( el.hasAttribute("font") ) {
        _font = el.attributeNode("font").value().simplified();
    }

    QDomNode n = e.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString tag = e.tagName();
            if ( tag == "title" ) {
                _title = e.text();
            } else if ( tag == "curve" ) {
                DPCurve* curve = new DPCurve(e);
                _curves.append(curve);
            } else if ( tag == "xaxis" ) {
                QDomElement labelElement = e.firstChildElement("label");
                if ( !labelElement.isNull() ) {
                    _xAxisLabel = labelElement.text();
                }
            } else if ( tag == "yaxis" ) {
                QDomElement labelElement = e.firstChildElement("label");
                if ( !labelElement.isNull() ) {
                    _yAxisLabel = labelElement.text();
                }
            } else if ( tag == "tstart" ) {
                _startTime = e.text().toDouble();
            } else if ( tag == "tstop" ) {
                _stopTime = e.text().toDouble();
            }
        }
        n = n.nextSibling();
    }
}

DPPlot::DPPlot(const char *title) :
    _title(title),
    _xMinRange(-DBL_MAX),
    _xMaxRange(DBL_MAX),
    _yMinRange(-DBL_MAX),
    _yMaxRange(DBL_MAX),
    _startTime(-DBL_MAX),
    _stopTime(DBL_MAX),
    _isGrid(true),
    _gridColor("#E1E1E1"),
    _backgroundColor("#FFFFFF"),
    _foregroundColor(""),
    _font("")
{
}

QString DPPlot::xAxisLabel()
{
    QString label = _xAxisLabel;

    if ( label.isEmpty() ) {
        // create label from curve0
        if ( _curves.size() > 0 ) {
            DPCurve* curve0 = _curves.at(0);
            label = _abbreviate(curve0->x()->name());
        }
    }

    return label;
}

QString DPPlot::yAxisLabel()
{
    QString label = _yAxisLabel;

    if ( label.isEmpty() ) {
        // create label from curve0
        if ( _curves.size() > 0 ) {
            DPCurve* curve0 = _curves.at(0);
            label = _abbreviate(curve0->y()->name());
        }
    }

    return label;
}

double DPPlot::xMinRange()
{
    return _xMinRange;
}

double DPPlot::xMaxRange()
{
    return _xMaxRange;
}

double DPPlot::yMinRange()
{
    return _yMinRange;
}

double DPPlot::yMaxRange()
{
    return _yMaxRange;
}

double DPPlot::startTime()
{
    return _startTime;
}

double DPPlot::stopTime()
{
    return _stopTime;
}

bool DPPlot::grid()
{
    return _isGrid;
}

QString DPPlot::gridColor()
{
    return _gridColor;
}

QString DPPlot::backgroundColor()
{
    return _backgroundColor;
}

QString DPPlot::foregroundColor()
{
    return _foregroundColor;
}

QString DPPlot::font()
{
    return _font;
}

void DPPlot::setXMinRange(double xMin)
{
    _xMinRange = xMin;
}

void DPPlot::setXMaxRange(double xMax)
{
    _xMaxRange = xMax;
}

void DPPlot::setYMinRange(double yMin)
{
    _yMinRange = yMin;
}

void DPPlot::setYMaxRange(double yMax)
{
    _yMaxRange = yMax;
}

void DPPlot::setStartTime(double startTime)
{
    _startTime = startTime;
}

void DPPlot::setStopTime(double stopTime)
{
    _stopTime = stopTime;
}

void DPPlot::setGrid(const QString &isGridString)
{
    bool ok = false;
    if ( Options::stringToBool(isGridString,&ok) ) {
        _isGrid = true;
    } else {
        _isGrid = false;
    }
}

void DPPlot::setGridColor(const QString &color)
{
    _gridColor = color;
}

void DPPlot::setBackgroundColor(const QString &color)
{
    _backgroundColor = color;
}

void DPPlot::setForegroundColor(const QString &color)
{
    _foregroundColor = color;
}

void DPPlot::setFont(const QString &fnt)
{
    _font = fnt;
}

DPCurve *DPPlot::addCurve()
{
    DPCurve* curve = new DPCurve();
    _curves.append(curve);
    return curve;
}


QString DPPlot::_abbreviate(const QString &label, int maxlen)
{
    if ( label == "sys.exec.out.time" ) {
        return "Time";
    }

    QString abbr = label.right(maxlen);
    int idx = label.size()-1;
    while ( idx > 0 ) {
        idx = label.lastIndexOf('.',idx);
        QString str = label.mid(idx+1) ;
        idx--;
        if ( str.size() > maxlen ) {
            break;
        }
        abbr = str;
    }

    return abbr;
}

QString DPCurve::_err_string;
QTextStream DPCurve::_err_stream(&DPCurve::_err_string);

DPCurve::DPCurve(const QDomElement &e) : _t(0), _x(0), _y(0)
{
    int count = 0;
    QDomNode n = e.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString tag = e.tagName();
            if ( tag == "var" ) {
                DPVar* var = new DPVar(e);
                if ( count == 0 ) {
                    _t = var; // hack for now since DP xml has no t,x,y
                    _x = var;
                } else {
                    _y = var;
                }
                setLineColor(var->lineColor().toAscii().constData());
                if ( !var->lineStyle().isEmpty() ) {
                    setLineStyle(var->lineStyle().toAscii().constData());
                }
                if ( count > 1 ) {
                    _err_stream << "snap [error]: DPPlot can't handle "
                                << "multiple y vars found in "
                                << e.ownerDocument().toString() << "\n";
                    throw std::runtime_error(_err_string.toAscii().constData());
                }
                count++;
            } else if ( tag == "varcase" ) {
                DPXYPair* xypair = new DPXYPair(e);
                _xyPairs.append(xypair);
            }
        }
        n = n.nextSibling();
    }
}

DPVar *DPCurve::setXVarName(const char *name)
{
    if ( _x ) delete _x;
    _x = new DPVar(name);
    return _x;
}

DPVar *DPCurve::setYVarName(const char *name)
{
    if ( _y ) delete _y;
    _y = new DPVar(name);
    return _y;
}

QList<DPXYPair *> DPCurve::xyPairs()
{
    return _xyPairs;
}

void DPCurve::addXYPair(DPVar *x, DPVar *y)
{
    DPXYPair* xyPair = new DPXYPair(x,y);
    _xyPairs.append(xyPair);
}


DPVar* DPCurve::t() {
    if ( !_t ) {
        _t = new DPVar("sys.exec.out.time");
        _t->setLabel("time");
        _t->setUnit("s");
    }
    return _t;
}

DPVar *DPCurve::x()
{
    if ( !_x ) {
        _x = new DPVar("x");
        _x->setLabel("X Label Unset");
        _x->setUnit("--");
    }
    return _x;
}

DPVar *DPCurve::y()
{
    if ( !_y ) {
        _y = new DPVar("y");
        _y->setLabel("Y Label Unset");
        _y->setUnit("--");
    }
    return _y;
}

void DPCurve::setLineColor(const char *color)
{
    _color = QString(color);
}

QString DPCurve::lineStyle()
{
    return _y->lineStyle();
}

void DPCurve::setLineStyle(const char *lineStyle)
{
    _y->setLineStyle(lineStyle);
}

QString DPCurve::symbolStyle()
{
    return _y->symbolStyle();
}

void DPCurve::setSymbolStyle(const char *style)
{
    _y->setSymbolStyle(style);
}

QString DPCurve::symbolSize()
{
    return _y->symbolSize();
}

void DPCurve::setSymbolSize(const char *size)
{
    _y->setSymbolSize(size);
}

QString DPCurve::lineColor()
{
    return _color;
}

DPVar::DPVar(const QDomElement &e) :
    _name(QString()),
    _label(QString()),
    _unit(QString()),
    _lineColor(QString()),
    _lineStyle(QString()),
    _scaleFactor(1.0),
    _bias(0.0),
    _symbol(QString()),
    _symbolSize(QString()),
    _timeName(QString())
{
    QDomElement el = e;

    _name = el.text().simplified();

    QString label("label");
    if ( el.hasAttribute(label) ) {
        _label = el.attributeNode(label).value().simplified();
    } else {
        _label = _name;
    }

    QString unit("units");
    if ( el.hasAttribute(unit) ) {
        _unit = el.attributeNode(unit).value().simplified();
    }

    QString lineColor("line_color");
    if ( el.hasAttribute(lineColor) ) {
        _lineColor = el.attributeNode(lineColor).value().simplified();
    }

    QString lineStyle("line_style");
    if ( el.hasAttribute(lineStyle) ) {
        _lineStyle = el.attributeNode(lineStyle).value().simplified();
    }

    QString scale("scale");
    if ( el.hasAttribute(scale) ) {
        _scaleFactor = el.attributeNode(scale).value().simplified().toDouble();
    }

    QString bias("bias");
    if ( el.hasAttribute(bias) ) {
        _bias = el.attributeNode(bias).value().simplified().toDouble();
    }

    QString symbol("symbol_style");
    if ( el.hasAttribute(symbol) ) {
        _symbol = el.attributeNode(symbol).value().simplified();
    }

    QString symbolSize("symbol_size");
    if ( el.hasAttribute(symbolSize) ) {
        _symbolSize = el.attributeNode(symbolSize).value().simplified();
    }

    QString timeName("time_name");
    if ( el.hasAttribute(timeName) ) {
        _timeName = el.attributeNode(timeName).value().simplified();
    }
}

DPVar::DPVar(const char *name) :
    _name(name),
    _label(QString()),
    _unit(QString()),
    _lineColor(QString()),
    _lineStyle(QString()),
    _scaleFactor(1.0),
    _bias(0.0),
    _symbol(QString()),
    _symbolSize(QString()),
    _minRange(-DBL_MAX),
    _maxRange(DBL_MAX)
{
}


DPXYPair::DPXYPair(const QDomElement &e) :
    _x(0), _y(0)
{
    QDomNode n = e.firstChild();
    int i = 0;
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString tag = e.tagName();
            if ( tag == "var" ) {
                if ( i == 0 ) {
                    // X var
                    _x = new DPVar(e);
                } else if ( i == 1 ) {
                    // Y var
                    _y = new DPVar(e);
                }
            }
        }
        n = n.nextSibling();
        ++i;
    }
}

DPXYPair::DPXYPair(DPVar *x, DPVar *y) :
    _x(0), _y(0)
{
    _x = x;
    _y = y;
}


DPTable::DPTable(const char *title) :
    _title(title),
     _delimiter(","),
    _startTime(-DBL_MAX),
    _stopTime(DBL_MAX),
    _vars(QList<DPVar*>())
{
}


DPVar *DPTable::addVar(const char *title)
{
    DPVar* var = new DPVar(title);
    _vars.append(var);
    return var;
}
