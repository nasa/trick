#include "dp.h"
#include "options.h"

DPProduct* product;// hack to pass to yacc parser

QString DPProduct::_err_string;
QTextStream DPProduct::_err_stream(&DPProduct::_err_string);

DPProduct::DPProduct(const QString &fileName) :
    _fileName(fileName),
    _doc(0),
    _program(new DPProgram)
{
    QFileInfo finfo(fileName);

    if ( finfo.suffix() == "xml" ) {
        _handleDPXMLFile(fileName);
    } else {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            _err_stream << "koviz [error]: could not open "
                        << file.fileName();
            throw std::runtime_error(_err_string.toLatin1().constData());
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
    delete _doc;

    product = 0 ;
    foreach ( DPPage* page, _pages ) {
        delete page;
    }
    _pages.clear();

    foreach ( DPTable* table, _tables ) {
        if ( table ) {
            delete table;
        }
    }
    _tables.clear();

    delete _program;
}

QString DPProduct::title()
{
    return _title;
}

void DPProduct::_handleDP05File(QString &contents)
{
    // Remove all characters up to PLOTS: or TABLES: key word
    QStringList keywords;
    keywords << "PLOTS:" << "TABLES:" << "PROGRAM:";
    int i = INT_MAX;
    foreach (QString keyword, keywords) {
        int j = contents.indexOf(keyword,0,Qt::CaseInsensitive);
        if ( j >= 0 && j < i ) {
            i = j;
        }
    }
    if ( i == INT_MAX ) {
        fprintf(stderr,"koviz [error]: no PLOTS: or TABLES: "
                       "or PROGRAM: keywords in %s\n",
                _fileName.toLatin1().constData());
        exit(-1);
    }
    contents = contents.remove(0,i-1);

    product = this; // TODO: product is global, need to fix the hack

    YY_BUFFER_STATE state = yy_scan_string(contents.toLatin1().constData());
    yyparse();
    yy_delete_buffer(state);
}

void DPProduct::_handleDPXMLFile(const QString &xmlfile)
{
    _doc = new QDomDocument(xmlfile);
    QFile file(xmlfile);
    if (!file.open(QIODevice::ReadOnly)) {
        _err_stream << "koviz [error]: could not open "
                    << xmlfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
    }
    if (!_doc->setContent(&file)) {
        file.close();
        _err_stream << "koviz [error]: could not parse "
                    << xmlfile << "\n";
        throw std::runtime_error(_err_string.toLatin1().constData());
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

    DPProduct dp(fileName);
    foreach (DPPage* page, dp.pages() ) {
        foreach ( DPPlot* plot, page->plots() ) {
            foreach ( DPCurve* curve, plot->curves() ) {
                QString x = curve->x()->name();
                QString y = curve->y()->name();
                if ( !params.contains(x) && !x.isEmpty() ) {
                    params.append(x);
                }
                if ( !params.contains(y) && !y.isEmpty() ) {
                    params.append(y);
                }
            }
        }
    }

    return params;
}

QStringList DPProduct::paramList(const QStringList &dpFileNames,
                                 const QString& timeName)
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

    if ( !params.isEmpty() ) {
        int timeIdx = params.indexOf(timeName);
        if ( timeIdx > 0 ) {
            params.move(timeIdx,0);
        } else {
            params.insert(0,timeName);
        }
    }

    return params;
}

QStringList DPProduct::tableParamList(const QString &fileName)
{
    QStringList params;

    DPProduct dp(fileName);
    foreach (DPTable* table, dp.tables() ) {
        foreach ( DPVar* var, table->vars() ) {
            params << var->name();
        }
    }

    return params;
}

QStringList DPProduct::tableParamList(const QStringList &dpFileNames,
                                      const QString& timeName)
{
    QStringList params;

    QHash<QString,int> paramHash;
    foreach ( QString dpFileName, dpFileNames ) {
        foreach ( QString param, DPProduct::tableParamList(dpFileName) ) {
            paramHash.insert(param,0);
        }
    }

    params = paramHash.keys();
    params.sort();

    if ( !params.isEmpty() ) {
        int timeIdx = params.indexOf(timeName);
        if ( timeIdx > 0 ) {
            params.move(timeIdx,0);
        } else if ( timeIdx < 0 ) {
            params.insert(0,timeName);
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

DPPage::~DPPage()
{
    foreach ( DPPlot* plot, _plots ) {
        delete plot;
    }
    _plots.clear();
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
    _foregroundColor("#000000"),
    _font(""),
    _plotXScale("linear"),
    _plotYScale("linear"),
    _presentation("")
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
    _foregroundColor("#000000"),
    _font(""),
    _plotXScale("linear"),
    _plotYScale("linear"),
    _presentation("")
{
}

DPPlot::~DPPlot()
{
    foreach ( DPCurve* curve, _curves ) {
        delete curve;
    }
    _curves.clear();
    foreach ( DPHLine* hline, _hlines ) {
        delete hline;
    }
    _hlines.clear();
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

QString DPPlot::plotXScale() const
{
    return _plotXScale;
}

QString DPPlot::plotYScale() const
{
    return _plotYScale;
}

QString DPPlot::plotRatio() const
{
    return _plotRatio;
}

QList<double> DPPlot::majorXTics() const
{
    return _majorXTics;
}

QList<double> DPPlot::majorYTics() const
{
    return _majorYTics;
}

QList<double> DPPlot::minorXTics() const
{
    return _minorXTics;
}

QList<double> DPPlot::minorYTics() const
{
    return _minorYTics;
}

QRectF DPPlot::rect() const
{
    return _rect;
}

QString DPPlot::presentation() const
{
    return _presentation;
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

void DPPlot::setPlotXScale(const QString &plotXScale)
{
    _plotXScale = plotXScale;
}

void DPPlot::setPlotYScale(const QString &plotYScale)
{
    _plotYScale = plotYScale;
}

void DPPlot::setPlotRatio(const QString &plotRatio)
{
    _plotRatio = plotRatio;
}

void DPPlot::setMajorXTics(const QList<double> &xTics)
{
    foreach ( double xtic, xTics ) {
        _majorXTics << xtic;
    }
}

void DPPlot::setMajorYTics(const QList<double>& yTics)
{
    foreach ( double ytic, yTics ) {
        _majorYTics << ytic;
    }
}

void DPPlot::setMinorXTics(const QList<double> &xTics)
{
    foreach ( double xtic, xTics ) {
        _minorXTics << xtic;
    }
}

void DPPlot::setMinorYTics(const QList<double> &yTics)
{
    foreach ( double ytic, yTics ) {
        _minorYTics << ytic;
    }
}

void DPPlot::setRect(double x, double y, double w, double h)
{
    _rect = QRectF(x,y,w,h);
}

void DPPlot::setPresentation(const QString &presentation)
{
    _presentation = presentation;
}

DPCurve *DPPlot::addCurve()
{
    DPCurve* curve = new DPCurve();
    _curves.append(curve);
    return curve;
}

DPHLine *DPPlot::addHLine(double val)
{
    DPHLine* hline = new DPHLine(val);
    _hlines.append(hline);
    return hline;
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

DPCurve::DPCurve(const QDomElement &e) : _x(0), _y(0)
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
                    _x = var;
                } else {
                    _y = var;
                }
                setLineColor(var->lineColor().toLatin1().constData());
                if ( !var->lineStyle().isEmpty() ) {
                    setLineStyle(var->lineStyle().toLatin1().constData());
                }
                if ( count > 1 ) {
                    _err_stream << "koviz [error]: DPPlot can't handle "
                                << "multiple y vars found in "
                                << e.ownerDocument().toString() << "\n";
                    throw std::runtime_error(_err_string.toLatin1().constData());
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

DPCurve::~DPCurve()
{
    if ( _x ) {
        if ( _x == _y ) {
            _y = 0;
        }
        delete _x;
        _x = 0;
    }

    if ( _y ) {
        delete _y;
        _y = 0;
    }
}

DPVar *DPCurve::setXVarName(const char *name)
{
    delete _x;
    _x = new DPVar(name);
    return _x;
}

DPVar *DPCurve::setYVarName(const char *name)
{
    delete _y;
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
    _timeName(QString()),
    _minRange(-DBL_MAX),
    _maxRange(DBL_MAX)
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

    QString min("min");
    if ( el.hasAttribute(min) ) {
        _minRange = el.attributeNode(min).value().simplified().toDouble();
    }

    QString max("max");
    if ( el.hasAttribute(max) ) {
        _maxRange = el.attributeNode(max).value().simplified().toDouble();
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

DPTable::~DPTable()
{
    foreach ( DPVar* var, _vars ) {
        delete var;
    }
    _vars.clear();
}

DPVar *DPTable::addVar(const char *title)
{
    DPVar* var = new DPVar(title);
    _vars.append(var);
    return var;
}

DPProgram::~DPProgram()
{
}

QList<Parameter> DPProgram::inputParams() const
{
    return _inputParams;
}

QList<Parameter> DPProgram::outputParams() const
{
    return _outputParams;
}

void DPProgram::addInput(const QString &input)
{
    _inputs << input;

    Parameter inputParam(input);
    _inputParams << inputParam;
}

void DPProgram::addOutput(const QString &output)
{
    _outputs << output;

    Parameter outputParam(output);
    _outputParams << outputParam;
}

void DPHLine::setValue(double val)
{
    _val = val;
}

void DPHLine::setColor(const QString &color)
{
    _color = color;
}

void DPHLine::setLabel(const QString &label)
{
    _label = label;
}

void DPHLine::setUnit(const QString &unit)
{
    _unit = unit;
}

void DPHLine::setLabelUnit(const QString &labelUnit)
{
    _labelUnit = labelUnit;
}

// 0 is left, 0.5 is center and 1.0 is right
void DPHLine::setLabelPlacement(double placement)
{
    if ( placement < 0 ) {
        // Clamp placement to 0
        _labelPlacement = 0;
    } else if ( placement > 1 ) {
        // Clamp placement to 1
        _labelPlacement = 1;
    } else {
        _labelPlacement = placement;
    }
}

void DPHLine::setLabelOrientation(const QString &orientation)
{
    if ( orientation == "left" ) {
        _labelPlacement = 0;
    } else if ( orientation == "center" ) {
        _labelPlacement = 0.5;
    } else if ( orientation == "right" ) {
        _labelPlacement = 1.0;
    } else {
        // If unrecognized format, issue error and set orient to left
        fprintf(stderr, "koviz [error]: DPHLine::setLabelOrientation: "
                "unrecognized orientation=%s. Try left,center,right.\n",
                orientation.toLatin1().constData());
        _labelPlacement = 0;
    }
}
