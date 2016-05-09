#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QAbstractItemView>
#include <QTabWidget>
#include <QVBoxLayout>

#include "libkplot/pageview.h"
#include "libqplot/plotbookmodel.h"

class BookView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit BookView(QWidget *parent = 0);

public:
    virtual void setModel(QAbstractItemModel *model);
    virtual QModelIndex indexAt( const QPoint& point) const;
    virtual QRect visualRect(const QModelIndex &index) const;
    virtual void scrollTo(const QModelIndex &index,
                          ScrollHint hint = EnsureVisible);

protected:
    virtual QModelIndex moveCursor(CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers);
    virtual int horizontalOffset() const;
    virtual int verticalOffset() const;
    virtual bool isIndexHidden(const QModelIndex &index) const;
    virtual void setSelection(const QRect &rect,
                              QItemSelectionModel::SelectionFlags command);
    virtual QRegion visualRegionForSelection(
                              const QItemSelection &selection) const;

private:
    QVBoxLayout* _mainLayout;
    QTabWidget* _nb;

signals:

public slots:

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    //virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    //virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    
};

#endif // BOOKVIEW_H
