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
    static void usort(QList<double>& list);
    static QList<double> merge(const QList<double>& listA,
                               const QList<double>& listB);
    static void insert(double t, QList<double> &list);
    static int idxAtTime(const QList<double>& list, double time);

private:
    TimeStamps();
    QList<double> _freqs;
    double _stopTime;

    double _lcm;
    QList<double> _timeStampsToLCM;
    QList<double> _timeStamps;

    static int _lastIdx ;
    static int _idxAtTimeBinarySearch(const QList<double>& list,
                               int low, int high, double time);
};

#endif // TIMESTAMPS_H
