#include "utils.h"

static int _idxAtTimeBinarySearch (double* timestamps,
                                  int low, int high, double time);

long round_10(long a)
{
    long r = a;

    if ( a % 10 != 0 ) {
        for ( int ii = 1; ii < 100; ++ii) {
            r = a + ii;
            if ( r % 10 == 0 ) break;
            r = a - ii;
            if ( r % 10 == 0 ) break;
        }
    }

    return r;
}

int getIndexAtTime( int ntimestamps, double* timestamps, double time)
{
        return _idxAtTimeBinarySearch( timestamps,
                                       0, ntimestamps-1, time );
}

int _idxAtTimeBinarySearch (double* timestamps,
                            int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                return ( timestamps[high] > time ) ? high-1 : high;
        } else {
                int mid = (low + high)/2;
                if (time == timestamps[mid]) {
                        return mid;
                } else if ( time < timestamps[mid] ) {
                        return _idxAtTimeBinarySearch(timestamps,
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(timestamps,
                                                      mid+1, high, time);
                }
        }
}

void printSelection(const QString& title, const QItemSelection& sel,
                    int indentLevel)
{
    QString indent;
    for ( int i = 0; i < indentLevel; ++i) {
        indent += " ";
    }
    QString t = indent + "selection=";
    t  += title;

    if ( sel.indexes().size() == 0 ) {
        qDebug() << t.toAscii().constData() << "(null)";
    } else if ( sel.indexes().size() == 1 ) {
        QString label = stringify(sel.indexes().at(0));
        qDebug() << t.toAscii().constData() << label.toAscii().constData();
    } else {
        qDebug() << t.toAscii().constData() ;
        foreach ( QModelIndex idx, sel.indexes() ) {
            QString label = stringify(idx);
            label.prepend("    ");
            label.prepend(indent);
            qDebug() << label.toAscii().constData();
        }
    }
}

QString stringify(const QModelIndex& idx)
{
    int level = 0 ;
    QModelIndex i = idx;
    while ( i.parent().isValid() ) {
        i = i.parent();
        level++;
    }
    QString s;
    switch ( level ) {
    case 0: {
        s = "page";
        break;
    }
    case 1: {
        s = stringify(idx.parent());
        s += "plot";
        break;
    }
    case 2: {
        s = stringify(idx.parent());
        s += "curve";
        break;
    }
    default: {}// nada
    }

    s += QString("(%0,%1)").arg(idx.row()).arg(idx.column());

    return s;
}
