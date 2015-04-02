#ifndef PLOTBOOKMODEL_H
#define PLOTBOOKMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>

class PlotBookModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PlotBookModel(QObject *parent = 0);
    explicit PlotBookModel( int rows, int columns, QObject * parent = 0 );
    QModelIndexList pageIdxs() const ;
    QModelIndexList plotIdxs(const QModelIndex& pageIdx) const ;
    bool isPageIdx(const QModelIndex& idx) const ;

    QModelIndex sessionStartIdx() const ;
    QModelIndex sessionStopIdx() const ;

signals:
    
public slots:

private:
    void _initModel();
    
};

#endif // PLOTBOOKMODEL_H
