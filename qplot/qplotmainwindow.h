#ifndef QPLOTMAINWINDOW_H
#define QPLOTMAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "qcustomplot.h"

#include "../trickdatamodel.h"

namespace Ui {
class QPlotMainWindow;
}

class QPlotMainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit QPlotMainWindow(QWidget *parent = 0);
  ~QPlotMainWindow();
  void addTrickGraph(const TrickDataModel& model, int column);

private slots:
  void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
  void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
  void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
  void selectionChanged();
  void mousePress();
  void mouseWheel();
  void removeSelectedGraph();
  void removeAllGraphs();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void graphClicked(QCPAbstractPlottable *plottable);
  
private:

  Ui::QPlotMainWindow *ui;

  double _xaxis_min;
  double _xaxis_max;
  double _yaxis_min;
  double _yaxis_max;

  void _zoom_to_fit();
};

#endif 
