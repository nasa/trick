#ifndef PARAM_H
#define PARAM_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QAbstractItemModel>
#include <QFile>
#include <QDataStream>
#include <QPair>

class TrickDataModel : QAbstractItemModel
{
  public:

    enum TrickVersion
    {
        TrickVersion07
    };

    enum Endianness
    {
        LittleEndian,
        BigEndian
    };

    enum TrickRole
    {
        TrickRoleParamName = Qt::UserRole+0,
        TrickRoleUnit = Qt::UserRole+1
    };

    TrickDataModel(TrickVersion version = TrickVersion07,
                   Endianness endianness = LittleEndian);

    ~TrickDataModel();

    bool write(const QString &trk);

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

    bool insertColumns(int column, int count,
                       const QModelIndex &pidx = QModelIndex());

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int sect, Qt::Orientation orientation,
                       const QVariant &val, int role);

  private:


    TrickVersion _trick_version;
    enum Endianness _endianess;

    QList<QString> _name;
    QList<QString> _unit;
    QList<QList<QVariant*>* > _data;

    void _write_param_header(QDataStream &out, int col);
    void _write_param_data(QDataStream &out, int row, int col);
    void _write_qstring(const QString& str, QDataStream &out);
    QPair<qint32,qint32> _tricktype(const QVariant& val);
};

#endif // PARAM_H
