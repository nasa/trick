#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QTabWidget>
#include <QVBoxLayout>
#include <QFileInfo>

#include "libkplot/bookidxview.h"
#include "libkplot/pageview.h"
#include "libqplot/plotbookmodel.h"

class BookView : public BookIdxView
{
    Q_OBJECT
public:
    explicit BookView(QWidget *parent = 0);

protected:
    virtual void _update();

protected:
    virtual void currentChanged(const QModelIndex& current,
                                const QModelIndex & previous );

    virtual void selectionChanged(const QItemSelection& selected,
                                  const QItemSelection& deselected );

private:
    QVBoxLayout* _mainLayout;
    QTabWidget* _nb;

signals:

public slots:

protected slots:
    void _nbCloseRequested(int idx);
    void _nbCurrentChanged(int idx);

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    //virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    //virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    
};

#endif // BOOKVIEW_H
