#include "rangeinput.h"
#include <QDebug>

RangeInput::RangeInput(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameStyle(QFrame::NoFrame);

    _minInput = new QLineEdit(this);
    _minInput->setAlignment(Qt::AlignRight);
    _minInput->setPlaceholderText("Start Time");
    _minInput->setValidator(&_validator);
    _maxInput = new QLineEdit(this);
    _maxInput->setAlignment(Qt::AlignRight);
    _maxInput->setPlaceholderText("Stop Time");
    _maxInput->setValidator(&_validator);

    connect(_minInput, SIGNAL(editingFinished()),
            this, SLOT(_slotMinChanged()));

    connect(_maxInput, SIGNAL(editingFinished()),
            this, SLOT(_slotMaxChanged()));

     _layout = new QHBoxLayout(this);
     _layout->addWidget(_minInput);
     _layout->addWidget(_maxInput);
}

QSize RangeInput::sizeHint() const
{
    QSize sz(_minInput->sizeHint());
    return sz;

}


void RangeInput::_slotMinChanged()
{
    double min = _minInput->text().toDouble();
    emit minChanged(min);
}

void RangeInput::_slotMaxChanged()
{
    double max = _maxInput->text().toDouble();
    emit maxChanged(max);
}
