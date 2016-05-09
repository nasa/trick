#ifndef PLOTBOOKVIEW_H
#define PLOTBOOKVIEW_H

#include <QAbstractItemView>
#include <QScrollBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTableView>
#include <QHash>
#include <QGridLayout>
#include <QHBoxLayout>
#include "libsnapdata/montemodel.h"
#include "libsnapdata/unit.h"
#include "libqplot/plotbookmodel.h"
#include "libkplot/koviplot.h"
#include "libkplot/pagetitleview.h"
#include "libqplot/trickcurve.h"

class PlotBookView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit PlotBookView(PlotBookModel* plotModel,
                          const QStringList& titles,
                          QWidget *parent = 0);
    ~PlotBookView();

    virtual QRect visualRect(const QModelIndex &index) const;
    virtual void scrollTo(const QModelIndex &index,
                          ScrollHint hint = EnsureVisible);
    virtual QModelIndex indexAt(const QPoint &point) const;
    virtual void setSelectionModel(QItemSelectionModel* selectionModel);
    QModelIndex currentPageIndex();
    void setCurrentPage(const QModelIndex &pageIdx);
    bool savePdf(const QString& fileName, bool isVectorizePdf);
    void showCurveDiff(bool isShow) ;
    void replot();

private:
    explicit PlotBookView(QWidget *parent = 0);


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
    virtual QItemSelectionModel::SelectionFlags selectionCommand(
                              const QModelIndex &index,
                              const QEvent *event = 0) const;
    virtual void keyPressEvent (QKeyEvent* event);

public slots:
    void maximize(const QModelIndex & idx );
    void minimize(const QModelIndex & idx );
    void selectRun(int runId);

protected slots:
    void rowsInserted(const QModelIndex &pidx, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &pidx, int start, int end);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private slots:
    void _plotSelectModelCurrentChanged(const QModelIndex& currIdx,
                                       const QModelIndex& prevIdx);
    void _plotSelectModelSelectChanged(const QItemSelection& curr,
                          const QItemSelection& prev);
    void tabCloseRequested(int tabId);
    void tabCurrentChanged(int tabId);
    void _closeAllPlots();
    void _toggleDiffPlots();
    void doubleClick(QMouseEvent* event);
    void _slotCurveClicked(TrickCurve* curve);
    void plotKeyPress(QKeyEvent* e);

private:
    QStringList _titles;       // override for page title,subtitle etc.
    PlotBookModel* _plotModel;
    QFrame* _bookFrame;
    QGridLayout* _bookGridLayout ;

    QFrame* _buttonFrame;
    QHBoxLayout* _buttonLayout;
    QPushButton* _buttonCloseAll;
    QPushButton* _buttonToggleDiff;

    QTabWidget* _nb;
    int _currSelectedRun;
    bool _isShowCurveDiff;

    QVector<QWidget*> _pages;
    QHash<QWidget*,QGridLayout*> _page2grid;
    QHash<QWidget*,PageTitleView*> _page2pagewidget;
    QHash<QWidget*,QVector<KoviPlot*> >  _page2Plots;
    QHash<QWidget*,double> _page2startTime;
    QHash<QWidget*,double> _page2stopTime;
    QHash<KoviPlot*, QVector<TrickCurve*> > _plot2Curves;

    QVector<QTableView*> _tables;
    QModelIndex _table2Idx(QWidget* tableView) const;

    QHash<QWidget*,int> _widget2notebookTab;
    void _nbTabAboutToBeRemoved(int tabId);

    inline QWidget* _idx2Page(const QModelIndex& idx) const;
    inline QGridLayout* _idx2Grid(const QModelIndex& idx) const;
    inline KoviPlot* _idx2Plot(const QModelIndex& idx) const;
    inline TrickCurve* _idx2Curve(const QModelIndex& idx) const;
    inline QModelIndex _curve2Idx(TrickCurve* curve);
    inline QModelIndex _page2Idx(QWidget* page) const;
    inline QModelIndex _plot2Idx(KoviPlot* plot) const;

    void _selectNextCurve();
    void _selectPrevCurve();

    QString _appendUnitToLabel(const QString &labelInput,
                                 const QString &unit ) const;

    void _layoutPdfPlots(const QVector<KoviPlot*>& plots);
    bool _savePdfVectorized(const QString& fileName);
    bool _savePdfPixmapped(const QString& fileName);

    void _insertPage(const QModelIndex &pageIdx, const QString &dpFileName);
    void _insertPageTitle(QWidget* page, const QString& title);
    void _insertPlot(const QModelIndex& plotIdx, QWidget* page);

    inline void _setTableData(QStandardItemModel* table, int row, int col,
                              double val,
                              double sf, double bias,
                              double minRange, double maxRange);

};

#endif // PLOTBOOKVIEW_H
