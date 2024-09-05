#include "timeinput.h"

TimeInput::TimeInput(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameStyle(QFrame::NoFrame);

    _startTimeInput = new QLineEdit(this);
    _startTimeInput->setAlignment(Qt::AlignRight);
#if QT_VERSION >= QT_VERSION_CHECK(4,8,0)
    _startTimeInput->setPlaceholderText("Start Time");
    _startTimeInput->setValidator(&_startValidator);
#endif
    _liveTimeInput = new TimeInputLineEdit(this);
    _liveTimeInput->setAlignment(Qt::AlignRight);
#if QT_VERSION >= QT_VERSION_CHECK(4,8,0)
    _liveTimeInput->setPlaceholderText("Live Time");
    _liveTimeInput->setValidator(&_liveValidator);
#endif
    _stopTimeInput = new QLineEdit(this);
    _stopTimeInput->setAlignment(Qt::AlignRight);
#if QT_VERSION >= QT_VERSION_CHECK(4,8,0)
    _stopTimeInput->setPlaceholderText("Stop Time");
    _stopTimeInput->setValidator(&_stopValidator);
#endif

    connect(_startTimeInput, SIGNAL(editingFinished()),
            this, SLOT(_slotStartTimeChanged()));
    connect(_liveTimeInput, SIGNAL(editingFinished()),
            this, SLOT(_slotLiveTimeChanged()));
    connect(_liveTimeInput, SIGNAL(nextTime()),
            this, SLOT(_slotLiveTimeNext()));
    connect(_liveTimeInput, SIGNAL(prevTime()),
            this, SLOT(_slotLiveTimePrev()));
    connect(_stopTimeInput, SIGNAL(editingFinished()),
            this, SLOT(_slotStopTimeChanged()));

     _layout = new QHBoxLayout(this);
     _layout->addWidget(_startTimeInput);
     _layout->addWidget(_liveTimeInput);
     _layout->addWidget(_stopTimeInput);
}

QSize TimeInput::sizeHint() const
{
    QSize sz(_startTimeInput->sizeHint());
    return sz;
}

void TimeInput::setLiveTime(double t)
{
    QString s = QString("%1").arg(t);
    _liveTimeInput->setText(s);
}

void TimeInput::showLiveTime()
{
    _liveTimeInput->show();
}

void TimeInput::hideLiveTime()
{
    _liveTimeInput->hide();
}

void TimeInput::_slotStartTimeChanged()
{
    double min = _startTimeInput->text().toDouble();
    emit startTimeChanged(min);
    if ( min < -1.7e+308 ) {
        _startTimeInput->clear();
    }
}

void TimeInput::_slotLiveTimeChanged()
{
    double liveTime = _liveTimeInput->text().toDouble();
    emit liveTimeChanged(liveTime);
}

void TimeInput::_slotLiveTimeNext()
{
    emit liveTimeNext();
}

void TimeInput::_slotLiveTimePrev()
{
    emit liveTimePrev();
}

void TimeInput::_slotStopTimeChanged()
{
    double max = _stopTimeInput->text().toDouble();
    emit stopTimeChanged(max);
    if ( max > 1.7e+308 ) {
        _stopTimeInput->clear();
    }
}

void TimeInput::_slotDataChanged(const QModelIndex &topLeft,
                                  const QModelIndex &bottomRight,
                                  const QVector<int> &roles)
{
    Q_UNUSED(roles);
    if ( topLeft != bottomRight ) return;
    const QAbstractItemModel* model = topLeft.model();
    if ( !model ) return;
    if ( topLeft.column() != 1 ) return;

    QString tag = model->data(topLeft.sibling(topLeft.row(),0)).toString();
    if ( tag == "LiveCoordTime" ) {
        bool ok;
        double liveTime = model->data(topLeft).toDouble(&ok);
        if ( ok ) {
            _liveTimeInput->setText(_format(liveTime));
        }
    }
}

// Taken from CurvesView
QString TimeInput::_format(double d)
{
    QString s;
    s = s.asprintf("%.9g",d);
    QVariant v(s);
    double x = v.toDouble();
    double e = qAbs(x-d);
    if ( e > 1.0e-9 ) {
        s = s.asprintf("%.9lf",d);
    }
    return s;
}

StartDoubleValidator::StartDoubleValidator(QObject *parent) :
    QDoubleValidator(parent)
{
}

void StartDoubleValidator::fixup(QString &input) const
{
    if ( input.isEmpty() ) {
        input = QString("%1").arg(-DBL_MAX);
    }
}

StopDoubleValidator::StopDoubleValidator(QObject *parent) :
    QDoubleValidator(parent)
{
}

void StopDoubleValidator::fixup(QString &input) const
{
    if ( input.isEmpty() ) {
        input = QString("%1").arg(DBL_MAX);
    }
}

TimeInputLineEdit::TimeInputLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void TimeInputLineEdit::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers keymods = event->modifiers();
    if ( event->key() == Qt::Key_Right && (keymods & Qt::AltModifier) ) {
        emit nextTime();
    } else if ( event->key() == Qt::Key_Left && (keymods & Qt::AltModifier) ) {
        emit prevTime();
    }
    QLineEdit::keyPressEvent(event);
}
