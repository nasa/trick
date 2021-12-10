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
#include <QRectF>
#include <stdexcept>
#include <float.h>
#include <limits.h>

#include "parameter.h"

#include "product_lexer.h"
#include "product_parser.h"
extern int yyparse(void);


class DPProgram
{
  public:
    DPProgram() {}
    ~DPProgram();
    QString fileName() const {return _fileName; }
    QStringList inputs() const {return _inputs; }
    QStringList outputs() const {return _outputs; }
    QList<Parameter> inputParams() const ;
    QList<Parameter> outputParams() const ;

    void setFileName(const QString& fileName) { _fileName = fileName; }
    void addInput(const QString& input);
    void addOutput(const QString& output);

  private:
    QString _fileName;
    QStringList _inputs;
    QStringList _outputs;
    QList<Parameter> _inputParams;
    QList<Parameter> _outputParams;
};

class DPVar
{
public:
    DPVar(const QDomElement& e);
    DPVar(const char* name);
    QString name() { return _name; }
    QString label() { return _label; }
    QString unit() { return _unit; }
    QString lineColor() { return _lineColor; }
    QString lineStyle() { return _lineStyle; }
    double scaleFactor() { return _scaleFactor; }
    double bias() { return _bias; }
    QString symbolStyle() { return _symbol; }
    QString symbolSize() { return _symbolSize; }
    QString timeName() const { return _timeName; }
    QString format() const { return _format; }
    double  minRange() const { return _minRange; }
    double  maxRange() const { return _maxRange; }

    void setLabel(const char* label) { _label = label; }
    void setUnit(const char* unit) { _unit = unit; }
    void setLineColor(const char* lineColor) { _lineColor = lineColor; }
    void setLineStyle(const char* ls) { _lineStyle = ls; }
    void setScaleFactor(double sf) { _scaleFactor = sf; }
    void setBias(double b) { _bias = b; }
    void setSymbolStyle(const char* ss) { _symbol = ss; }
    void setSymbolSize(const char* ss) { _symbolSize = ss; }
    void setTimeName(const char* t) { _timeName = t; }
    void setFormat(const char* f) { _format = f; }
    void setMinRange(double range) { _minRange = range; }
    void setMaxRange(double range) { _maxRange = range; }

private:
    QString _name;
    QString _label;
    QString _unit;
    QString _lineColor;
    QString _lineStyle;
    double _scaleFactor;
    double _bias;
    QString _symbol;
    QString _symbolSize;
    QString _timeName;
    QString _format;
    double  _minRange;
    double  _maxRange;
};

class DPXYPair
{
public:
    DPXYPair(const QDomElement& e);
    DPXYPair(DPVar* x, DPVar* y);
    DPVar* x() { return _x; }
    DPVar* y() { return _y; }

private:
    DPVar* _x;
    DPVar* _y;
};

class DPCurve
{
public:
    DPCurve() : _x(0),_y(0) {}

    DPCurve(const QDomElement& e);
    ~DPCurve();
    DPVar* x();
    DPVar* y();

    DPVar* setXVarName(const char* name);
    DPVar* setYVarName(const char* name);

    QList<DPXYPair*> xyPairs();
    void addXYPair(DPVar* x, DPVar* y);

    QString lineColor();
    void setLineColor(const char* lineColor);

    QString lineStyle();
    void setLineStyle(const char* lineStyle);

    QString symbolStyle();
    void setSymbolStyle(const char* style);

    QString symbolSize();
    void setSymbolSize(const char* size); // tiny,small,medium,large

private:
    DPVar* _x;
    DPVar* _y;
    QString _color;  // TODO: should this be a member of y, like symbolStyle?
    QList<DPXYPair*> _xyPairs;
    static QString _err_string;
    static QTextStream _err_stream;
};

class DPPlot
{
public:
    DPPlot(const QDomElement& e);
    DPPlot(const char* title);
    ~DPPlot();
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
    QString plotXScale() const;
    QString plotYScale() const;
    QString plotRatio() const;
    QList<double> majorXTics() const;
    QList<double> majorYTics() const;
    QList<double> minorXTics() const;
    QList<double> minorYTics() const;
    QRectF rect() const ;
    QString presentation() const;

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
    void setPlotXScale(const QString& plotXScale);
    void setPlotYScale(const QString& plotYScale);
    void setPlotRatio(const QString& plotRatio);
    void setMajorXTics(const QList<double>& xTics);
    void setMajorYTics(const QList<double>& yTics);
    void setMinorXTics(const QList<double>& xTics);
    void setMinorYTics(const QList<double>& yTics);
    void setRect(double x, double y, double w, double h);
    void setPresentation(const QString& presentation);

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
    QString _plotXScale;
    QString _plotYScale;
    QString _plotRatio;
    QList<double> _majorXTics;
    QList<double> _majorYTics;
    QList<double> _minorXTics;
    QList<double> _minorYTics;
    QRectF _rect;
    QString _presentation;
};

class DPPage
{
public:
    DPPage(const QDomElement& e);
    DPPage(const char* title);
    ~DPPage();
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

class DPTable
{
public:
    DPTable(const char* title);
    DPTable(const QDomElement& e);
    ~DPTable();

    QString title() const { return _title ; }
    QString delimiter() const { return _delimiter; }
    double startTime() const { return _startTime; }
    double stopTime() const { return _stopTime; }
    QList<DPVar*> vars() const { return _vars; }

    void setDelimiter(const QString& d) { _delimiter = d; }
    void setStartTime(double startTime) { _startTime = startTime; }
    void setStopTime(double stopTime) { _stopTime = stopTime; }

    DPVar* addVar(const char* title);

private:
    QString _title;
    QString _delimiter;
    double _startTime;
    double _stopTime;
    QList<DPVar*> _vars;
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
    QList<DPTable*> tables() const { return _tables; }
    DPTable* addTable(const char* title);

    // Made for speed and used when filtering for params in DP
    static QStringList paramList(const QString& fileName);
    static QStringList paramList(const QStringList& dpFileNames,
                                 const QString &timeName);

    // Used for -dp2trk option
    static QStringList tableParamList(const QString& fileName);
    static QStringList tableParamList(const QStringList& dpFileNames,
                                      const QString &timeName);

    DPProgram* program() { return _program; }

private:
    QString _fileName;
    QDomDocument* _doc;
    DPProgram* _program;
    QString _title;
    QList<DPPage*> _pages;
    QList<DPTable*> _tables;
    static QString _err_string;
    static QTextStream _err_stream;

    void _handleDPXMLFile(const QString &fileName);
    void  _handleDP05File(QString &contents);
};

#endif // DPPLOT_H
