#include "rundir.h"

RunDir::RunDir(const QString &run,
               const QStringList &timeNames,
               const QHash<QString, QStringList> &varMap,
               const QString &filterPattern,
               const QString &excludePattern) :
    _varMap(varMap)
{
    if ( ! QFileInfo(run).exists() ) {
        fprintf(stderr, "koviz [error]: RunDir couldn't find run dir=%s\n",
                run.toLatin1().constData());
        exit(-1);
    }

    QStringList files = RunDir::_fileList(run,filterPattern,excludePattern);

    if ( files.empty() ) {
        fprintf(stderr, "koviz [error]: Either no *.trk/csv/mot files \n"
                        "               in run dir: %s\n"
                        "               or log files were filtered out.\n",
                        run.toLatin1().constData());
        exit(-1);
    }

    foreach (QString file, files) {
        QString ffile = run + "/" + file;
        DataModel* model = DataModel::createDataModel(timeNames,run,ffile);
        model->unmap();
        int ncols = model->columnCount();
        for ( int col = 0; col < ncols; ++col ) {
            QString param = model->param(col)->name();
            foreach (QString key, varMap.keys() ) {
                if ( param == key ) {
                    break;
                }
                QStringList vals = varMap.value(key);
                QStringList names;
                foreach ( QString val, vals ) {
                    MapValue mapval(val);
                    names.append(mapval.name());
                }
                if ( names.contains(param) ) {
                    param = key;
                    break;
                }
            }
            _param2model.insert(param,model);
        }
    }
}

QStringList RunDir::params()
{
    QStringList lparams = _param2model.keys();
    lparams.removeDuplicates();
    lparams.sort();
    return lparams;
}

DataModel *RunDir::dataModel(const QString &param)
{
    DataModel* model = _param2model.value(param);
    if ( !model ) {
        // Look in varmap for param (e.g. when DP does not use map key for var)
        foreach (QString key, _varMap.keys()) {
            QStringList names;
            foreach (QString val, _varMap.value(key)) {
                MapValue mapval(val);
                names.append(mapval.name());
            }
            if ( names.contains(param) ) {
                model = _param2model.value(key);
                break;
            }
        }
    }
    return model;
}

bool RunDir::isValid(const QString &run,
                     const QString &filterPattern,
                     const QString &excludePattern)
{
    QStringList files = RunDir::_fileList(run,filterPattern,excludePattern);
    if ( files.isEmpty() ) {
        return false;
    } else {
        return true;
    }
}

QStringList RunDir::_fileList(const QString& run,
                              const QString &filterPattern,
                              const QString &excludePattern)
{
    QStringList files;

    if ( ! QFileInfo(run).exists() ) {
        return files; // empty list
    }

    QStringList filter;
    filter << "*.trk" << "*.csv" << "*.mot";

    QRegExp filterRgx(filterPattern);
    QRegExp excludeRgx(excludePattern);

    QDir runDir(run);
    files = runDir.entryList(filter, QDir::Files);

    if ( files.contains("log_timeline.csv") ) {
        files.removeAll("log_timeline.csv");
    }
    if ( files.contains("log_timeline_init.csv") ) {
        files.removeAll("log_timeline_init.csv");
    }
    if ( files.contains("_init_log.csv") ) {
        files.removeAll("_init_log.csv");
    }
    if ( !excludeRgx.isEmpty() ) {
        QStringList excludeFiles = files.filter(excludeRgx);
        foreach (QString excludeFile, excludeFiles) {
            files.removeAll(excludeFile);
        }
    }
    if ( !filterRgx.isEmpty() ) {
        QStringList filterFiles = files.filter(filterRgx);
        if ( !filterFiles.isEmpty() ) {
            // If the filter has found a match, use filter,
            // otherwise do not
            files = filterFiles;
        }
    }

    return files;
}
