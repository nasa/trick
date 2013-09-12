#ifndef TIMELINEPLOT_H
#define TIMELINEPLOT_H

#include "qplot/qcustomplot.h"
#include "timelineloader.h"
#include <QRubberBand>

class TimeLinePlot : public QCustomPlot
{
  public:
    TimeLinePlot(const QString& csv_file, QWidget* parent=0);
    ~TimeLinePlot();

    void showJob(double jid);
    bool hideJob(double jid);

    void zoomToFit(const QCPRange& xrange=QCPRange());

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

  private:
    TimeLineLoader* _timeline_loader;
    QList<QCPGraph*> _graphs;

    QPoint _origin;
    QRubberBand* _rubber_band;

    void _fitXRange();
    void _fitYRange();

    QCPGraph* _add_timeline_graph( QCustomPlot* plot,
                                  QString curvename,
                                  QColor color,
                                  TimeLineLoader* timelineloader,
                                  int tid, double jid,
                                  double y0, double y1,
                                  double beg_time, double end_time,
                                  bool is_exclude_jid=false);

    void _add_timeline_data(TimeLineLoader* timelineloader, QCPGraph *graph,
                            int tid, double jid,
                            double y0, double y1,
                            double beg_time, double end_time,
                            bool is_exclude_jid=false);

};

#endif // TIMELINEPLOT_H
