#ifndef VERTICALLABEL_H
#define VERTICALLABEL_H

#include <QLabel>
#include <QPainter>

class VerticalLabel : public QLabel
{
    Q_OBJECT
public:
    explicit VerticalLabel(QWidget *parent = 0);

protected:
       virtual void paintEvent(QPaintEvent * e);
       virtual QSize minimumSizeHint() const;
       virtual QSize sizeHint() const;
signals:
    
public slots:
    
};

#endif // VERTICALLABEL_H
