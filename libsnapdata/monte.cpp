#include "monte.h"
#include <QDebug>
#include "timeit_linux.h"

Monte::Monte(const QString& dirname, int beginRun, int endRun) :
    _montedir(dirname),
    _beginRun(beginRun),
    _endRun(endRun),
    _err_stream(&_err_string)
{
    if ( ! dirname.isEmpty() ) {
        setDir(dirname);
    }
    if ( _beginRun < 0 ) {
        _err_stream << "snap [error]: Monte::Monte() constructor has "
                    << "negative number for beginRun argument";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }
}

Monte::~Monte()
{
    foreach (QString ftrk, _ftrk2models.keys()) {
        QList<TrickModel*>* models = _ftrk2models.value(ftrk);
        foreach(TrickModel* model, *models) {
            delete model;
        }
        delete models;
    }
}

bool Monte::setDir(const QString &montedir)
{
    bool ok = true;
    _montedir = montedir;

    QDir mdir(montedir);
    if ( ! mdir.exists() ) {
        _err_stream << "snap [error]: couldn't find run directory: "
                    << montedir << "\n";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    QStringList filters;
    filters << "RUN_*";
    mdir.setNameFilters(filters);

    //
    // Sanity check: Are there any RUNs in the monte carlo dir?
    //
    _runs = mdir.entryList(filters, QDir::Dirs);
    if ( _runs.empty() ) {
        _err_stream << "snap [error]: no RUN dirs in "
                    << _montedir << "\n";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    // Get RUN_00000
    QString run0 = _runs.at(0);
    QDir rundir0(_montedir + "/" + run0);

    //
    // Make a list of trk log files from RUN_00000
    //
    QSet<QString> trks;
    QStringList filter_trk;
    filter_trk << "*.trk";
    QStringList skip;
    skip <<  "log_frame.trk" << "log_jobs.trk" << "log_mem_stats.trk";
    QStringList trklist = rundir0.entryList(filter_trk, QDir::Files);
    foreach ( QString trk, trklist ) {
        if ( skip.contains(trk) ) continue;
        trks << trk;
    }
    if ( trks.empty() ) {
        _err_stream << "snap [error]: no trk logfiles found in "
                    << _montedir << "\n";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    //
    // Make list of params from RUN_00000
    //
    foreach (QString ftrk, trks.values()) {
        QString trk(_montedir + "/" + run0 + "/" + ftrk);
        TrickModel* m = new TrickModel(trk,trk);
        int ncols = m->columnCount();
        for ( int col = 0; col < ncols; ++col) {
            QString param = m->headerData(col,
                                          Qt::Horizontal,Param::Name).toString();
            if ( ! _params.contains(param) ) {
                _params.append(param);
                _param2ftrk.insert(param,ftrk);
            }
        }
        delete m;
    }

    // Make subset of runs based on beginRun and endRun option
    foreach ( QString run, _runs ) {
        int runId = run.mid(4).toInt();
        if ( runId < _beginRun || runId > _endRun ) {
            continue;
        }
        _runsSubset.append(run);
    }
    if ( _runsSubset.empty() ) {
        _err_stream << "snap [error]: after applying -beginRun="
                    << _beginRun
                    << " and -endRun="
                    << _endRun
                    << " options, RUNs subset is empty for range "
                    << _runs.at(0) << "-" << _runs.at(_runs.size()-1)
                    << " in directory:\n"
                    << _montedir << "\n";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    //
    // Make sure all runs have same trks and params
    // Also, map ftrk to list of models
    //
    QString beginRunDir = _runsSubset.at(0);
    foreach (QString ftrk, trks.values()) {
        QStringList params;
        _ftrk2models.insert(ftrk,new QList<TrickModel*>);
        foreach ( QString run, _runsSubset ) {
            QDir rundir(_montedir + "/" + run);
            if ( ! rundir.exists(ftrk) ) {
                _err_stream << "snap [error]: monte carlo runs are "
                            << "inconsistent. " << _runs.at(0)
                            << " contains file " << ftrk
                            << " but run "
                            << run << " does not.";
                throw std::invalid_argument(_err_string.toAscii().constData());
            }
            QString trk(_montedir + "/" + run + "/" + ftrk);
            TrickModel* m = new TrickModel(trk,trk);
            int ncols = m->columnCount();
            if ( run == beginRunDir ) {
                params.clear();
                for ( int col = 0; col < ncols; ++col) {
                    QString param = m->headerData(col,
                                        Qt::Horizontal,Param::Name).toString();
                    params.append(param);
                }
            } else {
                if ( ncols != params.size() ) {
                    _err_stream << "snap [error]: monte carlo runs are "
                                << "inconsistent. " << beginRunDir << "/"
                                << ftrk << " has a different number of "
                                << "parameters(" << params.size()
                                << ") than run "
                                << run << "/" << ftrk << "("
                                   << ncols << ")";
                    throw std::invalid_argument(_err_string.toAscii().
                                                constData());
                }
                for ( int col = 0; col < ncols; ++col) {
                    QString param = m->headerData(col,
                                         Qt::Horizontal,Param::Name).toString();
                    if ( param != params.at(col) ) {
                        _err_stream << "snap [error]: monte carlo runs are "
                                    << "inconsistent. " << beginRunDir << "/"
                                    << ftrk << " and " << run << "/" << ftrk
                                    << " have two different parameters \n"
                                    << "      " << params.at(col) << "\n"
                                    << "      " << param << "\n"
                                    << "in column " << col << ".";
                        throw std::invalid_argument(_err_string.toAscii().
                                                    constData());
                    }
                }
            }
            _ftrk2models.value(ftrk)->append(m);
            m->unmap();
        }
    }

    return ok;
}

QList<TrickModel *> *Monte::models(const QString &param)
{
    return _ftrk2models.value(_param2ftrk.value(param));
}

//
// Create table model from monte_runs file
// Table structure:
//
// Note:  beginRun,endRun options will return a subset of runs
//
//       Var0     Var1                      VarN
// Run0  v00       v01                        v0n
// Run1  v10       v11                        v1n
//
// Runn  vm0       vm1                        vmn
//
QStandardItemModel *Monte::inputModel()
{
    QStandardItemModel* m = 0 ;

    QString monteInputsFileName("monte_runs");
    monteInputsFileName.prepend("/");
    monteInputsFileName.prepend(_montedir);

    QFile file(monteInputsFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _err_stream << "snap [error]: could not open "
                    << monteInputsFileName << "\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }
    QTextStream in(&file);

    //
    // Get Num Runs
    //
    int nRuns = 0 ;
    bool isNumRuns = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.startsWith("NUM_RUNS:") ) {
            nRuns = line.split(':').at(1).trimmed().toInt(&isNumRuns);
            break;
        }
    }
    if ( !isNumRuns ) {
        _err_stream << "snap [error]: error parsing monte carlo input file: "
                    << monteInputsFileName << "."
                    << "Couldn't find/parse \"NUM_RUNS:\" line.";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }
    if ( nRuns != _runs.size() ) {
        _err_stream << "snap [error]: error parsing monte carlo input file.  "
                    << "The \"NUM_RUNS:\" line indicate that there are "
                    << nRuns << " RUN directories, "
                    << "however there are actually "
                    << _runs.size() << ".\n"
                    << "See input file:\n"
                    << monteInputsFileName << "\n"
                    << "and monte directory:"
                    << _montedir << "\n";
        throw std::invalid_argument(_err_string.toAscii().constData());
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
        _err_stream << "snap [error]: error parsing monte carlo input file: "
                    << monteInputsFileName << "."
                    << "Couldn't find \"VARS:\" line";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }
    while ( !in.atEnd() ) {
        QString var = in.readLine();
        if ( !var.startsWith("DATA:") ) {
            int delim = var.indexOf(' ');
            var = var.mid(0,delim);
            var = var.split('.').last();
            //if ( var.split('.').last() == "RunId" ) {
            if ( var == "RunId" ) {
                var = "RunId";
            }
            if ( ! var.isEmpty() ) {
                vars.append(var); //<----------------------- meat
            }
        } else {
            isData = true;
            break;
        }
    }
    if ( !isData ) {
        _err_stream << "snap [error]: error parsing monte carlo input file: "
                    << monteInputsFileName << "."
                    << "Couldn't find \"DATA:\" line";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    //
    // Allocate table items
    //
    nRuns = runs().size(); // If beginRun,endRun set, runs could be a subset
    m = new QStandardItemModel(nRuns,vars.size());

    //
    // Unfortunately, since the vertical header is not sorted along with
    // data columns, make them empty strings.
    //
    // Hopefully, like I saw in Briscoe's data, column 0 will have the runID.
    //
    int r = 0 ;
    foreach ( QString run, _runsSubset ) {
        int runId = run.mid(4).toInt(); // 4 is for RUN_
        QString runName = QString("%0").arg(runId);
        m->setHeaderData(r,Qt::Vertical,runName);
        r++;
    }

    //
    // Get Data
    //
    int nDataLines = 0 ;
    while (!in.atEnd()) {
        QString dataLine = in.readLine();
        int hashIdx = dataLine.indexOf('#');
        if ( hashIdx >= 0 ) {
            dataLine = dataLine.mid(0,hashIdx);
        }
        dataLine = dataLine.trimmed();
        QStringList dataVals = dataLine.split(' ',QString::SkipEmptyParts);
        if ( dataVals.empty() ) continue;
        bool isSkip = false;
        if ( dataVals.size() == vars.size() ) {
            for ( int c = 0; c < dataVals.size(); ++c) {
                QString val = dataVals.at(c) ;
                if ( c == 0 ) {
                    // First val is the runId
                    int runId = val.toInt();
                    if ( runId < _beginRun || runId > _endRun ) {
                        // Skip runs outside of optional runSubset
                        isSkip = true;
                        break;
                    } else {
                        QString runName= QString("%0").arg(runId);
                        runName = runName.rightJustified(5, '0');
                        runName.prepend("RUN_");
                        if ( ! _runsSubset.contains(runName) ) {
                            _err_stream
                                << "snap [error]: error parsing monte carlo "
                                << "input file. RunID for Run=" << runName
                                << " is in the monte carlo input file, but "
                                << " can't find the RUN_ directory. "
                                << "Look in file:"
                                << "\n\n"
                                << "File:\n" << monteInputsFileName;
                            throw std::invalid_argument(_err_string.toAscii().constData());
                        }
                    }
                } else {
                    // Not a run, this is a monte carlo param input value
                    double v = val.toDouble();
                    val = val.sprintf("%.4lf",v);
                }
                NumSortItem *item = new NumSortItem(val);
                m->setItem(nDataLines,c,item); //<----------------------- meat
                m->setHeaderData(c,Qt::Horizontal,vars.at(c));
            }
            if ( isSkip ) {
                // Skip run outside of beginRun,endRun subset
                continue;
            }
        } else {
            _err_stream << "snap [error]: error parsing monte carlo input file: "
                        << "  There are " << vars.size() << " variables in "
                        << "VARS list, but only " << dataVals.size()
                        << " fields on line number " << nDataLines << ".  "
                        << "\n\n"
                        << "File:\n" << monteInputsFileName
                        << "Line:\n" << dataLine+1;

            throw std::invalid_argument(_err_string.toAscii().constData());
        }
        nDataLines++;
    }
    if ( nDataLines != runs().size() ) {
    }

    file.close();

    return m;

}
