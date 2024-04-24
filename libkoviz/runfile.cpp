#include "runfile.h"

RunFile::RunFile(const QString &run,
               const QStringList &timeNames,
               const QHash<QString, QStringList> &varMap) :
    _varMap(varMap)
{
    QFileInfo fi(run);
    if ( ! fi.exists() ) {
        fprintf(stderr, "koviz [error]: RunFile couldn't find run file=%s\n",
                run.toLatin1().constData());
        exit(-1);
    }

    QStringList suffixes = {"trk","csv","mot"};
    if ( !suffixes.contains(fi.suffix()) ) {
        fprintf(stderr, "koviz [error]: RunFile=%s has unsupported suffix=%s\n",
                run.toLatin1().constData(),fi.suffix().toLatin1().constData());
        exit(-1);
    }

    QString ffile = fi.absoluteFilePath();
    _model = DataModel::createDataModel(timeNames,run,ffile);
    _model->unmap();
    int ncols = _model->columnCount();
    for ( int col = 0; col < ncols; ++col ) {
        QString param = _model->param(col)->name();
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
        _params.append(param);
    }
}

QStringList RunFile::params()
{
    return _params;
}

// Note: Since there is only a single data model with a file, all params
//       have the same DataModel (unlike a RunDir)
DataModel *RunFile::dataModel(const QString &param)
{
    DataModel* model = 0;
    if ( _params.contains(param) ) {
        model = _model;
    } else {
        // Look in varmap for param
        foreach (QString key, _varMap.keys()) {
            QStringList names;
            foreach (QString val, _varMap.value(key)) {
                MapValue mapval(val);
                names.append(mapval.name());
            }
            if ( names.contains(param) ) {
                model = _model;
                break;
            }
        }
    }

    return model;
}
