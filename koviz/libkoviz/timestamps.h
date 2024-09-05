#ifndef TIMESTAMPS_H
#define TIMESTAMPS_H

#include <QList>

class TimeStamps
{
public:
    static int idxAtTime(const QList<double>& list, double time);
    static void insert(double t, QList<double> &list);

    static void usort(QList<double>& list);
    static QList<double> merge(const QList<double>& listA,
                               const QList<double>& listB);

    static double epsilon;

private:
    TimeStamps() {}
    static int _lastIdx ;
    static int _idxAtTimeBinarySearch(const QList<double>& list,
                                      int low, int high, double time);
};

#endif // TIMESTAMPS_H
