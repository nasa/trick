#ifndef PLOTLAYOUT_H
#define PLOTLAYOUT_H

#include <QLayout>
#include <QLayoutItem>
#include <QWidgetItem>
#include <QWidget>
#include <QString>
#include <QHash>
#include <QList>

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
    virtual QSize sizeHint() const;
    virtual QLayoutItem *takeAt(int index);

private:
    QHash<QString,QWidget*> _name2widget;
    QList<QWidgetItem*> _items;
};

#endif // PLOTLAYOUT_H
