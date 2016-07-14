#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QTabWidget>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QPrinter>
#include <QLineF>
#include <QPointF>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QStringList>
#include <QList>
#include <QTransform>
#include <QVector2D>

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

    void _printPage(QPainter* painter, const QModelIndex& pageIdx);
    QRect __printPageTitle(QPainter *painter, const QModelIndex& pageIdx);
    void __printPlot(const QRect& plotRect,
                     QPainter *painter, const QModelIndex& plotIdx);
    void __printCurves(const QRect& R,
                       QPainter *painter, const QModelIndex& plotIdx);
    void __printXAxisLabel(QPainter *painter, const QModelIndex& pageIdx);
    void __printYAxisLabel(QPainter *painter, const QModelIndex& pageIdx);

signals:

public slots:
    void savePdf(const QString& fname);

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
