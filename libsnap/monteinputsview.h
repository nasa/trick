#ifndef MONTEINPUTSVIEW_H
#define MONTEINPUTSVIEW_H

#include <QTableView>
#include <libsnap/bookview.h>

class MonteInputsView : public QTableView
{
    Q_OBJECT
public:
    explicit MonteInputsView(BookView* bookView, QWidget *parent = 0);
    virtual void setModel(QAbstractItemModel *model);
    int currSelectedRun();

private:
    BookView* _bookView;
    QItemSelectionModel* _selectModel;

signals:
    
public slots:

private slots:
     void _selectModelCurrentChanged(const QModelIndex& curr,
                                     const QModelIndex& prev);
     void _viewHeaderSectionClicked(int section);
};

#endif // MONTEINPUTSVIEW_H
