#ifndef SNAP_MAINWINDOW_H
#define SNAP_MAINWINDOW_H

#ifdef SNAPGUI

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QTableView>
#include <QAction>
#include <QRubberBand>

#include "snap.h"
#include "qplot/qcustomplot.h"
#include "trickdatamodel.h"

class SnapCurve : public QCPCurve
{
  public:
    SnapCurve(QCPAxis* xaxis, QCPAxis* yaxis,
              const TrickDataModel &model, int xcol, int ycol,
              const QColor& color=Qt::blue);

    double xmin() { return _xmin; }
    double xmax() { return _xmax; }
    double ymin() { return _ymin; }
    double ymax() { return _ymax; }

  private:
    double _xmin;
    double _ymin;
    double _xmax;
    double _ymax;

};

class SnapPlot : public QCustomPlot
{
  public:
    SnapPlot(QWidget* parent=0);

   //SnapCurve*    curve(int index) const;
    SnapCurve* addCurve(const TrickDataModel &model,
                       int xcol, int ycol,
                       const QColor& color=Qt::blue);
    //bool removeCurve(int index);
    //int clearCurves();
    //int curveCount() const;

    void zoomToFit();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

  private:
    QList<SnapCurve*> _curves;
    void _set_interactions();

    QPoint _origin;
    QRubberBand* _rubber_band;


};

class SnapWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SnapWindow(Snap *snap, QWidget *parent = 0);
    QTableView* tableView;

private:
    Snap* _snap;
    QGridLayout* _layout;

    void createMenu();
    QMenuBar* _menuBar;
    QMenu *_fileMenu;
    QAction *_exitAction;

    QList<QTableView*> _tvs;
    QTableView *_create_table_view(QAbstractItemModel* model,
                                   Qt::Orientation orientation);

    void _plot_set_interactions(QCustomPlot* plot);

signals:
    
public slots:
    
};

#endif // SNAPGUI

#endif // MAINWINDOW_H

