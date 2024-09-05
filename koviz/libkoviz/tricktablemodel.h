#ifndef TRICKTABLEMODEL_H
#define TRICKTABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QTextStream>
#include "datamodel.h"
#include "timestamps.h"

class TrickTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TrickTableModel(const QStringList &timeNames,
                             const QString& runPath,
                             const QStringList& paramList,
                             QObject *parent = 0);

    virtual  ~TrickTableModel ();

    virtual int  rowCount(const QModelIndex& pidx = QModelIndex() ) const;
    virtual int  columnCount(const QModelIndex& pidx = QModelIndex()) const;
    virtual QVariant  data (const QModelIndex & idx,
                            int role = Qt::DisplayRole ) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole ) const;

signals:
    
public slots:

private:
    explicit TrickTableModel( QObject *parent = 0);

    QString _runPath;
    int _rowCount;
    int _colCount;
    QList<double> _timeStamps;

    QList<DataModel*> _trkModels;
    QStringList _params;
    QHash<QString,DataModel*> _param2model;
    QStringList _trks(const QString& runDir);


    static QString _err_string;
    static QTextStream _err_stream;

};

#endif // TRICKTABLEMODEL_H
