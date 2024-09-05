#ifndef MONTEINPUTSVIEW_H
#define MONTEINPUTSVIEW_H

#include <QTableView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>

#include "runs.h"

class MonteInputsView : public QTableView
{
    Q_OBJECT
public:
    explicit MonteInputsView(Runs *runs, QWidget *parent = 0);
    virtual void setModel(QAbstractItemModel *model);
    int currentRun();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent* event);

private:
    Runs* _runs;
    QItemSelectionModel* _selectModel;

signals:
    
public slots:

private slots:
     void _selectModelCurrentChanged(const QModelIndex& curr,
                                     const QModelIndex& prev);
     void _viewHeaderSectionClicked(int section);
};

#endif // MONTEINPUTSVIEW_H
