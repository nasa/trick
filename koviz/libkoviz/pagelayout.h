#ifndef PAGELAYOUT_H
#define PAGELAYOUT_H

#include <QLayout>
#include <QWidget>
#include <QWidgetItem>
#include <QLayoutItem>
#include <QString>
#include <stdio.h>
#include <stdlib.h>

#include "bookmodel.h"

class PageLayout : public QLayout
{
public:
    PageLayout();
    ~PageLayout();

    virtual void addItem(QLayoutItem *item);
    virtual void addWidget(QWidget *widget);
    virtual int count() const;
    virtual QLayoutItem *itemAt(int index) const;
    virtual void setGeometry(const QRect &rect);
    virtual QSize sizeHint() const;
    virtual QLayoutItem *takeAt(int index);

    void setModelIndex(PlotBookModel* bookModel, const QModelIndex& pageIdx);

private:
    QList<QLayoutItem*> _items;
    PlotBookModel* _bookModel;
    QModelIndex _pageIdx;
    QRect _calcRect(int pageWidth,int pageHeight,
                    int titleHeight, const QRectF &r);

};

#endif // PAGELAYOUT_H
