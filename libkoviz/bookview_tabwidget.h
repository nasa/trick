#ifndef BOOKVIEW_TABWIDGET_H
#define BOOKVIEW_TABWIDGET_H

#include <QTabWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMessageBox>

#include "bookmodel.h"

class BookViewTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit BookViewTabWidget(QWidget *parent = 0);
    void setBookModel(PlotBookModel* bookModel);

protected:
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);

private:
    PlotBookModel* _bookModel;

protected slots:

};

#endif // BOOKVIEW_TABWIDGET_H
