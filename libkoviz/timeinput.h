#ifndef TIMEINPUT_H
#define TIMEINPUT_H

#include <QFrame>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QModelIndex>
#include <qglobal.h>
#include <float.h>

// Need empty string to be valid
class StartDoubleValidator : public QDoubleValidator
{
  public:
    StartDoubleValidator(QObject *parent = 0);
    virtual void fixup(QString &input) const;
};

class StopDoubleValidator : public QDoubleValidator
{
  public:
    StopDoubleValidator(QObject *parent = 0);
    virtual void fixup(QString &input) const;
};

class TimeInput : public QFrame
{
    Q_OBJECT
public:
    explicit TimeInput(QWidget *parent = 0);
    virtual QSize sizeHint() const;
    
signals:
    void startTimeChanged(double start);
    void liveTimeChanged(double liveTime);
    void stopTimeChanged(double stop);

public slots:
    void setLiveTime(double t);
    void showLiveTime();
    void hideLiveTime();

private slots:
    void _slotStartTimeChanged();
    void _slotLiveTimeChanged();
    void _slotStopTimeChanged();
    void _slotDataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight,
                          const QVector<int> &roles = QVector<int>());

private:
    QHBoxLayout* _layout;
    QLineEdit* _startTimeInput;
    QLineEdit* _liveTimeInput;
    QLineEdit* _stopTimeInput;
    QString _format(double d);
#if QT_VERSION >= QT_VERSION_CHECK(4,8,0)
    StartDoubleValidator _startValidator;
    StopDoubleValidator _stopValidator;
    QDoubleValidator _liveValidator;
#endif

};

#endif // RANGEINPUT_H
