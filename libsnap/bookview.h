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
#include <QFontDatabase>
#include <QString>
#include <QStringList>
#include <QList>
#include <QTransform>
#include <QVector2D>
#include <math.h>

#include "libsnap/bookidxview.h"
#include "bookview_page.h"
#include "libsnap/bookmodel.h"

class BookView : public BookIdxView
{
    Q_OBJECT
public:
    explicit BookView(QWidget *parent = 0);

protected:
    virtual void currentChanged(const QModelIndex& current,
                                const QModelIndex & previous );

    virtual void selectionChanged(const QItemSelection& selected,
                                  const QItemSelection& deselected );

private:
    QVBoxLayout* _mainLayout;
    QTabWidget* _nb;

    int _pageIdxToTabId(const QModelIndex& pageIdx);
    QModelIndex _tabIdToPageIdx(int tabId);

private:
    QTransform _coordToDotTransform(const QRectF &curvesRect,
                                    const QModelIndex &plotIdx);
    QString _format(double tic) const;

    void _printPage(QPainter* painter, const QModelIndex& pageIdx);
    QRect _printPageTitle(QPainter *painter, const QModelIndex& pageIdx);
    void _printPlot(const QRect& R,
                    QPainter *painter, const QModelIndex& plotIdx,
                    int q,
                    int wm0, int wm1, int wm2, int hm3, int hm4, int hm5,
                    int titleFontSize,
                    int axisLabelFontSize, int ticLabelFontSize,
                    int axisLineDotSize, int ticWidth, int ticHeight);
    void _printPlotTitle(const QRect& R,
                         QPainter *painter, const QModelIndex& plotIdx);
    void _printCurves(const QRect& R,
                      QPainter *painter, const QModelIndex& plotIdx);
    void _printCoplot(const QRect& R,
                      QPainter *painter, const QModelIndex& plotIdx);
    void _printErrorplot(const QRect& R,
                         QPainter *painter, const QModelIndex& plotIdx);
    void _printXAxisLabel(const QRect& R,
                          QPainter *painter, const QModelIndex& plotIdx);
    void _printYAxisLabel(const QRect& R,
                          QPainter *painter, const QModelIndex& plotIdx);
    void _printXTicLabels(const QRect &R, QPainter *painter,
                          const QModelIndex& plotIdx, const QRectF &curvesRect);
    void _printYTicLabels(const QRect &R, QPainter *painter,
                          const QModelIndex& plotIdx, const QRectF &curvesRect);
    void _printXTicsBottom(const QRect &R, QPainter *painter,
                           const QModelIndex& plotIdx, const QRect &curvesRect);
    void _printXTicsTop(const QRect &R, QPainter *painter,
                        const QModelIndex& plotIdx, const QRect& curvesRect);
    void _printYTicsLeft(const QRect &R, QPainter *painter,
                         const QModelIndex& plotIdx,
                         const QRect &curvesRect);
    void _printYTicsRight(const QRect &R, QPainter *painter,
                          const QModelIndex& plotIdx,
                          const QRect &curvesRect);
    void _printTopLeftCorner(const QRect &R, QPainter *painter,
                             const QModelIndex& plotIdx);
    void _printTopRightCorner(const QRect &R, QPainter *painter,
                              const QModelIndex& plotIdx);
    void _printBottomRightCorner(const QRect &R, QPainter *painter,
                                 const QModelIndex& plotIdx);
    void _printBottomLeftCorner(const QRect &R, QPainter *painter,
                                const QModelIndex& plotIdx);

signals:

public slots:
    void savePdf(const QString& fname);

protected slots:
    void _nbCloseRequested(int tabId);
    void _nbCurrentChanged(int tabId);
    void _pageViewCurrentChanged(const QModelIndex& currIdx,
                                 const QModelIndex& prevIdx);

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    virtual void rowsAboutToBeRemoved(const QModelIndex &pidx,
                                      int start, int end);
    //virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    //virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    
};

#endif // BOOKVIEW_H
