#ifndef DPPRODUCT_H
#define DPPRODUCT_H

#include <QDomDocument>
#include <QList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <stdexcept>

class DPVar
{
public:
    DPVar(const QDomElement& e);
    QString name() { return _name; }
    QString label() { return _label; }
    QString unit() { return _unit; }
private:
    QString _name;
    QString _label;
    QString _unit;
};

class DPCurve
{
public:
    DPCurve(const QDomElement& e);
    DPVar* t() { return _t; }
    DPVar* x() { return _x; }
    DPVar* y() { return _y; }
private:
    DPVar* _t;
    DPVar* _x;
    DPVar* _y;
    QString _err_string;
    QTextStream _err_stream;
};

class DPPlot
{
public:
    DPPlot(const QDomElement& e);
    QString title() { return _title ; }
    QList<DPCurve*> curves() { return _curves; }
private:
    QString _title;
    QList<DPCurve*> _curves;
};

class DPPage
{
public:
    DPPage(const QDomElement& e);
    QString title() const { return _title ; }
    QList<DPPlot*> plots() { return _plots; }
private:
    QString _title;
    QList<DPPlot*> _plots;
};

class DPProduct
{
public:
    DPProduct(const QString& xmlfile);
    ~DPProduct();
    QString title() { return _title; }
    double start();
    double stop();
    double freq();
    QList<DPPage*> pages() { return _pages; }
private:
    QDomDocument* _doc;
    QString _title;
    QString _err_string;
    QTextStream _err_stream;
    QList<DPPage*> _pages;
};

#endif // DPPLOT_H
