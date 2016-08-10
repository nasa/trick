#ifndef PLOTBOOKMODEL_H
#define PLOTBOOKMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>
#include "libsnapdata/montemodel.h"
#include "libkplot/utils.h"

#include <QList>
#include <QColor>

class PlotBookModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PlotBookModel( MonteModel* monteModel,
                            QObject *parent = 0);
    explicit PlotBookModel( MonteModel* monteModel,
                            int rows, int columns, QObject * parent = 0 );

    QStandardItem* addChild(QStandardItem* parentItem,
                            const QString& childTitle,
                            const QVariant &childValue=QVariant());

    bool isIndex(const QModelIndex& idx,
                  const QString& itemText) const;

    bool isChildIndex(const QModelIndex& pidx,
                      const QString &expectedParentItemText,
                      const QString& childItemText) const;

    QModelIndex getIndex(const QModelIndex& startIdx,
                        const QString& searchItemText,
                        const QString &expectedStartIdxText=QString()) const;

    QModelIndex getDataIndex(const QModelIndex& startIdx,
                          const QString& searchItemText,
                          const QString &expectedStartIdxText=QString()) const;

    QString getDataString(const QModelIndex& startIdx,
                          const QString& searchItemText,
                          const QString &expectedStartIdxText=QString()) const;

    QModelIndexList getIndexList(const QModelIndex& startIdx,
                        const QString& searchItemText,
                        const QString &expectedStartIdxText=QString()) const;

    // Convenience wrappers for getIndexList
    QModelIndexList pageIdxs() const ;
    QModelIndexList plotIdxs(const QModelIndex& pageIdx) const ;
    QModelIndexList curveIdxs(const QModelIndex& curvesIdx) const ;

    // Curve Colors
    QList<QColor> createCurveColors(int nCurves);

signals:
    
public slots:

private:
    MonteModel* _monteModel;
    void _initModel();
    QModelIndex _pageIdx(const QModelIndex& idx) const ;
    QModelIndex _plotIdx(const QModelIndex& idx) const ;
    QModelIndex _ancestorIdx(const QModelIndex &startIdx,
                        const QString& ancestorText,
                        const QString &expectedStartIdxText=QString()) const;

};

#endif // PLOTBOOKMODEL_H
