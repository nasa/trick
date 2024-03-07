#include "session.h"

Session::Session() :
    _device("terminal"),
    _presentation("compare"),
    _timeMatchTolerance(1.0e-6),
    _frequency(0.0),
    _fg(""),
    _bg(""),
    _start(-DBL_MAX),
    _stop(DBL_MAX),
    _orient("landscape"),
    _timeName(""),
    _map(""),
    _mapFile(""),
    _isLegend(true),
    _showTables(""),
    _videoFileName(""),
    _videoOffset(0.0),
    _isShowPageTitle(true),
    _isShowPlotLegend(""),
    _xAxisLabel(""),
    _yAxisLabel("")
{
    _titles << "" << "" << "" << "";
    _colors << "" << "" << ""
            << "" << "" << "" << "";
    _legendLabels << "" << "" << ""
                  << "" << "" << "" << "";
    _linestyles << "" << "" << ""
                << "" << "" << "" << "";
    _symbolstyles << "" << "" << ""
                  << "" << "" << "" << "";
    _symbolends << "" << "" << ""
                << "" << "" << "" << "";
    _groups << "" << "" << ""
            << "" << "" << "" << "";
}

Session::Session(const QString &sessionFileName) :
    _device("terminal"),
    _presentation("compare"),
    _timeMatchTolerance(1.0e-6),
    _frequency(0.0),
    _fg(""),
    _bg(""),
    _start(-DBL_MAX),
    _stop(DBL_MAX),
    _orient("landscape"),
    _timeName(""),
    _map(""),
    _mapFile(""),
    _isLegend(true),
    _videoFileName(""),
    _videoOffset(0.0),
    _isShowPageTitle(true),
    _isShowPlotLegend(""),
    _plotLegendPosition("ne"),
    _xAxisLabel(""),
    _yAxisLabel("")
{
    _titles << "" << "" << "" << "";
    _colors << "" << "" << ""
            << "" << "" << "" << "";
    _legendLabels << "" << "" << ""
                  << "" << "" << "" << "";
    _linestyles << "" << "" << ""
                << "" << "" << "" << "";
    _symbolstyles << "" << "" << ""
                  << "" << "" << "" << "";
    _symbolends << "" << "" << ""
                << "" << "" << "" << "";
    _groups << "" << "" << ""
            << "" << "" << "" << "";

    QFile file(sessionFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        fprintf(stderr, "koviz [error]: Cannot read session file %s!\n",
                sessionFileName.toLatin1().constData());
        exit(-1);
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.contains("RUN:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("RUN:",0,Qt::CaseInsensitive);
            QString run = line.mid(i+4).trimmed();
            run = run.remove("\"");
            if ( run.isEmpty() ) {
                fprintf(stderr,"koviz [error]: empty run specification in "
                               "session file %s.\n",
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
            _runs << run.split(" ",QString::SkipEmptyParts).at(0);

        } else if ( line.contains("PRODUCT:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("PRODUCT:",0,Qt::CaseInsensitive);
            QString dp = line.mid(i+8).trimmed();
            if ( dp.startsWith("\"") ) {
                dp = dp.mid(1);
            }
            if ( dp.endsWith("\"") ) {
                dp.chop(1);
            }
            _dps << dp;
        } else if ( line.contains("DEVICE:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("DEVICE:",0,Qt::CaseInsensitive);
            _device = line.mid(i+7).trimmed();
            _device = _device.remove("\"");
            if ( _device.isEmpty() ) {
                fprintf(stderr,"koviz [error]: empty device specification in "
                               "session file %s.\n",
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
            QStringList list = _device.split(" ",QString::SkipEmptyParts);
            if ( !QString::compare(list.at(0),"FILE",Qt::CaseInsensitive) ) {
                _device = list.at(1); // device is filename
                if ( _device == "terminal") { // in case filename=="terminal"
                    _device = "terminal.pdf";
                }
            } else {
                if ( !_device.compare("terminal",Qt::CaseInsensitive) ) {
                    _device = "terminal";
                } else {
                    // device is considered to be a filename
                }
            }
        } else if ( line.contains("PRESENTATION:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("PRESENTATION:",0,Qt::CaseInsensitive);
            _presentation = line.mid(i+13).trimmed();
            if ( _presentation.startsWith("\"") ) {
                _presentation = _presentation.mid(1);
            }
            if ( _presentation.endsWith("\"") ) {
                _presentation.chop(1);
            }
            if ( _presentation != "compare" &&
                 _presentation != "error" &&
                 _presentation != "error+compare") {
                fprintf(stderr,"koviz [error]: session file has presentation "
                               "set to \"%s\".  Presentation should be"
                               "set to \"compare\", \"error\" or "
                               "\"error+compare\"\n",
                               _presentation.toLatin1().constData());
                exit(-1);
            }

        } else if (line.contains("TIME_MATCH_TOLERANCE:",Qt::CaseInsensitive)) {
            int i = line.indexOf("TIME_MATCH_TOLERANCE:",0,Qt::CaseInsensitive);
            bool ok;
            _timeMatchTolerance = line.mid(i+21).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: time match tolerance in session "
                               "file %s is corrupt.\n",
                        sessionFileName.toLatin1().constData());
                exit(-1);
            }

        } else if ( line.contains("FREQUENCY:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("FREQUENCY:",0,Qt::CaseInsensitive);
            bool ok;
            _frequency = line.mid(i+10).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: frequency spec in session "
                               "file %s is corrupt.\n",
                        sessionFileName.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains(QRegExp("[Tt][1-4]:")) ) {
            int i = line.indexOf(QRegExp("[Tt][1-4]:"),0);
            int titleId = QString(line.at(i+1)).toInt(); // 1-4
            QString title = line.mid(i+3).trimmed();
            if ( title.startsWith("\"") ) {
                title = title.mid(1);
            }
            if ( title.endsWith("\"") ) {
                title.chop(1);
            }
            _titles.replace(titleId-1,title);
        } else if ( line.contains(QRegExp("[Cc][1-7]:")) ) {
            int i = line.indexOf(QRegExp("[Cc][1-7]:"),0);
            int colorId = QString(line.at(i+1)).toInt(); // 1-7
            QString color = line.mid(i+3).trimmed();
            if ( color.startsWith("\"") ) {
                color = color.mid(1);
            }
            if ( color.endsWith("\"") ) {
                color.chop(1);
            }
            _colors.replace(colorId-1,color);
        } else if ( line.contains(QRegExp("[Ll][1-7]:")) ) {
            int i = line.indexOf(QRegExp("[Ll][1-7]:"),0);
            int legendId = QString(line.at(i+1)).toInt(); // 1-7
            QString label = line.mid(i+3).trimmed();
            if ( label.startsWith("\"") ) {
                label = label.mid(1);
            }
            if ( label.endsWith("\"") ) {
                label.chop(1);
            }
            _legendLabels.replace(legendId-1,label);
        } else if ( line.contains(QRegExp("[Ll][Ss][1-7]:")) ) {
            int i = line.indexOf(QRegExp("[Ll][Ss][1-7]:"),0);
            int lsId = QString(line.at(i+2)).toInt(); // 1-7
            QString linestyle = line.mid(i+4).trimmed();
            if ( linestyle.startsWith("\"") ) {
                linestyle = linestyle.mid(1);
            }
            if ( linestyle.endsWith("\"") ) {
                linestyle.chop(1);
            }
            _linestyles.replace(lsId-1,linestyle);
        } else if ( line.contains(QRegExp("[Ss][1-7]:")) ) {
            int i = line.indexOf(QRegExp("[Ss][1-7]:"),0);
            int ssId = QString(line.at(i+1)).toInt(); // 1-7
            QString symbolstyle = line.mid(i+3).trimmed();
            if ( symbolstyle.startsWith("\"") ) {
                symbolstyle = symbolstyle.mid(1);
            }
            if ( symbolstyle.endsWith("\"") ) {
                symbolstyle.chop(1);
            }
            _symbolstyles.replace(ssId-1,symbolstyle);
        } else if ( line.contains(QRegExp("[Ss][Ee][1-7]:")) ) {
            int i = line.indexOf(QRegExp("[Ss][Ee][1-7]:"),0);
            int seId = QString(line.at(i+2)).toInt(); // 1-7
            QString symbolend = line.mid(i+4).trimmed();
            if ( symbolend.startsWith("\"") ) {
                symbolend = symbolend.mid(1);
            }
            if ( symbolend.endsWith("\"") ) {
                symbolend.chop(1);
            }
            _symbolends.replace(seId-1,symbolend);
        } else if ( line.contains(QRegExp("[Gg][1-7]:")) ) {
            int i = line.indexOf(QRegExp("[Gg][1-7]:"),0);
            int groupId = QString(line.at(i+1)).toInt(); // 1-7
            QString group = line.mid(i+3).trimmed();
            if ( group.startsWith("\"") ) {
                group = group.mid(1);
            }
            if ( group.endsWith("\"") ) {
                group.chop(1);
            }
            _groups.replace(groupId-1,group);
        } else if ( line.contains(QRegExp("[Ff][Gg]:")) ) {
            int i = line.indexOf(QRegExp("[Ff][Gg]:"),0);
            QString color = line.mid(i+3).trimmed();
            if ( color.startsWith("\"") ) {
                color = color.mid(1);
            }
            if ( color.endsWith("\"") ) {
                color.chop(1);
            }
            _fg = color;
        } else if ( line.contains(QRegExp("[Bb][Gg]:")) ) {
            int i = line.indexOf(QRegExp("[Bb][Gg]:"),0);
            QString color = line.mid(i+3).trimmed();
            if ( color.startsWith("\"") ) {
                color = color.mid(1);
            }
            if ( color.endsWith("\"") ) {
                color.chop(1);
            }
            _bg = color;
        } else if ( line.contains("START:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("START:",0,Qt::CaseInsensitive);
            bool ok;
            _start = line.mid(i+6).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: start spec in session "
                               "file %s is corrupt.\n",
                        sessionFileName.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains("STOP:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("STOP:",0,Qt::CaseInsensitive);
            bool ok;
            _stop = line.mid(i+5).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: stop spec in session "
                               "file %s is corrupt.\n",
                        sessionFileName.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains("ORIENT:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("ORIENT:",0,Qt::CaseInsensitive);
            _orient = line.mid(i+7).trimmed();
            if ( _orient.startsWith("\"") ) {
                _orient = _orient.mid(1);
            }
            if ( _orient.endsWith("\"") ) {
                _orient.chop(1);
            }
            if ( _orient != "portrait" && _orient != "landscape" ) {
                fprintf(stderr,"koviz [error]: session file has orienation "
                               "set to \"%s\".  Koviz only "
                               "supports \"portrait\" and \"landscape\"\n",
                               _orient.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains("SHIFT:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("SHIFT:",0,Qt::CaseInsensitive);
            _shift = line.mid(i+6).trimmed();
            if ( _shift.startsWith("\"") ) {
                _shift = _shift.mid(1);
            }
            if ( _shift.endsWith("\"") ) {
                _shift.chop(1);
            }
        } else if ( line.contains("timeName:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("timeName:",0,Qt::CaseInsensitive);
            _timeName = line.mid(i+9).trimmed();
            if ( _timeName.startsWith("\"") ) {
                _timeName = _timeName.mid(1);
            }
            if ( _timeName.endsWith("\"") ) {
                _timeName.chop(1);
            }
        } else if ( line.contains("map:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("map:",0,Qt::CaseInsensitive);
            _map = line.mid(i+4).trimmed();
            if ( _map.startsWith("\"") ) {
                _map = _map.mid(1);
            }
            if ( _map.endsWith("\"") ) {
                _map.chop(1);
            }
        } else if ( line.contains("mapFile:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("mapFile:",0,Qt::CaseInsensitive);
            _mapFile = line.mid(i+8).trimmed();
            if ( _mapFile.startsWith("\"") ) {
                _mapFile = _mapFile.mid(1);
            }
            if ( _mapFile.endsWith("\"") ) {
                _mapFile.chop(1);
            }
        } else if ( line.contains("showPlotLegend:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("showPlotLegend:",0,Qt::CaseInsensitive);
            _isShowPlotLegend = line.mid(i+15).trimmed();
            if ( _isShowPlotLegend.startsWith("\"") ) {
                _isShowPlotLegend = _isShowPlotLegend.mid(1);
            }
            if ( _isShowPlotLegend.endsWith("\"") ) {
                _isShowPlotLegend.chop(1);
            }
        } else if ( line.contains("legend:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("legend:",0,Qt::CaseInsensitive);
            QString isLegendString = line.mid(i+7).trimmed();
            if ( isLegendString.startsWith("\"") ) {
                isLegendString = isLegendString.mid(1);
            }
            if ( isLegendString.endsWith("\"") ) {
                isLegendString.chop(1);
            }
            bool ok;
            _isLegend = Options::stringToBool(isLegendString,&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: bad legend spec=\"%s\" in "
                               "session file %s.  Value should be a boolean.\n",
                               isLegendString.toLatin1().constData(),
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains("showTables:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("showTables:",0,Qt::CaseInsensitive);
            QString showTables = line.mid(i+11).trimmed();
            if ( showTables.startsWith("\"") ) {
                showTables = showTables.mid(1);
            }
            if ( showTables.endsWith("\"") ) {
                showTables.chop(1);
            }
            bool ok;
            bool isShow = Options::stringToBool(showTables,&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: bad showTables spec=\"%s\" in "
                               "session file %s.  Value should be a boolean.\n",
                               showTables.toLatin1().constData(),
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
            if ( isShow ) {
                _showTables = "true";
            } else {
                _showTables = "false";
            }
        } else if ( line.contains("exclude:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("exclude:",0,Qt::CaseInsensitive);
            _excludePattern = line.mid(i+8).trimmed();
            if ( _excludePattern.startsWith("\"") ) {
                _excludePattern = _excludePattern.mid(1);
            }
            if ( _excludePattern.endsWith("\"") ) {
                _excludePattern.chop(1);
            }
        } else if ( line.contains("filter:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("filter:",0,Qt::CaseInsensitive);
            _filterPattern = line.mid(i+7).trimmed();
            if ( _filterPattern.startsWith("\"") ) {
                _filterPattern = _filterPattern.mid(1);
            }
            if ( _filterPattern.endsWith("\"") ) {
                _filterPattern.chop(1);
            }
        } else if ( line.contains("video:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("video:",0,Qt::CaseInsensitive);
            _videoFileName = line.mid(i+6).trimmed();
            if ( _videoFileName.startsWith("\"") ) {
                _videoFileName = _videoFileName.mid(1);
            }
            if ( _videoFileName.endsWith("\"") ) {
                _videoFileName.chop(1);
            }
        } else if ( line.contains("videoOffset:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("videoOffset:",0,Qt::CaseInsensitive);
            bool ok;
            _videoOffset = line.mid(i+12).trimmed().toDouble(&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: videoOffset spec in session "
                               "file %s is corrupt.\n",
                        sessionFileName.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains("showPageTitle:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("showPageTitle:",0,Qt::CaseInsensitive);
            QString isShowTxt = line.mid(i+14).trimmed();
            if ( isShowTxt.startsWith("\"") ) {
                isShowTxt = isShowTxt.mid(1);
            }
            if ( isShowTxt.endsWith("\"") ) {
                isShowTxt.chop(1);
            }
            bool ok;
            bool isShow = Options::stringToBool(isShowTxt,&ok);
            if ( !ok ) {
                fprintf(stderr,"koviz [error]: bad showPageTitle "
                               "spec=\"%s\" in session file %s.  "
                               "Value should be a boolean.\n",
                               isShowTxt.toLatin1().constData(),
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
            if ( isShow ) {
                _isShowPageTitle = true;
            } else {
                _isShowPageTitle = false;
            }
        } else if ( line.contains("plotLegendPosition:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("plotLegendPosition:",0,Qt::CaseInsensitive);
            _plotLegendPosition = line.mid(i+19).trimmed();
            if ( _plotLegendPosition.startsWith("\"") ) {
                _plotLegendPosition = _plotLegendPosition.mid(1);
            }
            if ( _plotLegendPosition.endsWith("\"") ) {
                _plotLegendPosition.chop(1);
            }
            QStringList list;
            list << "ne" << "e" << "se" << "s" << "sw" << "w" << "nw" << "n" ;
            if ( ! list.contains(_plotLegendPosition) ) {
                fprintf(stderr,"koviz [error]: bad plotLegendPosition "
                               "spec=\"%s\" in session file %s.  "
                               "Value should be ne,e,se,s,sw,w,nw or n\n",
                               _plotLegendPosition.toLatin1().constData(),
                               sessionFileName.toLatin1().constData());
                exit(-1);
            }
        } else if ( line.contains("xaxislabel:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("xaxislabel:",0,Qt::CaseInsensitive);
            _xAxisLabel = line.mid(i+11).trimmed();
            if ( _xAxisLabel.startsWith("\"") ) {
                _xAxisLabel = _xAxisLabel.mid(1);
            }
            if ( _xAxisLabel.endsWith("\"") ) {
                _xAxisLabel.chop(1);
            }
        } else if ( line.contains("yaxislabel:",Qt::CaseInsensitive) ) {
            int i = line.indexOf("yaxislabel:",0,Qt::CaseInsensitive);
            _yAxisLabel = line.mid(i+11).trimmed();
            if ( _yAxisLabel.startsWith("\"") ) {
                _yAxisLabel = _yAxisLabel.mid(1);
            }
            if ( _yAxisLabel.endsWith("\"") ) {
                _yAxisLabel.chop(1);
            }
        }
    }

    file.close();
}
