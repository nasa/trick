#ifndef VERTICALLABEL_H
#define VERTICALLABEL_H

#include <QLabel>
#include <QPersistentModelIndex>
#include <QPainter>
#include <QDebug>

#include "kplotmodel.h"

class VerticalLabel : public QLabel
{
    Q_OBJECT
public:
    explicit VerticalLabel(KPlotModel* kPlotModel,
                           const QModelIndex& plotIdx,
                           QWidget *parent = 0);

protected:
       virtual void paintEvent(QPaintEvent * e);
       virtual QSize minimumSizeHint() const;
       virtual QSize sizeHint() const;

private:
    KPlotModel* _kPlotModel;
    QPersistentModelIndex _plotIdx;
    QStandardItem* _yAxisItem;

signals:
    
public slots:
    void rowsInserted(const QModelIndex& pidx,
                      int start,
                      int end);
    
};

#endif // VERTICALLABEL_H
