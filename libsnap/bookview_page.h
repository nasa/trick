#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include <QAbstractItemView>
#include <QGridLayout>
#include <QList>

#include "bookidxview.h"
#include "bookview_pagetitle.h"
#include "bookview_plot.h"

class PageView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PageView(QWidget *parent = 0);
    virtual void setModel(QAbstractItemModel *model);
    virtual void setRootIndex(const QModelIndex &index);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    QGridLayout* _grid;
    bool _toggleSingleView;

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
