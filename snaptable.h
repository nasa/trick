#ifndef SNAPTABLE_H
#define SNAPTABLE_H

#include <QAbstractTableModel>

class Role
{
  public:
    Role() :
        format(QString("")),
        alignment(Qt::AlignRight | Qt::AlignVCenter)
    {}

    QString format;
    int alignment;

    enum Roles
    {
        Format = Qt::UserRole+0,
    };

    QVariant value(int role)
    {
        QVariant ret;

        switch (role)
        {
        case Qt::TextAlignmentRole: ret = QVariant(alignment); break;
        default: break;
        };

        return ret;
    }

    bool setValue(int role, QVariant val)
    {
        bool ret = false;

        switch (role)
        {
        case Format: format = val.toString(); ret = true; break;
        case Qt::TextAlignmentRole: alignment = val.toInt(); ret = true; break;
        default: break;
        };

        return ret;
    }
};

class SnapTable : public QAbstractTableModel
{
  Q_OBJECT

  public:

    // leaning toward QSqlTableModel
    QString  tableName () const { return _tableName ; }
    void  setTableName (const QString& name) { _tableName = name ; }
    Qt::Orientation orientation() { return _orientation ; }

  public:

    explicit SnapTable(const QString &tableName, QObject *parent = 0);
    ~SnapTable();

    QModelIndex index(int row, int column,
                      const QModelIndex &pidx = QModelIndex()) const;

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

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeaderData(int sect, Qt::Orientation orientation,
                       const QVariant &val, int role=Qt::EditRole);

  private:

    Qt::Orientation _orientation;  // for meta data e.g. format
    QString _tableName;
    QList<QList<QVariant*>* > _data;
    QList<QVariant*> _col_headers;
    QList<QVariant*> _row_headers;
    QList<Role*> _row_roles;
    QList<Role*> _col_roles;
    
signals:
    
public slots:
    
};

#endif // SNAPTABLE_H
