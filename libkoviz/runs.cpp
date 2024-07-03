#include "runs.h"

QString Runs::_err_string;
QTextStream Runs::_err_stream(&Runs::_err_string);

Runs::Runs() :
    _runPaths(QStringList()),
    _varMap(QHash<QString,QStringList>()),
    _isShowProgress(true)
{
}

Runs::Runs(const QStringList &timeNames,
           const QStringList &runPaths,
           const QHash<QString,QStringList>& varMap,
           const QString &filterPattern,
           const QString &excludePattern,
           bool isShowProgress) :
    _timeNames(timeNames),
    _runPaths(runPaths),
    _varMap(varMap),
    _filterPattern(filterPattern),
    _excludePattern(excludePattern),
    _isShowProgress(isShowProgress),
    _runsModel(new QStandardItemModel(this))
{
    if ( runPaths.isEmpty() ) {
        return;
    }
    _init();
}

Runs::~Runs()
{
    _delete();
}

void Runs::_delete()
{
    foreach ( DataModel* m, _models ) {
        delete m;
    }
    _models.clear();

    foreach ( QString p, _params ) {
        delete _paramToModels.value(p);
    }
    _paramToModels.clear();


    foreach ( Run* run, _runs ) {
        delete run;
    }
    _runs.clear();

    _params.clear();
}


// Refresh runs (normally called when sim reruns and gens new data)
void Runs::refresh()
{
    _delete();
    _init();
}

void Runs::clear()
{
    _delete();
    _runsModel->clear();
    _runPaths.clear();
    _init();
}

void Runs::addRun(const QString &runPath)
{
    _runPaths.append(runPath);
    refresh();
}

void Runs::_init()
{
    _montePath.clear();
    if ( _runPaths.size() == 1 ) {
        QFileInfo fileInfo(_runPaths.at(0));
        if ( fileInfo.fileName().startsWith("MONTE_") ) {
            _montePath = _runPaths.at(0);
        }
    }

    if ( !_montePath.isEmpty() ) {

        QDir monteDir(_montePath);
        if ( ! monteDir.exists() ) {
            fprintf(stderr,
                    "koviz [error]: couldn't find monte directory: %s\n",
                    _montePath.toLatin1().constData());
            exit(-1);
        }

        // Make list of RUNs in the monte directory
        QStringList filters;
        filters << "RUN_*";
        monteDir.setNameFilters(filters);
        QStringList monteRuns = monteDir.entryList(filters, QDir::Dirs);
        if ( monteRuns.empty() ) {
            fprintf(stderr, "koviz [error]: no RUN dirs in %s \n",
                    _montePath.toLatin1().constData());
            exit(-1);
        }

        QStringList runsList = _runsSubset(monteRuns,
                                           _filterPattern,
                                           _excludePattern,
                                           0,1e20);
        _runPaths.clear();
        foreach ( QString run, runsList ) {
            _runPaths << _montePath + "/" + run;
        }
    } else {
        if ( _runPaths.size() > 0 ) {
            _runPaths = _runsSubset(_runPaths,
                                    _filterPattern,
                                    _excludePattern,
                                    0,1e20);
        }
    }

    // Begin Progress Dialog
    QProgressDialog* progress = 0;
    if ( _isShowProgress ) {
        if ( _runPaths.size() > 1 ) {
            // Show progress when loading more than one model
            progress =  new QProgressDialog("Initializing data models...",
                                            "Abort", 0, _runPaths.size(), 0);
            progress->setWindowModality(Qt::WindowModal);
            progress->setMinimumDuration(500);
        }
    }

    int ii = 0;
    foreach ( QString runPath, _runPaths ) {
        QFileInfo fi(runPath);
        Run* run = 0;
        if ( fi.isDir() ) {
            run = new RunDir(runPath,_timeNames,_varMap,
                             _filterPattern,_excludePattern);
        } else if ( fi.isFile() ) {
            run = new RunFile(runPath,_timeNames,_varMap);
        }
        _runs.append(run);
        if ( progress ) {
            if (progress->wasCanceled()) {
                exit(0);
            }
            progress->setValue(++ii);
        }
    }

    // End Progress Dialog
    if ( progress ) {
        progress->setValue(_runPaths.size());
        delete progress;
    }

    // Make list of params that are in each run (coplottable)
    QSet<QString> paramSet;
    foreach ( Run* run, _runs ) {
        QSet<QString> runParamSet = run->params().toSet();
        if ( paramSet.isEmpty() ) {
            paramSet = runParamSet;
        } else {
            paramSet = paramSet.intersect(runParamSet);
        }
    }
    _params = paramSet.toList();
    _params.removeDuplicates();
    _params.sort();

    // Load runs or monte input model
    _loadRunsModel(_runsModel,_montePath,_runPaths);

    emit runsRefreshed();
}

CurveModel *Runs::curveModel(int row,
                             const QString &tName,
                             const QString &xName,
                             const QString &yName) const
{
    CurveModel* curveModel = 0;
    if ( row < 0 || row >= _runs.size() ) {
        return 0;
    }
    Run* run = _runs.at(row);
    DataModel* model = run->dataModel(yName);
    if ( !model ) {
        return 0;
    }

    int tcol = _paramColumn(model,tName) ;
    int xcol = _paramColumn(model,xName) ;
    int ycol = _paramColumn(model,yName) ;

    if ( tcol < 0 || xcol < 0 ) {
        fprintf(stderr, "koviz [error]: Runs::curveModel() : koviz does not "
                "support making a curve from parameters that reside in "
                "separate files. Found:\n"
                "    y=%s in file=%s\n"
                "but could not find:\n",
                yName.toLatin1().constData(),
                model->fileName().toLatin1().constData());
        if ( tcol < 0 ) {
            fprintf(stderr, "   t=%s in file=%s\n",
                    tName.toLatin1().constData(),
                    model->fileName().toLatin1().constData());
        }
        if ( xcol < 0 ) {
            fprintf(stderr, "   x=%s in file=%s\n",
                    xName.toLatin1().constData(),
                    model->fileName().toLatin1().constData());
        }
        exit(-1);
    }

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

QStandardItemModel* Runs::runsModel()
{
    return _runsModel;
}

void Runs::_loadRunsModel(QStandardItemModel *runsModel,
                          const QString &montePath,
                          const QStringList &runPaths)
{
    if ( !montePath.isEmpty() ) {
        _loadMonteInputModel(runsModel,montePath,runPaths);
    } else {
        __loadRunsModel(runsModel,runPaths);
    }
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

void Runs::__loadRunsModel(QStandardItemModel *model,
                           const QStringList &runs)
{
    model->clear();
    model->setRowCount(runs.size());
    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal,"RunId");
    model->setHeaderData(1,Qt::Horizontal,"RunName");

    QStringList fruns;
    foreach ( QString run, runs ) {
        QString frun = QFileInfo(run).absoluteFilePath();
        fruns << frun;
    }
    QStringList runNames = Runs::abbreviateRunNames(fruns);

    int r = 0 ;
    foreach ( QString runName, runNames ) {
        model->setHeaderData(r,Qt::Vertical,fruns.at(r));
        QString runIdString ;
        runIdString = runIdString.sprintf("%d",r);
        NumSortItem *runIdItem = new NumSortItem(runIdString);
        QStandardItem* runNameItem = new QStandardItem(runName);
        model->setItem(r,0,runIdItem);
        model->setItem(r,1,runNameItem);
        r++;
    }
}


//
// Create table model from monte_runs file
//
//       Var0        Var1                         Varn
// Run0  val00       val01                        val0n
// Run1  val10       val11                        val1n
// ...
// Runm  valm0       valm1                        valmn
//
void Runs::_loadMonteInputModel(QStandardItemModel* model,
                                const QString& montePath,
                                const QStringList& runPaths)
{
    model->clear();

    if ( montePath.isEmpty() ) {
        return;
    }

    QDir monteDir(montePath);
    if ( ! monteDir.exists() ) {
        fprintf(stderr,
                "koviz [error]: couldn't find monte directory: %s\n",
                montePath.toLatin1().constData());
        exit(-1);
    }

    QString monteInputFile("monte_runs");
    monteInputFile.prepend("/");
    monteInputFile.prepend(montePath);

    QFile file(monteInputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr, "koviz [error]: could not open %s\n",
                         monteInputFile.toLatin1().constData());
        exit(-1);
    }
    QTextStream in(&file);

    bool isTrick07 = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.startsWith("NUM_RUNS:") ) {
            isTrick07 = true;
            break;
        }
    }
    file.close();

    if ( isTrick07 ) {
        _loadMonteInputModelTrick07(model,monteInputFile,runPaths);
    } else {
        _loadMonteInputModelTrick17(model,monteInputFile,runPaths);
    }
}

void Runs::_loadMonteInputModelTrick07(QStandardItemModel* model,
                                       const QString &monteInputFile,
                                       const QStringList& runs)
{
    model->clear();

    QFile file(monteInputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr, "koviz [error]: could not open %s\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }
    QTextStream in(&file);

    //
    // Get Num Runs
    //
    bool isNumRuns = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.startsWith("NUM_RUNS:") ) {
            line.split(':').at(1).trimmed().toInt(&isNumRuns);
            break;
        }
    }
    if ( !isNumRuns ) {
        fprintf(stderr, "koviz [error]: error parsing monte carlo input file: "
                        "%s.   Couldn't find/parse \"NUM_RUNS:\" line.\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }

    //
    // Get Vars
    //
    QStringList vars;
    bool isVars = false;
    bool isData = false;
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        if ( line.startsWith("VARS:") ) {
            isVars = true;
            break;
        }
    }
    if ( ! isVars ) {
        fprintf(stderr, "koviz [error]: error parsing monte carlo input file: "
                        "%s.   Couldn't find/parse \"VARS:\" line.\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }
    while ( !in.atEnd() ) {
        QString var = in.readLine();
        if ( var.startsWith('#')) continue;
        if ( !var.startsWith("DATA:") ) {
            int delim = var.indexOf(' ');
            var = var.mid(0,delim);
            var = var.split('.').last();
            if ( ! var.isEmpty() ) {
                vars.append(var);
            }
        } else {
            isData = true;
            break;
        }
    }
    if ( !isData ) {
        fprintf(stderr, "koviz [error]: error parsing monte carlo input file: "
                        "%s.   Couldn't find/parse \"DATA:\" line.\n",
                        monteInputFile.toLatin1().constData());
        exit(-1);
    }

    //
    // Allocate table items
    //
    int nRuns = runs.size();
    model->setRowCount(nRuns);
    model->setColumnCount(vars.size());

    //
    // Set table header for rows and get begin/end run ids
    //
    int beginRun = INT_MAX;
    int endRun = 0;
    int r = 0 ;
    foreach ( QString run, runs ) {
        int i = run.lastIndexOf("RUN_");
        int runId = run.mid(i+4).toInt(); // 4 is for RUN_
        if ( runId < beginRun ) {
            beginRun = runId;
        }
        if ( runId > endRun ) {
            endRun = runId;
        }
        QString runName = QString("%0").arg(runId);
        model->setHeaderData(r,Qt::Vertical,runName);
        r++;
    }

    //
    // Get Data
    //
    int nDataLines = 0 ;
    while (!in.atEnd()) {

        QString dataLine = in.readLine();

        //
        // Get runId
        //
        int hashIdx = dataLine.indexOf('#');
        if ( hashIdx < 0 ) {
            fprintf(stderr, "koviz [error]: Each dataline in monte_runs should "
                            "have a runId following a hash mark.\n"
                            "The following line does not:\n    \"%s\"\n",
                            dataLine.toLatin1().constData());
            exit(-1);
        }
        int runId = dataLine.mid(hashIdx+1).trimmed().toInt();
        if ( runId < beginRun || runId > endRun ) {
            continue;
        }

        //
        // Get RUN_<nnnn> dir
        //
        QString runName= QString("%0").arg(runId);
        runName = runName.rightJustified(5, '0');
        runName = _montePath + "/RUN_" + runName;
        if ( ! runs.contains(runName) ) {
            // Run is in the monte carlo input file,
            // but it is not in the runs list.
            // Assume that RUN was excluded intentionally,
            // and not missing.
            continue;
        }
        int c = 0;
        QString runIdString ;
        runIdString = runIdString.sprintf("%d",runId);
        NumSortItem *runIdItem = new NumSortItem(runIdString);
        model->setItem(nDataLines,c,runIdItem);
        model->setHeaderData(c,Qt::Horizontal,"RunId");

        //
        // Monte carlo input data values
        //
        dataLine = dataLine.mid(0,hashIdx).trimmed();
        QStringList dataVals = dataLine.split(' ',QString::SkipEmptyParts);
        if ( dataVals.size() != vars.size() ) {
            fprintf(stderr,
                    "koviz [error]: error parsing monte carlo input file.  "
                    "There are %d variables in VARS list, but only %d fields "
                    "on line number %d.\n\n"
                    "File:%s\n"
                    "Line:%s\n",
                    vars.size(), dataVals.size(),
                    nDataLines,
                    monteInputFile.toLatin1().constData(),
                    dataLine.toLatin1().constData());
            exit(-1);
        }
        c = 1;
        foreach ( QString val, dataVals ) {
            double v = val.toDouble();
            val = val.sprintf("%.4lf",v);
            NumSortItem *item = new NumSortItem(val);
            model->setItem(nDataLines,c,item);
            model->setHeaderData(c,Qt::Horizontal,vars.at(c-1));
            ++c;
        }

        nDataLines++;
    }

    file.close();
}

void Runs::_loadMonteInputModelTrick17(QStandardItemModel* model,
                                       const QString &monteInputFile,
                                       const QStringList& runs)
{
    model->clear();

    QFile file(monteInputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr,"koviz [error]: could not open %s\n",
                       monteInputFile.toLatin1().constData());
        exit(-1);
    }
    QTextStream in(&file);

    //
    // Sanity check - top line should be #NAME:
    //
    QString line = in.readLine();
    if ( !line.startsWith("#NAME:") ) {
        fprintf(stderr,"koviz [error]: error parsing %s. "
                       "First line should begin with \"#NAME:\"\n",
                       monteInputFile.toLatin1().constData());
        exit(-1);
    }

    //
    // Get Vars
    //
    QStringList vars;
    vars << "RunId";
    in.seek(0); // Go to beginning of file
    while (!in.atEnd()) {
        line = in.readLine();
        if ( line.startsWith("#NAME:") ) {
            QString var = line.split(QRegExp("\\s+")).at(1);
            vars << var;
        }
    }

    //
    // Get number of runs
    //
    bool isCount = false;
    int nRuns = 0;
    in.seek(0); // Go to beginning of file
    while (!in.atEnd()) {
        line = in.readLine();
        if ( line.startsWith("00000") ) {
             isCount = true;
        }
        if ( isCount ) {
            ++nRuns;
        }
    }


    //
    // Allocate table items
    //
    nRuns = runs.size(); // If beginRun,endRun set, runs could be a subset
    model->setRowCount(nRuns);
    model->setColumnCount(vars.size());

    //
    // Set table header for rows and get begin/end run ids
    //
    int beginRun = INT_MAX;
    int endRun = 0;
    int r = 0 ;
    foreach ( QString run, runs ) {
        int i = run.lastIndexOf("RUN_");
        int runId = run.mid(i+4).toInt(); // 4 is for RUN_
        if ( runId < beginRun ) {
            beginRun = runId;
        }
        if ( runId > endRun ) {
            endRun = runId;
        }
        QString runName = QString("%0").arg(runId);
        model->setHeaderData(r,Qt::Vertical,runName);
        r++;
    }

    //
    // Data
    //
    bool isParseLine = false;
    int lineNum = 0;
    int runLine = 0;
    in.seek(0); // Go to beginning of file
    while (!in.atEnd()) {
        ++lineNum;
        line = in.readLine();
        if ( line.startsWith("00000") ) {
             isParseLine = true;
        }
        if ( !isParseLine ) {
            continue;
        }
        QStringList vals = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if ( vals.size() != vars.size() ) {
            fprintf(stderr, "koviz [error]: error parsing %s.  There "
                            "are %d variables specified in top line, "
                            "but only %d values on line number %d.\n",
                           monteInputFile.toLatin1().constData(),
                           vars.size(),vals.size(),lineNum);
            exit(-1);
        }

        int runId = vals.at(0).toInt();
        if ( runId < beginRun || runId > endRun ) {
            continue;
        }

        QString runName= QString("%0").arg(runId);
        runName = runName.rightJustified(5, '0');
        runName = _montePath + "/RUN_" + runName;
        if ( ! runs.contains(runName) ) {
            // Run is in the monte carlo input file,
            // but it is not in the runs list.
            // Assume that RUN was excluded intentionally,
            // and not missing.
            continue;
        }

        int nv = vals.size();
        for ( int c = 0; c < nv; ++c) {
            QString val = vals.at(c) ;
            double v = val.toDouble();
            if ( c == 0 ) {
                int ival = val.toInt();
                val = val.sprintf("%d",ival);
            } else {
                val = val.sprintf("%.4lf",v);
            }
            NumSortItem *item = new NumSortItem(val);
            model->setItem(runLine,c,item);
            model->setHeaderData(c,Qt::Horizontal,vars.at(c));
        }

        ++runLine;
    }

    file.close();
}

// Make subset of runs based on beginRun and endRun option
QStringList Runs::_runsSubset(const QStringList& runsList,
                             const QString& filterPattern,
                             const QString& excludePattern,
                             uint beginRun, uint endRun)
{
    QStringList subset;

    QRegExp frgx(filterPattern);
    QRegExp ergx(excludePattern);

    QStringList filteredRunsList = runsList;
    if ( !frgx.isEmpty() ) {
        filteredRunsList = runsList.filter(frgx);
        if ( filteredRunsList.isEmpty() ) {
            // If the filter did not find a match,
            // do not use filter
            filteredRunsList = runsList;
        }
    }

    foreach ( QString run, filteredRunsList ) {
        bool ok = false;
        QString runName = QFileInfo(run).fileName();
        unsigned int runId = runName.mid(4).toInt(&ok);
        if ( ok && (runId < beginRun || runId > endRun) ) {
            continue;
        }
        if (!ergx.isEmpty() &&
             QFileInfo(run).absoluteFilePath().contains(ergx)) {
            continue;
        }
        subset.append(run);
    }
    if ( subset.empty() ) {
        fprintf(stderr,"koviz [error]: Runs::_runsSubset() is empty.  "
                       "Two possible reasons:\n"
                       "   1) -beginRun -endRun options excluding all runs\n"
                       "   2) -exclude <pattern> option excluding all runs\n"
                       "Aborting!!!");
        exit(-1);
    }

    return subset;
}
