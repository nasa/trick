#include "tricktablemodel.h"

#include <QDir>
#include "runs.h"
#include "montemodel.h"

QString TrickTableModel::_err_string;
QTextStream TrickTableModel::_err_stream(&TrickTableModel::_err_string);

// Private
TrickTableModel::TrickTableModel(QObject *parent) :
      QAbstractTableModel(parent),
      _runDir(QString()),
      _rowCount(0),
      _colCount(0)
{
}


TrickTableModel::TrickTableModel(const QStringList& timeNames,
                                 const QString& runDir,
                                 const QStringList &paramList,
                                 QObject *parent)
    : QAbstractTableModel(parent),
      _runDir(runDir),
      _rowCount(0),
      _colCount(0)
{
    _colCount = paramList.count()+1; // +1 for timestamp
    _params = paramList;
    _params.prepend(timeNames.at(0));

    // Get list of trk files in RUN
    QStringList trks = _trks(runDir);

    // Make list of trk models in RUN that contain the table params
    // Also make hash of param->trkModel
    int nVars = paramList.count();
    int cntVars = 0;
    QString timeName;
    foreach ( QString trk, trks ) {
        QString rtrk = runDir + "/" + trk;
        DataModel* trkModel = DataModel::createDataModel(timeNames,rtrk);
        trkModel->map();
        foreach ( QString paramName, paramList ) {
            int cc = trkModel->columnCount();
            for ( int i = 0; i < cc; ++i ) {
                const Parameter* param = trkModel->param(i);
                if ( timeNames.contains(param->name()) ) {
                    timeName = param->name();
                    continue;
                }
                if ( param->name() == paramName ) {
                    if ( _param2model.contains(param->name()) ) continue;
                    if ( !_trkModels.contains(trkModel) ) {
                        _trkModels << trkModel;
                    }
                    _param2model.insert(param->name(),trkModel);
                    ++cntVars;
                    if ( cntVars == nVars ) break;
                }
            }
            if ( cntVars == nVars ) break;
        }
        trkModel->unmap();
        if ( cntVars == nVars ) break;
    }
    if ( cntVars != nVars ) {
        // Didn't find one of the table params in RUN/*trk*s
        // TODO: make error with bad param listed
    }

    // Make time stamps list
    foreach ( DataModel* trkModel, _trkModels ) {
        trkModel->map();
        int timeCol = trkModel->paramColumn(timeName);
        ModelIterator* it = trkModel->begin(timeCol,timeCol,timeCol);
        while ( !it->isDone() ) {
            double t = it->t();
            TimeStamps::insert(t,_timeStamps);
            it->next();
        }
        delete it;
        trkModel->unmap();
    }
}

TrickTableModel::~TrickTableModel()
{
}

QStringList TrickTableModel::_trks(const QString &runDir)
{
    QStringList trkList;

    QDir dir(runDir);

    QStringList filter_trk;
    filter_trk << "*.trk";
    trkList = dir.entryList(filter_trk, QDir::Files);
    if ( trkList.empty() ) {
        _err_stream << "koviz [error]: no trk logfiles found in "
                    << runDir << "\n";
        throw std::invalid_argument(_err_string.toLatin1().constData());
    }

    return trkList;
}

int TrickTableModel::rowCount(const QModelIndex &pidx) const
{
    return ( (!pidx.isValid()) ? _timeStamps.size() : 0 );
}

int TrickTableModel::columnCount(const QModelIndex &pidx) const
{
    return ( (!pidx.isValid()) ? _colCount : 0 );
}

QVariant TrickTableModel::data(const QModelIndex &idx, int role) const
{
    //Q_UNUSED(role);
    QVariant v;
    if ( _timeStamps.isEmpty() ) return v;

    if ( role == Qt::DisplayRole ) {
        if ( idx.column() == 0 ) {
            int r = idx.row();
            int rc = _timeStamps.size();
            if ( r >= 0 && r < rc ) {
                v = _timeStamps.at(r);
            }
        } else {
            QString param = _params.at(idx.column());
            DataModel* trkModel = _param2model.value(param);
            if ( trkModel ) {
                trkModel->map();
                int r = idx.row();
                double t = _timeStamps.at(r);
                int i = trkModel->indexAtTime(t);
                int tmCol = trkModel->paramColumn(param);
                if ( tmCol >= 0 ) {
                    QModelIndex tmIdx = trkModel->index(i,tmCol);
                    v = trkModel->data(tmIdx);
                }
                trkModel->unmap();
            }
        }
    }
    return v;
}

QVariant TrickTableModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const
{
    Q_UNUSED(role);

    QVariant v;
    if ( orientation == Qt::Horizontal ) {
        v = _params.at(section);
    }
    return v;
}
