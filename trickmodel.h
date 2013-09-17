#ifndef TRICK_MODEL_H
#define TRICK_MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QFile>
#include <QDataStream>
#include <vector>
#include "snaptable.h"
#include "trickdatamodel.h"
using namespace std;

class TrickModel : public SnapTable
{
  Q_OBJECT

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

    explicit TrickModel(const QString &trkfile,
                       const QString &tableName=QString(),
                       QObject *parent = 0);
    ~TrickModel();

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool setData (const QModelIndex & idx,
                  const QVariant & value,
                  int role = Role::EditNoEmitDataChange );

    virtual bool insertRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());

    virtual bool removeRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());

    virtual bool insertColumns(int column, int count,
                       const QModelIndex &pidx = QModelIndex());

    virtual bool removeColumns(int column, int count, const QModelIndex &pidx
                                                               = QModelIndex());

  protected:
    bool _hasColumnRoles() { return true; }
    bool _hasRowRoles() { return false; }
    Role* _createColumnRole();

  private:

    QString _trkfile;
    QString _tableName;

    TrickVersion _trick_version;
    enum Endianness _endianess;
    vector<int> _paramtypes;

    int _nrows;
    int _row_size;
    qint32 _ncols;
    QMap<int,qint64> _col2offset;

    char* _mem;
    char* _data;
    int _fd;
    struct stat _fstat;

    bool _load_trick_header();
    qint32 _load_binary_param(QDataStream& in, int col);

signals:
    
public slots:
    
};

#endif // SNAPTABLE_H
