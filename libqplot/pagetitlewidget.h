#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

class PageTitleWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PageTitleWidget(QWidget *parent = 0);
    void setTitle(const QString& title);
    
signals:
    
public slots:

private:
    QVBoxLayout* _mainLayout;
    QFrame* _titleFrame;
    QGridLayout* _titleGrid;
    QLabel* _label;
};

#endif // PAGEWIDGET_H
