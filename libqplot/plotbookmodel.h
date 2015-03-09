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


signals:
    
public slots:
    
};

#endif // PLOTBOOKMODEL_H
