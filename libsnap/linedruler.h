#ifndef LINEDRULER_H
#define LINEDRULER_H

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QList>
#include <QTransform>
#include <QStandardItemModel>
#include "kplotmodel.h"

class TicMark
{
public:
    explicit TicMark(double value, const QSizeF& size);
    virtual void draw(QPainter * painter);

private:
    double _value;
    QSizeF _size;

};

class LinedRuler : public QWidget
{
    Q_OBJECT
public:
    explicit LinedRuler(KPlotModel *plotModel,
                        Qt::Alignment alignment,
                        QWidget *parent = 0);
protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;


private:

    KPlotModel* _plotModel;
    Qt::Alignment _alignment;
    double _ptSize;   // Point size of pen (0 is "cosmetic", i.e. 1pt unscaled)

signals:
    
public slots:
    
};

#endif // LINEDRULER_H
