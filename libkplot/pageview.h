#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include <QAbstractItemView>
#include <QGridLayout>
#include <QList>

#include "bookidxview.h"
#include "pagetitleview.h"
#include "plotview.h"

class PageView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PageView(QWidget *parent = 0);

protected:
    virtual void _update();

private:
    QGridLayout* _gridLayout;
    PageTitleView* _titleView;
    QList<PlotView*> _plots;

signals:

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    //virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    //virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);


signals:
    
public slots:
    
};

#endif // PAGEVIEW_H
