#include "runs.h"

QString Runs::_err_string;
QTextStream Runs::_err_stream(&Runs::_err_string);

Runs::Runs() :
    _runDirs(QStringList()),
    _varMap(QHash<QString,QStringList>()),
    _isShowProgress(true)
{
}

Runs::Runs(const QStringList &timeNames,
           const QStringList &runDirs,
           const QHash<QString,QStringList>& varMap,
           bool isShowProgress) :
    _timeNames(timeNames),
    _runDirs(runDirs),
    _varMap(varMap),
    _isShowProgress(isShowProgress)
{
    if ( runDirs.isEmpty() ) {
        return;
    }
    _init();
}

Runs::~Runs()
{
    foreach ( DataModel* m, _models ) {
        delete m;
    }
    foreach ( QString p, _params ) {
        delete _paramToModels.value(p);
    }
}

void Runs::_init()
{
    QStringList filter;
    filter << "*.trk" << "*.csv";
    QStringList files;
    QHash<QString,QStringList> runToFiles;
    QHash<QString,QString> fileToRun;
    foreach ( QString run, _runDirs ) {
        if ( ! QFileInfo(run).exists() ) {
            _err_stream << "koviz [error]: couldn't find run directory: "
                        << run << "\n";
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
        QDir runDir(run);
        QStringList lfiles = runDir.entryList(filter, QDir::Files);
        if ( lfiles.contains("log_timeline.csv") ) {
            lfiles.removeAll("log_timeline.csv");
        }
        if ( lfiles.contains("log_timeline_init.csv") ) {
            lfiles.removeAll("log_timeline_init.csv");
        }

        if ( lfiles.empty() ) {
            _err_stream << "koviz [error]: no *.trk/csv files in run dir: "
                        << run << "\n";
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
        QStringList fullPathFiles;
        foreach (QString file, lfiles) {
            QString ffile = run + '/' + file;
            fullPathFiles.append(ffile);
            if ( !files.contains(ffile) ) {
                files << ffile;
                fileToRun.insert(ffile,run);
            }
        }
        runToFiles.insert(run,fullPathFiles);
    }

    // Begin Progress Dialog
    const int nFiles = files.size();
    QProgressDialog* progress = 0;
    if ( _isShowProgress ) {
        progress =  new QProgressDialog("Initializing data models...",
                                        "Abort", 0, nFiles, 0);
        progress->setWindowModality(Qt::WindowModal);
        progress->setMinimumDuration(500);
    }

    QHash<QString,QStringList> runToParams;
    QHash<QPair<QString,QString>,DataModel*> pfnameToModel;
    int i = 0;
    foreach (QString fname, files ) {
        if ( _isShowProgress ) {
            if ( nFiles > 7 ) {
                // Only show progress when loading many files (7 is arbitrary)
                progress->setValue(i);
                if (progress->wasCanceled()) {
                    exit(0);
                }
            } else {
                progress->setValue(files.size());
            }
        }
        DataModel* m = DataModel::createDataModel(_timeNames,fname);
        m->unmap();
        _models.append(m);
        int ncols = m->columnCount();
        QStringList mParams;
        for ( int col = 0; col < ncols; ++col ) {
            QString p = m->param(col)->name();
            foreach (QString key, _varMap.keys() ) {
                if ( p == key ) {
                    break;
                }
                QString runDir = QFileInfo(fname).absolutePath();
                QStringList vals = _varMap.value(key);
                if ( vals.contains(p) ) {
                    p = key;
                    break;
                } else {
                    bool isFound = false;
                    foreach ( QString val, vals ) {
                        if ( val.contains(':') ) {
                            QStringList l = val.split(':');
                            QString run = QFileInfo(l.at(0).trimmed()).
                                                            absoluteFilePath();
                            QString var =  l.at(1);
                            if ( run == runDir && p == var) {
                                p = key;
                                isFound = true;
                                break;
                            }
                        }
                    }
                    if ( isFound ) {
                        break;
                    }
                }
            }
            mParams << p;
            pfnameToModel.insert(qMakePair(p,fname),m);
        }
        QString run = fileToRun.value(fname);
        QStringList params = runToParams.value(run);
        params.append(mParams);
        params.removeDuplicates();
        params.sort();
        runToParams.insert(run,params);
        ++i;
    }

    // End Progress Dialog
    if ( _isShowProgress ) {
        progress->setValue(nFiles);
        delete progress;
    }

    // Make list of params that are in each run (coplottable)
    QSet<QString> paramSet;
    foreach ( QString run, _runDirs ) {
        QSet<QString> runParamSet = runToParams.value(run).toSet();
        if ( paramSet.isEmpty() ) {
            paramSet = runParamSet;
        } else {
            paramSet = paramSet.intersect(runParamSet);
        }
    }
    _params = paramSet.toList();
    _params.removeDuplicates();
    _params.sort();

    foreach ( QString p, _params ) {
        _paramToModels.insert(p,new QList<DataModel*>);
        foreach ( QString run, _runDirs ) {
            DataModel* m = 0;
            foreach ( QString fname, runToFiles.value(run) ) {
                QPair<QString,QString> pfname = qMakePair(p,fname);
                if ( pfnameToModel.contains(pfname) ) {
                    m = pfnameToModel.value(pfname);
                    break;
                }
            }
            _paramToModels.value(p)->append(m);
        }
    }
}

CurveModel* Runs::curveModel(int row,
                        const QString &tName,
                        const QString &xName,
                        const QString &yName) const
{
    QString runDir = QFileInfo(_runDirs.at(row)).absoluteFilePath();

    QString t = tName;
    QString x = xName;
    QString y = yName;

    QList<DataModel*>* models = 0;

    if ( tName.contains(':') || xName.contains(':') || yName.contains(':') ) {
        QString varName;
        if ( tName.contains(':') ) {
            varName = tName;
        } else if ( xName.contains(':') ) {
            varName = xName;
        } else {
            varName = yName;
        }
        fprintf(stderr, "koviz [error]: var=%s contains a colon.\n"
                "The RUN:var syntax has been superceded by use of koviz maps.  "
                "See the \"Maps\" section of the User's Guide for info.\n",
                varName.toLatin1().constData());
        exit(-1);
    }

    if ( _paramToModels.value(y) ) {
        if ( _paramToModels.value(y)->at(row)->paramColumn(y) >= 0 ) {
            models = _paramToModels.value(y);
        }
    }

    if ( models == 0 ) {
        if ( _varMap.contains(y) ) {
            QList<DataModel*>* mdls = _paramToModels.value(y);
            if ( mdls ) {
                foreach ( QString yval, _varMap.value(y) ) {
                    if ( yval.contains(':') ) {
                        QStringList l = yval.split(':');
                        QString run = QFileInfo(l.at(0).trimmed()).
                                                        absoluteFilePath();
                        if ( run == runDir ) {
                            int ycol = mdls->at(row)->
                                             paramColumn(l.at(1).trimmed());
                            if ( ycol >= 0 ) {
                                models = mdls;
                                y = l.at(1).trimmed();
                                break;
                            }
                        }
                    } else {
                        if ( mdls->at(row)->paramColumn(yval) >= 0 ) {
                            models = mdls;
                            y = yval;
                            break;
                        }
                    }
                }
            }
        }
    }

    if ( models == 0 ) {
        return 0;
    }

    DataModel* tm = models->at(row);
    if ( !tm ) {
        return 0;
    }

    int tcol = tm->paramColumn(t) ;
    if ( tcol < 0 ) {
        if ( _varMap.contains(t) ) {
            foreach ( QString tval, _varMap.value(t) ) {
                if ( tval.contains(':') ) {
                    QStringList l = tval.split(':');
                    QString run=QFileInfo(l.at(0).trimmed()).absoluteFilePath();
                    if ( run == runDir ) {
                        tcol = tm->paramColumn(l.at(1).trimmed());
                        if ( tcol >= 0 ) {
                            t = l.at(1).trimmed();
                            break;
                        }
                    }
                } else {
                    tcol = tm->paramColumn(tval);
                    if ( tcol >= 0 ) {
                        t = tval;
                        break;
                    }
                }
            }
        }
        if ( tcol < 0 ) {
            foreach (QString timeName, _timeNames ) {
                tcol = tm->paramColumn(timeName);
                if ( tcol >= 0 ) {
                    t = timeName;
                    break;
                }
            }
            if ( tcol < 0 ) {
                return 0;
            }
        }
    }

    int ycol = tm->paramColumn(y) ;
    if ( ycol < 0 ) {
        return 0;
    }

    QString xp = x;
    if ( xp.isEmpty() ) {
        xp = t;
    }
    int xcol = tm->paramColumn(xp) ;
    if ( xcol < 0 ) {
        if ( _varMap.contains(xp) ) {
            foreach ( QString xval, _varMap.value(xp) ) {
                if ( xval.contains(':') ) {
                    QStringList l = xval.split(':');
                    QString run=QFileInfo(l.at(0).trimmed()).absoluteFilePath();
                    if ( run == runDir ) {
                        xcol = tm->paramColumn(l.at(1).trimmed());
                        if ( xcol >= 0 ) {
                            break;
                        }
                    }
                } else {
                    xcol = tm->paramColumn(xval);
                    if ( xcol >= 0 ) {
                        break;
                    }
                }
            }
        }
        if ( xcol < 0 ) {
            if ( _timeNames.contains(xp) ) {
                foreach ( QString timeName, _timeNames ) {
                    xcol = tm->paramColumn(timeName);
                    if ( xcol >= 0 ) {
                        break;
                    }
                }
            }
            if ( xcol < 0 ) {
                return 0;
            }
        }
    }

    return new CurveModel(tm,tcol,xcol,ycol);
}

// Example1:
//
//     names:
//        "/the/rain/in/spain/falls/on/the/plain/good/grief",
//        "/the/rain/in/spokane/falls/on/the/hills/good/grief",
//        "/the/rain/in/space/falls/on/houston/good/grief"
//
//     returns:
//         "spain/falls/on/the/plain",
//         "spokane/falls/on/the/hills",
//         "space/falls/on/houston"
//
// Example2:
//
//     names:
//        "fhw/sensors/GPSR_model/verif/SIM_gpsr_model/RUN_test"
//        "fhw/sensors/GPSR_model/verif/SIM_gpsr_model/verif/RUN_test"
//
//     returns:
//        "RUN_test"
//        "verif/RUN_test"
//
QStringList Runs::abbreviateRunNames(const QStringList &runNames)
{
    QStringList names;

    QString prefix = Runs::commonPrefix(runNames,"/");
    QString suffix = Runs::commonSuffix(runNames,"/");

    // Would a run name be empty if we removed the common suffix and prefix?
    bool isEmpty = false;
    foreach ( QString s, runNames ) {
        s = s.remove(prefix);
        if ( s.startsWith("/") ) {
            s = s.mid(1); // remove prepended '/'
        }
        s = s.remove(suffix);
        if ( s.endsWith("/") ) {
            s.chop(1);
        }
        if ( s.isEmpty() ) {
            isEmpty = true;
        }
    }

    foreach ( QString s, runNames ) {

        if ( !isEmpty || (isEmpty && prefix.length() > suffix.length()) ) {
            s = s.remove(prefix);
            if ( s.startsWith("/") ) {
                s = s.mid(1); // remove prepended '/'
            }
        }

        if ( !isEmpty || (isEmpty && suffix.length() > prefix.length()) ) {
            s = s.remove(suffix);
            if ( s.endsWith("/") ) {
                s.chop(1);
            }
        }

        names << s;
    }

    return names;
}

// Example:
//     names:
//        name[0]=/the/rain/in/spain/falls/on/the/plain
//        name[1]=/the/rain/in/spokane/falls/on/the/hills
//        name[2]=/the/rain/in/space/falls/on/houston
//        sep = /
//
//     returns "/the/rain/in"
QString Runs::commonPrefix(const QStringList &names, const QString &sep)
{
    QString prefix;

    if ( names.isEmpty() ) return prefix;

    prefix = names.at(0);
    foreach ( QString name, names ) {
        prefix = __commonPrefix(prefix,name,sep);
    }

    return prefix;
}

// Example:
//     a = /the/rain/in/spain/falls/on/the/plain
//     b = /the/rain/in/spokane/falls/on/the/hills
//     sep = /
//
//     returns "/the/rain/in"
QString Runs::__commonPrefix(const QString &a, const QString &b,
                        const QString &sep)
{
    QString prefix;

    QStringList as = a.split(sep);
    QStringList bs = b.split(sep);
    QStringList names ;

    for (int i = 0; i < as.size(); ++i) {
        if ( bs.size() <= i ) {
            break;
        }
        if ( as.at(i) != bs.at(i) ) {
            break;
        }
        names << as.at(i);
    }

    prefix = names.join(sep) ;

    return prefix;
}

// Example:
//     names:
//        name[0]=/the/rain/in/spain/falls/on/the/plain
//        name[1]=/the/rain/in/spokane/falls/on/the/plain
//        name[2]=/the/rain/in/space/falls/on/the/plain
//        sep = /
//
//     returns "falls/on/the/plain"
QString Runs::commonSuffix(const QStringList &names, const QString &sep)
{
    QString suffix;

    if ( names.isEmpty() ) return suffix;

    suffix = names.at(0);
    foreach ( QString name, names ) {
        suffix = __commonSuffix(suffix,name,sep);
    }

    return suffix;
}

// Example:
//     a = /the/rain/in/spain/falls/on/the/plain
//     b = /the/rain/in/spokane/falls/on/the/plain
//     sep = /
//
//     returns "falls/on/the/plain"
QString Runs::__commonSuffix(const QString &a, const QString &b,
                             const QString &sep)
{
    QString suffix;

    QStringList as = a.split(sep);
    QStringList bs = b.split(sep);
    QStringList names ;

    int i = as.size()-1;
    int j = bs.size()-1;
    while ( i >= 0 && j >= 0 ) {
        QString aa = as.at(i);
        QString bb = bs.at(j);
        if ( aa != bb ) {
            break;
        }
        names.prepend(aa);
        --i;
        --j;
    }

    suffix = names.join(sep) ;

    return suffix;
}
