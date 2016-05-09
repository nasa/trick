#ifndef PLOTTITLEVIEW_H
#define PLOTTITLEVIEW_H

#include <QAbstractItemView>
#include <QLabel>
#include <QVBoxLayout>
#include "libqplot/plotbookmodel.h"

class PlotTitleView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit PlotTitleView(QWidget *parent = 0);

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
    QPersistentModelIndex _plotIdx;
    QPersistentModelIndex _plotTitleIdx;
    QLabel* _label;
    QVBoxLayout* _vBoxLayout;

signals:

public slots:
    virtual void setRootIndex(const QModelIndex &plotIdx);

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    //virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    //virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

};

#endif // PLOTTITLEVIEW_H
