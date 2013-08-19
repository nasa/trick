#ifndef PARAM_H
#define PARAM_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QAbstractItemModel>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QTextStream>

// inserting into a list
#include <iostream>
#include <list>
#include <vector>
using namespace std;

class Param
{
  public:
    QString name;
    QString unit;
    int     type;      // e.g. TRICK_DOUBLE
    int size;
};

class TrickType
{
  public:
    qint32 id;
    QString name;
    qint32 size;
};

class TrickDataModel : QAbstractItemModel
{
  public:
    static const int TRICK_DOUBLE;
    static const int TRICK_LONG_LONG;
    static const int TRICK_UNSIGNED_LONG_LONG;

    enum TrickVersion
    {
        TrickVersion07,
        TrickVersion10
    };

    enum Endianness
    {
        LittleEndian,
        BigEndian
    };

    enum TrickRole
    {
        ParamName      = Qt::UserRole+0,
        ParamUnit      = Qt::UserRole+1,
        ParamType      = Qt::UserRole+2,
        ParamSize      = Qt::UserRole+3
    };

    TrickDataModel(TrickVersion version = TrickVersion07,
                   Endianness endianness = LittleEndian);

    ~TrickDataModel();

    bool write_log_header(const QString &log_name, const QString &rundir);
    bool write_binary_trk(const QString &log_name, const QString &rundir);

    bool load_binary_trk(const QString &log_name, const QString &rundir);

    QModelIndex index(int row, int column,
                      const QModelIndex &pidx = QModelIndex()) const;

    QModelIndex parent(const QModelIndex & index) const;

    int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    QVariant data (const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData (const QModelIndex & idx,
                  const QVariant & value,
                  int role = Qt::EditRole );

    bool insertRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());

    bool removeRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());

    bool insertColumns(int column, int count,
                       const QModelIndex &pidx = QModelIndex());

    bool removeColumns(int column, int count, const QModelIndex &pidx
                                                               = QModelIndex());

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int sect, Qt::Orientation orientation,
                       const QVariant &val, int role);

  private:

    TrickVersion _trick_version;
    enum Endianness _endianess;

    QList<Param*> _params;
    vector<vector<QVariant>* > _data;
    vector<vector<QVariant>* >::iterator _idata;

    void _load_binary_param(QDataStream &in, int col);
    void _write_binary_param(QDataStream &out, int col);
    void _write_binary_param_data(QDataStream &out, int row, int col);
    void _write_binary_qstring(const QString& str, QDataStream &out);
    TrickType _tricktype(int col);
};

#endif // PARAM_H
