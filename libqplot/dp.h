#ifndef DPPRODUCT_H
#define DPPRODUCT_H

#include <QDomDocument>
#include <QList>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QDebug>
#include <stdexcept>

#include "product_lexer.h"
#include "product_parser.h"
extern int yyparse(void);

class DPVar
{
public:
    DPVar(const QDomElement& e);
    DPVar(const char* name);
    QString name() { return _name; }
    QString label() { return _label; }
    QString unit() { return _unit; }

    void setLabel(const char* label) { _label = label; }
    void setUnit(const char* unit) { _unit = unit; }
private:
    QString _name;
    QString _label;
    QString _unit;
};

class DPCurve
{
public:
    DPCurve() : _t(0),_x(0),_y(0) {}

    DPCurve(const QDomElement& e);
    DPVar* t();
    DPVar* x();
    DPVar* y();

    DPVar* setXVarName(const char* name);
    DPVar* setYVarName(const char* name);
private:
    DPVar* _t;
    DPVar* _x;
    DPVar* _y;
    static QString _err_string;
    static QTextStream _err_stream;
};

class DPPlot
{
public:
    DPPlot(const QDomElement& e);
    DPPlot(const char* title);
    QString title() { return _title ; }
    QString xAxisLabel();
    QString yAxisLabel();
    QList<DPCurve*> curves() { return _curves; }

    DPCurve* addCurve();
    void setXAxisLabel(const char* label) { _xAxisLabel = label ; }
    void setYAxisLabel(const char* label) { _yAxisLabel = label ; }

private:
    QString _title;
    QString _xAxisLabel;
    QString _yAxisLabel;
    QList<DPCurve*> _curves;
    static QString _abbreviate(const QString& label,int maxlen=35);
};

class DPPage
{
public:
    DPPage(const QDomElement& e);
    DPPage(const char* title);
    QString title() const { return _title ; }
    QList<DPPlot*> plots() { return _plots; }
    DPPlot* addPlot(const char* title);
private:
    QString _title;
    QList<DPPlot*> _plots;
};

class DPProduct
{
public:
    DPProduct(const QString& fileName);
    ~DPProduct();

    QString dpFileName() { return _fileName; }
    QString title();
    double start();
    double stop();
    double freq();
    QList<DPPage*> pages() const { return _pages; }
    DPPage* addPage(const char* title);

    // Made for speed and used when filtering for params in DP
    static QStringList paramList(const QString& fileName);

private:
    QString _fileName;
    QDomDocument* _doc;
    QString _title;
    QList<DPPage*> _pages;
    static QString _err_string;
    static QTextStream _err_stream;

    void _handleDPXMLFile(const QString &fileName);
    void  _handleDP05File(QString &contents);
};

#endif // DPPLOT_H
