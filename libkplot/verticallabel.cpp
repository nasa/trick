#include "verticallabel.h"

VerticalLabel::VerticalLabel(QWidget *parent) :
    QLabel(parent)
{
}

void VerticalLabel::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.translate( width()/2, height()/2 );
    painter.rotate(270);
    painter.drawText(0,0, text());
}

QSize VerticalLabel::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    return QSize(s.height(), s.width());
}

QSize VerticalLabel::sizeHint() const
{
    QSize s = QLabel::sizeHint();
    return QSize(s.height(), s.width());
}
