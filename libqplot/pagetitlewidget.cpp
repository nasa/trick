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
    _titleGrid->setColumnStretch(0,6);

    _title1 = new QLabel(_titleFrame);
    _title1->setTextFormat(Qt::RichText);
    _title1->setText("");
    _title1->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    _titleGrid->addWidget(_title1,0,0);

    _title2 = new QLabel(_titleFrame);
    _title2->setTextFormat(Qt::PlainText);
    _title2->setText("");
    _title2->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    _titleGrid->addWidget(_title2,1,0);

    // Default to username
    QFileInfo f(".");
    QString userName = f.owner();
    _title3 = new QLabel(_titleFrame);
    _title3->setTextFormat(Qt::PlainText);
    _title3->setText("User: " + userName);
    _title3->setAlignment(Qt::AlignRight| Qt::AlignVCenter);
    _titleGrid->addWidget(_title3,0,1);

    // Default to date
    QDate date = QDate::currentDate();
    QString fmt("Date: MMMM d, yyyy");
    QString dateStr = date.toString(fmt);
    _title4 = new QLabel(_titleFrame);
    _title4->setTextFormat(Qt::PlainText);
    _title4->setText(dateStr);
    _title4->setAlignment(Qt::AlignRight| Qt::AlignVCenter);
    _titleGrid->addWidget(_title4,1,1);

    _mainLayout->addWidget(_titleFrame);
}

void PageTitleWidget::setTitle1(const QString &title)
{
    _title1->setText("<big>" + title + "</big>");
}

void PageTitleWidget::setTitle2(const QString &title)
{
    if ( !title.isEmpty() ) {
        _title2->setText("(" + title + ")");
    }
}

void PageTitleWidget::setTitle3(const QString &title)
{
    _title3->setText(title);
}

void PageTitleWidget::setTitle4(const QString &title)
{
    _title4->setText(title);
}

QString PageTitleWidget::title2()
{
    QString title = _title2->text();
    if ( title.startsWith("(") ) {
        title = title.replace(0,1,"");
        title.chop(1);
    }
    return title;
}
