#ifndef SNAPTABLE_H
#define SNAPTABLE_H

#include <QAbstractTableModel>
#include <vector>
using namespace std;

#include "role.h"

class SnapTable : public QAbstractTableModel
{
  Q_OBJECT

  public:

    // leaning toward QSqlTableModel
    QString  tableName () const { return _tableName ; }
    void  setTableName (const QString& name) { _tableName = name ; }
    Qt::Orientation orientation() { return _orientation ; }

  public:

    explicit SnapTable(const QString &tableName=QString(), QObject *parent = 0);
    ~SnapTable();

    virtual QModelIndex parent(const QModelIndex & index) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &pidx = QModelIndex()) const;

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

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual bool setHeaderData(int sect, Qt::Orientation orientation,
                       const QVariant &val, int role=Role::EditNoEmitDataChange);

  protected:
    virtual bool _hasColumnRoles() { return true; }
    virtual Role* _createColumnRole();
    virtual bool _hasRowRoles() { return true; }
    virtual Role* _createRowRole();

  protected:
    vector<vector<QVariant>* > _data;
    vector<vector<QVariant>* >::iterator _idata;

  protected:

    Qt::Orientation _orientation;  // for meta data e.g. format
    QString _tableName;
    QList<QVariant*> _col_headers;
    QList<QVariant*> _row_headers;
    QList<Role*> _row_roles;
    QList<Role*> _col_roles;
    
signals:
    
public slots:
    
};

#endif // SNAPTABLE_H
