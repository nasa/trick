#ifndef PLOTCORNER_H
#define PLOTCORNER_H

#include <QWidget>
#include <QPainter>
#include "kplotmodel.h"

class PlotCorner : public QWidget
{
    Q_OBJECT
public:
    explicit PlotCorner(KPlotModel *plotModel,
                        Qt::Corner corner,
                        QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

private:
    KPlotModel* _plotModel;
    Qt::Corner _corner;

signals:

public slots:

};

#endif // PLOTCORNER_H
