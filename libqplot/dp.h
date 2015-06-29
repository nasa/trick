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
#include <QHash>
#include <QDebug>
#include <stdexcept>
#include <float.h>

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
    QString lineColor() { return _lineColor; }
    double scaleFactor() { return _scaleFactor; }
    double bias() { return _bias; }

    void setLabel(const char* label) { _label = label; }
    void setUnit(const char* unit) { _unit = unit; }
    void setLineColor(const char* lineColor) { _lineColor = lineColor; }
    void setScaleFactor(double sf) { _scaleFactor = sf; }
    void setBias(double b) { _bias = b; }

private:
    QString _name;
    QString _label;
    QString _unit;
    QString _lineColor;
    double _scaleFactor;
    double _bias;
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

    QString lineColor();
    void setLineColor(const char* lineColor);

private:
    DPVar* _t;
    DPVar* _x;
    DPVar* _y;
    QString _color;
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
    double xMinRange();
    double xMaxRange();
    double yMinRange();
    double yMaxRange();
    double startTime();
    double stopTime();
    bool grid();
    QString gridColor();
    QString backgroundColor();
    QString foregroundColor();
    QString font();

    void setXAxisLabel(const char* label) { _xAxisLabel = label ; }
    void setYAxisLabel(const char* label) { _yAxisLabel = label ; }
    void setXMinRange(double xMin);
    void setXMaxRange(double xMax);
    void setYMinRange(double yMin);
    void setYMaxRange(double yMax);
    void setStartTime(double startTime);
    void setStopTime(double stopTime);
    void setGrid(const QString& isGridString);
    void setGridColor(const QString& color);
    void setBackgroundColor(const QString& color);
    void setForegroundColor(const QString& color);
    void setFont(const QString& fnt);

    QList<DPCurve*> curves() { return _curves; }
    DPCurve* addCurve();

private:
    QString _title;
    QString _xAxisLabel;
    QString _yAxisLabel;
    double _xMinRange;
    double _xMaxRange;
    double _yMinRange;
    double _yMaxRange;
    double _startTime;
    double _stopTime;
    bool _isGrid;
    QString _gridColor;
    QList<DPCurve*> _curves;
    static QString _abbreviate(const QString& label,int maxlen=35);
    QString _backgroundColor;
    QString _foregroundColor;
    QString _font;
};

class DPPage
{
public:
    DPPage(const QDomElement& e);
    DPPage(const char* title);
    QString title() const { return _title ; }
    QList<DPPlot*> plots() { return _plots; }
    DPPlot* addPlot(const char* title);

    double startTime();
    double stopTime();
    QString backgroundColor();
    QString foregroundColor();

    void setStartTime(double startTime);
    void setStopTime(double stopTime);
    void setBackgroundColor(const QString& color);
    void setForegroundColor(const QString& color);

private:
    QString _title;
    QList<DPPlot*> _plots;
    double _startTime;
    double _stopTime;
    QString _backgroundColor;
    QString _foregroundColor;
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
    static QStringList paramList(const QStringList& dpFileNames);

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
