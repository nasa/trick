#ifndef TRICKDATAMODEL_H
#define TRICKDATAMODEL_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QTextStream>

// inserting into a list
#include <iostream>
#include <list>
#include <vector>
using namespace std;

#include "snaptable.h"
#include "roleparam.h"

class TrickType
{
  public:
    qint32 id;
    QString name;
    qint32 size;
};

class TrickDataModel : public SnapTable
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

    TrickDataModel(TrickVersion version = TrickVersion07,
                   Endianness endianness = LittleEndian);

    ~TrickDataModel();

    QVariant data (const QModelIndex & index, int role = Qt::DisplayRole ) const;

    bool write_log_header(const QString &log_name, const QString &rundir);
    bool write_binary_trk(const QString &log_name, const QString &rundir);

    bool load_binary_trk(const QString &log_name, const QString &rundir);

  protected:
    bool _hasColumnRoles() { return true; }
    bool _hasRowRoles() { return false; }
    Role* _createColumnRole();

  private:

    TrickVersion _trick_version;
    enum Endianness _endianess;

    QList<Param*> _params;

    void _load_binary_param(QDataStream &in, int col);
    void _write_binary_param(QDataStream &out, int col);
    void _write_binary_param_data(QDataStream &out, int row, int col);
    void _write_binary_qstring(const QString& str, QDataStream &out);
    TrickType _tricktype(int col);
};

#endif // TRICKDATAMODEL_H
