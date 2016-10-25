#include "runs.h"
#include <QDebug>

QString Runs::_err_string;
QTextStream Runs::_err_stream(&Runs::_err_string);

Runs::Runs() :
    _runs(QStringList()),
    _startTime(0),
    _stopTime(1.0e20)
{
}

Runs::Runs(const QStringList &runDirs, double startTime, double stopTime) :
    _runs(runDirs),
    _startTime(startTime),
    _stopTime(stopTime)
{
    if ( runDirs.isEmpty() ) {
        return;
    }
    _setDirs(runDirs);
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

bool Runs::_setDirs(const QStringList &dirs)
{
    bool ok = true;

    _runs = dirs;

    QStringList filter_trk;
    filter_trk << "*.trk";
    QHash<QString,int> trks;
    QHash<QString,QStringList> runToTrks;

    foreach ( QString run, _runs ) {
        if ( ! QFileInfo(run).exists() ) {
            _err_stream << "snap [error]: couldn't find run directory: "
                        << run << "\n";
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
        QDir runDir(run);
        QStringList ltrks = runDir.entryList(filter_trk, QDir::Files);
        if ( ltrks.empty() ) {
            _err_stream << "snap [error]: no *.trk files in run directory: "
                        << run << "\n";
            throw std::invalid_argument(_err_string.toLatin1().constData());
        }
        QStringList fullPathTrks;
        foreach (QString trk, ltrks) {
            QString ftrk = run + '/' + trk;
            fullPathTrks.append(ftrk);
            trks.insert(ftrk,1);
        }
        runToTrks.insert(run,fullPathTrks);
    }
#if 0
    foreach ( QString r, _runs ) {
        QStringList l = runToTrks.value(r);
        foreach ( QString t, l ) {
            qDebug() << "t=" << t;
        }
    }
#endif

    QHash<QPair<QString,QString>,TrickModel*> ptrkToModel;
    foreach (QString trk, trks.keys() ) {
        TrickModel* m = new TrickModel("sys.exec.out.time",trk,trk);
        m->unmap();
        _models.append(m);
        int ncols = m->columnCount();
        for ( int col = 0; col < ncols; ++col) {
            QString p = m->param(col).name();
            ptrkToModel.insert(qMakePair(p,trk),m);
            if ( ! _params.contains(p) ) {
                _params.append(p);
            }
        }
    }

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
            }
            _paramToModels.value(p)->append(m);
        }
    }

#if 0
    foreach ( QString p, _params ) {
        qDebug() << p;
        for ( int i = 0 ; i < _paramToModels.value(p)->size(); ++i ) {
            TrickModel* m = _paramToModels.value(p)->at(i);
            qDebug() << "    " << (void*) m;
        }
    }
#endif

    return ok;
}

QList<TrickModel *> *Runs::models(const QString &param)
{
    return _paramToModels.value(param);
}
