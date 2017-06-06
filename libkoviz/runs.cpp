#include "runs.h"

QString Runs::_err_string;
QTextStream Runs::_err_stream(&Runs::_err_string);

Runs::Runs() :
    _runs(QStringList()),
    _varMap(QHash<QString,QStringList>())
{
}

Runs::Runs(const QStringList &timeNames,
           const QStringList &runDirs,
           const QHash<QString,QStringList>& varMap, int beginRun, int endRun) :
    _timeNames(timeNames),
    _runs(runDirs),
    _varMap(varMap),
    _beginRun(beginRun),
    _endRun(endRun)
{
    if ( runDirs.isEmpty() ) {
        return;
    }
    _init();
}

Runs::~Runs()
{
    foreach ( TrickModel* m, _models ) {
        delete m;
    }
    foreach ( QString p, _params ) {
        delete _paramToModels.value(p);
    }
}

void Runs::_init()
{
    QStringList filter_trk;
    filter_trk << "*.trk";
    QStringList trks;
    QHash<QString,QStringList> runToTrks;
    QHash<QString,QString> trkToRun;
    foreach ( QString run, _runs ) {
        if ( ! QFileInfo(run).exists() ) {
            _err_stream << "koviz [error]: couldn't find run directory: "
                        << run << "\n";
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
        QDir runDir(run);
        QStringList ltrks = runDir.entryList(filter_trk, QDir::Files);
        if ( ltrks.empty() ) {
            _err_stream << "koviz [error]: no *.trk files in run directory: "
                        << run << "\n";
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
        QStringList fullPathTrks;
        foreach (QString trk, ltrks) {
            QString ftrk = run + '/' + trk;
            fullPathTrks.append(ftrk);
            if ( !trks.contains(ftrk) ) {
                trks << ftrk;
                trkToRun.insert(ftrk,run);
            }
        }
        runToTrks.insert(run,fullPathTrks);
    }

    QProgressDialog progress("Initializing data models...",
                              "Abort", 0, trks.size(), 0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(500);
    QHash<QString,QStringList> runToParams;
    QHash<QPair<QString,QString>,TrickModel*> ptrkToModel;
    int i = 0;
    foreach (QString trk, trks ) {
        progress.setValue(i);
        if (progress.wasCanceled()) {
            exit(0);
        }
        TrickModel* m = new TrickModel(_timeNames,trk,trk);
        m->unmap();
        _models.append(m);
        int ncols = m->columnCount();
        QStringList mParams;
        for ( int col = 0; col < ncols; ++col) {
            QString p = m->param(col).name();
            foreach (QString key, _varMap.keys() ) {
                QStringList vals = _varMap.value(key);
                bool isFound = false;
                foreach ( QString val, vals ) {
                    if ( p == val ) {
                        p = key;
                        isFound = true;
                        break;
                    }
                }
                if ( isFound ) {
                    break;
                }
            }
            mParams << p;
            ptrkToModel.insert(qMakePair(p,trk),m);
        }
        QString run = trkToRun.value(trk);
        QStringList params = runToParams.value(run);
        params.append(mParams);
        params.removeDuplicates();
        params.sort();
        runToParams.insert(run,params);
        ++i;
    }
    progress.setValue(trks.size());

    // Make list of params that are in each run (coplottable)
    QSet<QString> paramSet;
    foreach ( QString run, _runs ) {
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
        _paramToModels.insert(p,new QList<TrickModel*>);
        foreach ( QString run, _runs ) {
            TrickModel* m = 0;
            foreach ( QString trk, runToTrks.value(run) ) {
                QPair<QString,QString> ptrk = qMakePair(p,trk);
                if ( ptrkToModel.contains(ptrk) ) {
                    m = ptrkToModel.value(ptrk);
                    break;
                }
                if ( _varMap.contains(p) ) {
                    QStringList vals = _varMap.value(p);
                    bool isFound = false;
                    foreach ( QString val, vals ) {
                        QPair<QString,QString> ptrk = qMakePair(val,trk);
                        if ( ptrkToModel.contains(ptrk) ) {
                            m = ptrkToModel.value(ptrk);
                            isFound = true;
                            break;
                        }
                    }
                    if ( isFound ) {
                        break;
                    }
                }
            }
            _paramToModels.value(p)->append(m);
        }
    }
}

TrickCurveModel* Runs::curve(int row,
                             const QString &tparam,
                             const QString &xparam,
                             const QString &yparam) const
{
    QList<TrickModel*>* models = _paramToModels.value(yparam);
    if ( models == 0 ) {
        return 0;
    }

    TrickModel* tm = models->at(row);
    if ( !tm ) {
        return 0;
    }

    int tcol = tm->paramColumn(tparam) ;
    if ( tcol < 0 ) {
        if ( _varMap.contains(tparam) ) {
            foreach ( QString tval, _varMap.value(tparam) ) {
                tcol = tm->paramColumn(tval);
                if ( tcol >= 0 ) {
                    break;
                }
            }

        }
        if ( tcol < 0 ) {
            foreach (QString timeName, _timeNames ) {
                tcol = tm->paramColumn(timeName);
                if ( tcol >= 0 ) {
                    break;
                }
            }
            if ( tcol < 0 ) {
                return 0;
            }
        }
    }

    int ycol = tm->paramColumn(yparam) ;
    if ( ycol < 0 ) {
        if ( _varMap.contains(yparam) ) {
            foreach ( QString yval, _varMap.value(yparam) ) {
                ycol = tm->paramColumn(yval);
                if ( ycol >= 0 ) {
                    break;
                }
            }
            if ( ycol < 0 ) {
                return 0;
            }
        } else {
            return 0;
        }
    }

    QString xp = xparam;
    if ( xp.isEmpty() ) {
        xp = tparam;
    }
    int xcol = tm->paramColumn(xp) ;
    if ( xcol < 0 ) {
        if ( _varMap.contains(xp) ) {
            foreach ( QString xval, _varMap.value(xp) ) {
                xcol = tm->paramColumn(xval);
                if ( xcol >= 0 ) {
                    break;
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

    return new TrickCurveModel(tm,tcol,xcol,ycol,yparam);
}
