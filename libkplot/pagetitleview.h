#ifndef PAGETITLEVIEW_H
#define PAGETITLEVIEW_H

#include <QAbstractItemView>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QStandardItemModel>
#include <QDebug>

#include "bookidxview.h"

class PageTitleView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PageTitleView(QWidget *parent = 0);

protected:
    virtual void _update();

private:
    QVBoxLayout* _mainLayout;
    QGridLayout* _titleGrid;
    QFrame* _titleFrame;
    QLabel* _title1;
    QLabel* _title2;
    QLabel* _title3;
    QLabel* _title4;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);

};

#endif // PAGETITLEVIEW_H
