#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDate>
#include <QFileInfo>

// Below is the placement of the titles
//
// |          Title 1                     Title 3|
// |         (Title 2)                    Title 4|
//
//

class PageTitleWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PageTitleWidget(QWidget *parent = 0);
    void setTitle1(const QString& title);
    void setTitle2(const QString& title);
    void setTitle3(const QString& title);
    void setTitle4(const QString& title);

signals:
    
public slots:

private:
    QVBoxLayout* _mainLayout;
    QFrame* _titleFrame;
    QGridLayout* _titleGrid;
    QLabel* _title1;
    QLabel* _title2;
    QLabel* _title3;
    QLabel* _title4;
};

#endif // PAGEWIDGET_H
