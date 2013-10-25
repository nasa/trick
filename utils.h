#ifndef UTILS_H
#define UTILS_H

#include <QVariant>

long round_10(long a);
int getIndexAtTime( int ntimestamps, double* timestamps, double time);

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
