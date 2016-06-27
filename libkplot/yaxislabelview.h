#ifndef YAXISLABELVIEW_H
#define YAXISLABELVIEW_H

#include <QPainter>
#include <QString>
#include <QWheelEvent>
#include "bookidxview.h"

class YAxisLabelView : public BookIdxView
{
    Q_OBJECT
public:
    explicit YAxisLabelView(QWidget *parent = 0);

protected:
    virtual void _update();

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    void wheelEvent(QWheelEvent *e);

private:
    QString _yAxisLabelText;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);


signals:
    
public slots:
    
};

#endif // YAXISLABELVIEW_H
