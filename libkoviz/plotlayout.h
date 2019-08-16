#ifndef PLOTLAYOUT_H
#define PLOTLAYOUT_H

#include <QLayout>
#include <QLayoutItem>
#include <QWidgetItem>
#include <QWidget>
#include <QString>
#include <QHash>
#include <QList>

#include <stdio.h>

class PlotLayout : public QLayout
{
public:
    PlotLayout();
    ~PlotLayout();

    virtual void addItem(QLayoutItem *item);
    virtual void addWidget(QWidget *widget);
    virtual int count() const;
    virtual QLayoutItem *itemAt(int index) const;
    virtual void setGeometry(const QRect &rect);
    virtual QRect geometry() const;
    virtual QSize sizeHint() const;
    virtual QLayoutItem *takeAt(int index);
    virtual QLayout* layout() ;

    void setPlotRatio(const QString& plotRatio);
    QString plotRatio() const ;

private:
    QHash<QString,QLayoutItem*> _name2item;
    QList<QLayoutItem*> _items;
    QRect _rect;
    QString _plotRatio;
};

#endif // PLOTLAYOUT_H
