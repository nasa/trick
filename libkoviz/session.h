#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRegExp>
#include <float.h>
#include <stdlib.h>
#include "options.h"

class Session
{
public:
    Session(const QString& sessionFileName);

    QStringList runs() const { return _runs; }
    QStringList dps() const { return _dps; }
    QString device() const { return _device; }
    QString presentation() const { return _presentation; }
    double timeMatchTolerance() const { return _timeMatchTolerance; }
    double frequency() const { return _frequency; }
    QString title1() const { return _titles.at(0); }
    QString title2() const { return _titles.at(1); }
    QString title3() const { return _titles.at(2); }
    QString title4() const { return _titles.at(3); }
    QString color1() const { return _colors.at(0); }
    QString color2() const { return _colors.at(1); }
    QString color3() const { return _colors.at(2); }
    QString color4() const { return _colors.at(3); }
    QString color5() const { return _colors.at(4); }
    QString color6() const { return _colors.at(5); }
    QString color7() const { return _colors.at(6); }
    QString legend1() const { return _legendLabels.at(0); }
    QString legend2() const { return _legendLabels.at(1); }
    QString legend3() const { return _legendLabels.at(2); }
    QString legend4() const { return _legendLabels.at(3); }
    QString legend5() const { return _legendLabels.at(4); }
    QString legend6() const { return _legendLabels.at(5); }
    QString legend7() const { return _legendLabels.at(6); }
    QString linestyle1() const { return _linestyles.at(0); }
    QString linestyle2() const { return _linestyles.at(1); }
    QString linestyle3() const { return _linestyles.at(2); }
    QString linestyle4() const { return _linestyles.at(3); }
    QString linestyle5() const { return _linestyles.at(4); }
    QString linestyle6() const { return _linestyles.at(5); }
    QString linestyle7() const { return _linestyles.at(6); }
    QString symbolstyle1() const { return _symbolstyles.at(0); }
    QString symbolstyle2() const { return _symbolstyles.at(1); }
    QString symbolstyle3() const { return _symbolstyles.at(2); }
    QString symbolstyle4() const { return _symbolstyles.at(3); }
    QString symbolstyle5() const { return _symbolstyles.at(4); }
    QString symbolstyle6() const { return _symbolstyles.at(5); }
    QString symbolstyle7() const { return _symbolstyles.at(6); }
    QString group1() const { return _groups.at(0); }
    QString group2() const { return _groups.at(1); }
    QString group3() const { return _groups.at(2); }
    QString group4() const { return _groups.at(3); }
    QString group5() const { return _groups.at(4); }
    QString group6() const { return _groups.at(5); }
    QString group7() const { return _groups.at(6); }
    QString foreground() const { return _fg; }
    QString background() const { return _bg; }
    double start() const { return _start; }
    double stop()  const { return _stop; }
    QString orient() const { return _orient; }
    QString shift() const { return _shift; }
    QString timeName() const { return _timeName; }
    QString map() const { return _map; }
    QString mapFile() const { return _mapFile; }
    bool isLegend() const { return _isLegend; }
    QString showTables() const { return _showTables; }
    QString excludePattern() const { return _excludePattern; }
    QString filterPattern() const { return _filterPattern; }
    QString videoFileName() const { return _videoFileName; }
    double videoOffset() const { return _videoOffset; }
    bool isShowPageTitle() const { return _isShowPageTitle; }
    QString isShowPlotLegend() const { return _isShowPlotLegend; }
    QString plotLegendPosition() const { return _plotLegendPosition; }
    QString xAxisLabel() const { return _xAxisLabel; }
    QString yAxisLabel() const { return _yAxisLabel; }

private:
    Session();

    QStringList _runs;
    QStringList _dps;
    QString _device;
    QString _presentation;
    double _timeMatchTolerance;
    double _frequency;
    QStringList _titles;
    QStringList _colors;
    QStringList _legendLabels;
    QStringList _linestyles;
    QStringList _symbolstyles;
    QStringList _groups;
    QString _fg;
    QString _bg;
    double _start;
    double _stop;
    QString _orient;
    QString _shift;
    QString _timeName;
    QString _map;
    QString _mapFile;
    bool _isLegend;
    QString _showTables; // should be empty or a boolean (poor man's troolean)
    QString _excludePattern;
    QString _filterPattern;
    QString _videoFileName;
    double _videoOffset;
    bool _isShowPageTitle;
    QString _isShowPlotLegend;
    QString _plotLegendPosition;
    QString _xAxisLabel;
    QString _yAxisLabel;
};

#endif // SESSION_H
