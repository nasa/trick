#include "pagetitlewidget.h"

PageTitleWidget::PageTitleWidget(QWidget *parent) :
    QFrame(parent)
{
    _mainLayout = new QVBoxLayout(this);
    _titleFrame = new QFrame(this);
    _titleFrame->setFrameStyle(QFrame::NoFrame);
    //_titleFrame->setFrameStyle(QFrame::Box);
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::white);
    _titleFrame->setAutoFillBackground(true);
    _titleFrame->setPalette(Pal);

    _titleFrame->setAutoFillBackground(true);
    _titleGrid = new QGridLayout(_titleFrame);

    _label = new QLabel(_titleFrame);
    _label->setTextFormat(Qt::RichText);
    _label->setText("");
    _label->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    _titleGrid->addWidget(_label,0,0);

    _mainLayout->addWidget(_titleFrame);
}

void PageTitleWidget::setTitle(const QString &title)
{
    _label->setText("<big>" + title + "</big>");
}
