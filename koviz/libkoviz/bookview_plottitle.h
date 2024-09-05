#ifndef PLOTTITLEVIEW_H
#define PLOTTITLEVIEW_H

#include <QLabel>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QFontInfo>
#include "bookidxview.h"
#include "layoutitem_plottitle.h"

class PlotTitleView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PlotTitleView(QWidget *parent = 0);

protected:
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void paintEvent(QPaintEvent * event);

private:
    QLabel* _label;
    QVBoxLayout* _vBoxLayout;
    QPoint _mousePressPos;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);

};

#endif // PLOTTITLEVIEW_H
