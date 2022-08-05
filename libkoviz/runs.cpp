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
           const QString &filterPattern,
           const QString &excludePattern,
           bool isShowProgress) :
    _timeNames(timeNames),
    _runDirs(runDirs),
    _varMap(varMap),
    _filterPattern(filterPattern),
    _excludePattern(excludePattern),
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
    filter << "*.trk" << "*.csv" << "*.mot";
    QStringList files;
    QHash<QString,QStringList> runToFiles;
    QHash<QString,QString> fileToRun;
    QRegExp filterRgx(_filterPattern);
    QRegExp excludeRgx(_excludePattern);
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
        if ( lfiles.contains("_init_log.csv") ) {
            lfiles.removeAll("_init_log.csv");
        }
        if ( !excludeRgx.isEmpty() ) {
            QStringList excludeFiles = lfiles.filter(excludeRgx);
            foreach (QString excludeFile, excludeFiles) {
                lfiles.removeAll(excludeFile);
            }
        }
        if ( !filterRgx.isEmpty() ) {
            QStringList filterFiles = lfiles.filter(filterRgx);
            if ( !filterFiles.isEmpty() ) {
                // If the filter has found a match, use filter,
                // otherwise do not
                lfiles = filterFiles;
            }
        }

        if ( lfiles.empty() ) {
            _err_stream << "koviz [error]: Either no *.trk/csv/mot files "
                           "in run dir: " << run << "\n"
                        << "               or log files were filtered out.\n";
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
                QStringList names;
                foreach ( QString val, vals ) {
                    MapValue mapval(val);
                    names.append(mapval.name());
                }
                if ( names.contains(p) ) {
                    p = key;
                    break;
                } else {
                    bool isFound = false;
                    foreach (QString val, vals) {
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
    int row = 0;
    foreach ( QString run, _runDirs ) {
        QDir rundir(run);
        QString fullrundir = rundir.absolutePath();
        _rundir2row.insert(fullrundir,row);
        ++row;
    }
}

CurveModel* Runs::curveModel(int row,
                        const QString &tName,
                        const QString &xName,
                        const QString &yName) const
{
    return curveModel(_runDirs.at(row),tName,xName,yName);
}

CurveModel* Runs::curveModel(const QString& rundir,
                        const QString &t,
                        const QString &x,
                        const QString &y) const
{
    CurveModel* curveModel = 0;

    DataModel* model = _paramModel(y,rundir);
    if ( !model ) {
        return 0;
    }
    int tcol = _paramColumn(model,t) ;
    int xcol = _paramColumn(model,x) ;
    int ycol = _paramColumn(model,y) ;

    curveModel = new CurveModel(model,tcol,xcol,ycol);

    // Mapfile unit, bias and scales
    foreach (QString key, _varMap.keys() ) {
        foreach (QString val, _varMap.value(key)) {
            MapValue mapval(val);
            if ( mapval.name() == curveModel->x()->name() ) {
                if ( !mapval.unit().isEmpty() ) {
                    curveModel->x()->setUnit(mapval.unit());
                }
                if ( mapval.bias() != 0.0 ) {
                    curveModel->x()->setBias(mapval.bias());
                }
                if ( mapval.scale() != 1.0 ) {
                    curveModel->x()->setScale(mapval.scale());
                }

            } else if ( mapval.name() == curveModel->y()->name() ) {
                if ( !mapval.unit().isEmpty() ) {
                    curveModel->y()->setUnit(mapval.unit());
                }
                if ( mapval.bias() != 0.0 ) {
                    curveModel->y()->setBias(mapval.bias());
                }

                if ( mapval.scale() != 1.0 ) {
                    curveModel->y()->setScale(mapval.scale());
                }
            }
        }
    }

    return curveModel;
}

DataModel* Runs::_paramModel(const QString &param, const QString& run) const
{
    DataModel* model = 0;
    QDir rundir(run);
    QString frundir = rundir.absolutePath();
    int row = _rundir2row.value(frundir);
    QList<DataModel*>* models = _paramToModels.value(param);
    if ( !models ) {
        // Look in varmap for param (case when DP does not use map key for var)
        foreach (QString key, _varMap.keys()) {
            QStringList names;
            foreach (QString val, _varMap.value(key)) {
                MapValue mapval(val);
                names.append(mapval.name());
            }
            if ( names.contains(param) ) {
                models = _paramToModels.value(key);
                break;
            }
        }
    }
    if ( models ) {
        model = models->at(row);
    }
    return model;
}


int Runs::_paramColumn(DataModel* model, const QString &param) const
{
    int col = -1;
    col = model->paramColumn(param) ;
    if ( col < 0 ) {
        // Search for param in _varMap keys and values
        foreach (QString key, _varMap.keys()) {
            QStringList names;
            names.append(key);
            foreach (QString val, _varMap.value(key)) {
                MapValue mapval(val);
                names.append(mapval.name());
            }
            if ( names.contains(param) ) {
                foreach (QString name, names ) {
                    col = model->paramColumn(name);
                    if ( col >= 0 ) {
                        break;
                    }
                }
                if ( col >= 0 ) {
                    break;
                }
            }
        }
    }
    if ( col < 0 ) {
        // Search for param in timeNames
        bool isParamTime = false;
        foreach (QString timeName, _timeNames ) {
            if ( timeName == param ) {
                isParamTime = true;
            }
        }
        if ( isParamTime ) {
            foreach (QString timeName, _timeNames ) {
                col = model->paramColumn(timeName);
                if ( col >= 0 ) {
                    break;
                }
            }
        }
    }
    if ( col < 0 ) {
        // If param is a mapped param, search each param in map
        QStringList params = param.split('=');
        foreach ( QString p, params ) {
            col = model->paramColumn(p);
            if ( col >= 0 ) {
                break;
            }
        }
    }
    if ( col < 0 ) {
        fprintf(stderr, "koviz [error]: Could not find:\n"
                        "         variable=%s\n"
                        "         in file=%s\n",
                param.toLatin1().constData(),
                model->fileName().toLatin1().constData());
        exit(-1);
    }

    return col;
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
