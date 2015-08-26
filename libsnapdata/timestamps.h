#ifndef TIMESTAMPS_H
#define TIMESTAMPS_H

#include <QObject>

class TimeStamps : public QObject
{
    Q_OBJECT
public:
    explicit TimeStamps(QList<double> freqs, double stopTime,
               QObject *parent = 0);
    double at(int i);
    int count() { return _timeStamps.count(); }

    static double leastCommonMultiple(const QList<double>& xs);
    static QList<double> uniquify(const QList<double>& list);
    static QList<double> merge(const QList<double>& listA,
                               const QList<double>& listB);

private:
    TimeStamps();
    QList<double> _freqs;
    double _stopTime;

    double _lcm;
    QList<double> _timeStampsToLCM;
    QList<double> _timeStamps;
};

#endif // TIMESTAMPS_H
