#ifndef PAGETITLEVIEW_H
#define PAGETITLEVIEW_H

#include <QAbstractItemView>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QStandardItemModel>
#include <QDebug>

class PageTitleView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit PageTitleView(const QModelIndex& pageIdx, QWidget *parent = 0);

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
    QPersistentModelIndex _pageIdx;
    QVBoxLayout* _mainLayout;
    QGridLayout* _titleGrid;
    QFrame* _titleFrame;
    QLabel* _title1;
    QLabel* _title2;
    QLabel* _title3;
    QLabel* _title4;

signals:
    
public slots:
    
};

#endif // PAGETITLEVIEW_H
