#ifndef BOOKVIEW_H
#define BOOKVIEW_H

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
#include <QPolygonF>
#include <QHash>
#include <math.h>

#include "bookidxview.h"
#include "bookview_tabwidget.h"
#include "bookview_page.h"
#include "bookview_tablepage.h"
#include "bookmodel.h"
#include "pagelayout.h"
#include "bookview_plot.h"
#include "plotlayout.h"
#include "layoutitem_pagetitle.h"
#include "layoutitem_paintable.h"
#include "layoutitem_yaxislabel.h"
#include "layoutitem_plotcorner.h"
#include "layoutitem_plottics.h"
#include "layoutitem_plottitle.h"
#include "layoutitem_ticlabels.h"
#include "layoutitem_xaxislabel.h"
#include "layoutitem_curves.h"

class BookView : public BookIdxView
{
    Q_OBJECT
public:
    explicit BookView(QWidget *parent = 0);
    virtual void setModel(PlotBookModel *model);

protected:
    virtual void currentChanged(const QModelIndex& current,
                                const QModelIndex & previous );
    virtual void selectionChanged(const QItemSelection& selected,
                                  const QItemSelection& deselected );

private:
    QVBoxLayout* _mainLayout;
    BookViewTabWidget* _nb;
    int _modelIdxToTabId(const QModelIndex& idx);
    QModelIndex _tabIdToModelIdx(int tabId);

private:
    void _printPage(QPainter* painter, const QModelIndex& pageIdx);

public slots:
    void savePdf(const QString& fname);
    void saveJpg(const QString& fname);

protected slots:
    void _nbCloseRequested(int tabId);
    void _pageViewCurrentChanged(const QModelIndex& currIdx,
                                 const QModelIndex& prevIdx);
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    virtual void rowsAboutToBeRemoved(const QModelIndex &pidx,
                                      int start, int end);
};

#endif // BOOKVIEW_H
