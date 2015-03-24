#include "dp.h"

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
    contents = contents.replace(QRegExp(".*PLOTS:"),"PLOTS:");

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

// Made for speed and used when filtering for params in DP
QStringList DPProduct::paramList(const QString &fileName)
{
    QStringList params;

    QFileInfo finfo(fileName);

    if ( finfo.suffix() == "xml" ) {
        // TODO: DPProduct::paramList() unsupported for DP xml files";
        //qDebug() << "DPProduct::paramList() unsupported for DP xml files";
    } else if ( finfo.isFile() && finfo.suffix().isEmpty() &&
                finfo.fileName().startsWith("DP_") ) {

        // Trick 05 DP_Product files

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            _err_stream << "snap [error]: could not open "
                        << file.fileName();
            throw std::runtime_error(_err_string.toAscii().constData());
        }

        QTextStream in(&file);
        QString inString = in.readAll();

        int idx = 0;
        QRegExp rx("[XY]_Variable",Qt::CaseInsensitive);
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

DPPage::DPPage(const QDomElement &e) :
    _startTime(-DBL_MAX),
    _stopTime(DBL_MAX)
{
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

DPPage::DPPage(const char *title) : _title(title)
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

void DPPage::setStartTime(double startTime)
{
    _startTime = startTime;
}

void DPPage::setStopTime(double stopTime)
{
    _stopTime = stopTime;
}

DPPlot::DPPlot(const QDomElement &e) :
    _xMinRange(-DBL_MAX),
    _xMaxRange(DBL_MAX),
    _yMinRange(-DBL_MAX),
    _yMaxRange(DBL_MAX),
    _startTime(-DBL_MAX),
    _stopTime(DBL_MAX)
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
    _stopTime(DBL_MAX)
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

DPCurve::DPCurve(const QDomElement &e) : _t(0),_x(0),_y(0)
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
                if ( count > 1 ) {
                    _err_stream << "snap [error]: DPPlot can't handle "
                                << "multiple y vars found in "
                                << e.ownerDocument().toString() << "\n";
                    throw std::runtime_error(_err_string.toAscii().constData());
                }
                count++;
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

DPVar::DPVar(const QDomElement &e) :
    _name(QString()),
    _label(QString()),
    _unit(QString())
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
}

DPVar::DPVar(const char *name) :
    _name(name),
    _label(QString()),
    _unit(QString())
{
}

