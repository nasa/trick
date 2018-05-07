#include "timeinput.h"

TimeInput::TimeInput(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameStyle(QFrame::NoFrame);

    _startTimeInput = new QLineEdit(this);
    _startTimeInput->setAlignment(Qt::AlignRight);
#if QT_VERSION >= QT_VERSION_CHECK(4,8,0)
    _startTimeInput->setPlaceholderText("Start Time");
    _startTimeInput->setValidator(&_validator);
#endif
    _liveTimeInput = new QLineEdit(this);
    _liveTimeInput->setAlignment(Qt::AlignRight);
#if QT_VERSION >= QT_VERSION_CHECK(4,8,0)
    _liveTimeInput->setPlaceholderText("Live Time");
    _liveTimeInput->setValidator(&_validator);
#endif
    _stopTimeInput = new QLineEdit(this);
    _stopTimeInput->setAlignment(Qt::AlignRight);
#if QT_VERSION >= QT_VERSION_CHECK(4,8,0)
    _stopTimeInput->setPlaceholderText("Stop Time");
    _stopTimeInput->setValidator(&_validator);
#endif

    connect(_startTimeInput, SIGNAL(editingFinished()),
            this, SLOT(_slotStartTimeChanged()));
    connect(_liveTimeInput, SIGNAL(editingFinished()),
            this, SLOT(_slotLiveTimeChanged()));
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
}

void TimeInput::_slotLiveTimeChanged()
{
    double liveTime = _liveTimeInput->text().toDouble();
    emit liveTimeChanged(liveTime);
}

void TimeInput::_slotStopTimeChanged()
{
    double max = _stopTimeInput->text().toDouble();
    emit stopTimeChanged(max);
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
    s = s.sprintf("%.9g",d);
    QVariant v(s);
    double x = v.toDouble();
    double e = qAbs(x-d);
    if ( e > 1.0e-9 ) {
        s = s.sprintf("%.9lf",d);
    }
    return s;
}
