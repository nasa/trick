#include "dp.h"

DPProduct::DPProduct(const QString &xmlfile) :
    _err_stream(&_err_string)
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

DPProduct::~DPProduct()
{
    delete _doc;
}


DPPage::DPPage(const QDomElement &e)
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
            }
        }
        n = n.nextSibling();
    }
}


DPPlot::DPPlot(const QDomElement &e)
{
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
            }
        }
        n = n.nextSibling();
    }
}


DPCurve::DPCurve(const QDomElement &e) :
    _err_stream(&_err_string)
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

