#ifndef UTILS_H
#define UTILS_H

#include <QVariant>
#include <QDebug>
#include <QModelIndex>
#include <QItemSelectionModel>

long round_10(long a);
int getIndexAtTime( int ntimestamps, double* timestamps, double time);
void printSelection(const QString &title,
                    const QItemSelection& sel, int indentLevel=0);
QString stringify(const QModelIndex& idx); // specific to plotModel

//
// With QAbstractItemModels' data(), setData() etc. methods you use
// QVariants.  These templates are hacks so you can pass ptrs from
// any class.inside a QVariant
//
// MyClass* ptr = new myClass;
// QVariant v = PtrToQVariant<MyClass>::convert(ptr);
// ... later ...
// MyClass* p = QVariantToPtr::convert(v);
//
template <class T> class QVariantToPtr
{
public:
    static T* convert(QVariant v)
    {
        return  (T *) v.value<void *>();
    }
};

template <class T> class PtrToQVariant
{
public:
    static QVariant convert(T* ptr)
    {
    return qVariantFromValue((void *) ptr);
    }
};
#endif // UTILS_H
