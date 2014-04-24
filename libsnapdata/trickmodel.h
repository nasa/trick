#ifndef TRICK_MODEL_H
#define TRICK_MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QHash>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <vector>
#include "snaptable.h"
#include "trick_types.h"
#include "roleparam.h"
using namespace std;

class TrickModel;
class TrickModelIterator;

class TrickModel : public SnapTable
{
  Q_OBJECT

  friend class TrickModelIterator;

  public:

    enum TrickVersion
    {
        TrickVersion07,
        TrickVersion10
    };

    enum Endianness
    {
        LittleEndian,
        BigEndian
    };

    explicit TrickModel(const QString &trkfile,
                       const QString &tableName=QString(),
                        double startTime=0.0,
                        double stopTime=1.0e20,
                       QObject *parent = 0);
    ~TrickModel();

    double startTime() { return _startTime; }
    double stopTime() { return _stopTime; }

    void map();
    void unmap();
    int paramColumn(const QString& param) const
    {
        return _param2column.value(param,-1);
    }
    TrickModelIterator begin(int tcol, int xcol, int ycol) const;
    TrickModelIterator end(int tcol, int xcol, int ycol) const;
    int indexAtTime(double time);

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const;
    virtual bool setData (const QModelIndex & idx,
                  const QVariant & value,
                  int role = Role::EditNoEmitDataChange );

    virtual bool insertRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());

    virtual bool removeRows(int row, int count,
                       const QModelIndex &pidx = QModelIndex());

    virtual bool insertColumns(int column, int count,
                       const QModelIndex &pidx = QModelIndex());

    virtual bool removeColumns(int column, int count, const QModelIndex &pidx
                                                               = QModelIndex());

  protected:
    bool _hasColumnRoles() { return true; }
    bool _hasRowRoles() { return false; }
    Role* _createColumnRole();

  private:

    QString _trkfile;
    QString _tableName;
    double _startTime;
    double _stopTime;

    TrickVersion _trick_version;
    enum Endianness _endianess;
    vector<int> _paramtypes;
    QHash<QString,int> _param2column;

    qint64 _nrows;
    qint64 _row_size;
    qint32 _ncols;
    QHash<int,qint64> _col2offset;

    qint64 _pos_beg_data;
    ptrdiff_t _mem;
    ptrdiff_t _data;
    int _fd;
    struct stat _fstat;

    TrickModelIterator* _iteratorTimeIndex;

    static QString _err_string;
    static QTextStream _err_stream;

    bool _load_trick_header();
    qint32 _load_binary_param(QDataStream& in, int col);
    int _idxAtTimeBinarySearch (TrickModelIterator& it,
                               int low, int high, double time);

  private:

    inline double _toDouble(ptrdiff_t addr, int paramtype) const
    {
        if ( _trick_version == TrickVersion07 ) {
            switch (paramtype) {
            case TRICK_07_DOUBLE:
            {
                return *((double*)(addr));
            }
            case TRICK_07_UNSIGNED_LONG_LONG:
            {
                return (double) *((unsigned long long*)(addr));
            }
            case TRICK_07_LONG_LONG:
            {
                return (double) *((long long*)(addr));
            }
            case TRICK_07_FLOAT:
            {
                return (double) *((float*)(addr));
            }
            case TRICK_07_INTEGER:
            {
                return (double) *((int*)(addr));
            }
            default:
            {
                fprintf(stderr,
                        "snap [error]: can't handle trick type \"%d\"\n"
                        "              Look in trick_types.h for type.\n",
                        paramtype);
                exit(-1);
            }
            }
        } else {
            switch (paramtype) {
            case TRICK_10_DOUBLE:
            {
                return *((double*)(addr));
            }
            case TRICK_10_UNSIGNED_LONG_LONG:
            {
                return (double) *((unsigned long long*)(addr));
            }
            case TRICK_10_LONG_LONG:
            {
                return (double) *((long long*)(addr));
            }
            case TRICK_10_FLOAT:
            {
                return (double) *((float*)(addr));
            }
            case TRICK_10_INTEGER:
            {
                return (double) *((int*)(addr));
            }
            default:
            {
                fprintf(stderr,
                        "snap [error]: can't handle trick type \"%d\"\n"
                        "              Look in trick_types.h for type\n.",
                        paramtype);
                exit(-1);
            }
            }
        }
    }

signals:
    
public slots:
    
};

//
// Iterates over rows for curve data triple - (time,x,y)
//
class TrickModelIterator
{
  public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;
    typedef double value_type;
    typedef const double *pointer;
    typedef const double &reference;

    inline TrickModelIterator(double xScaleFactor=1.0,double yScaleFactor=1.0) :
        i(0),
        _xScaleFactor(xScaleFactor),_yScaleFactor(yScaleFactor)
        {}

    inline TrickModelIterator(int row, // iterator pos
                              const TrickModel* model,
                              int tcol, int xcol, int ycol,
                              double xScaleFactor=1.0,
                              double yScaleFactor=1.0):
        i(row),
        _xScaleFactor(xScaleFactor),_yScaleFactor(yScaleFactor),
        _model(model),
        _row_size(model->_row_size),_data(model->_data),
        _tcol(tcol), _xcol(xcol), _ycol(ycol),
        _tco(_model->_col2offset.value(tcol)),
        _xco(_model->_col2offset.value(xcol)),
        _yco(_model->_col2offset.value(ycol)),
        _ttype(_model->_paramtypes.at(tcol)),
        _xtype(_model->_paramtypes.at(xcol)),
        _ytype(_model->_paramtypes.at(ycol))
    {
    }

    inline double t() const
    {
        return _model->_toDouble(_data+i*_row_size+_tco,_ttype);
    }

    inline double x() const
    {
        return _xScaleFactor*_model->_toDouble(_data+i*_row_size+_xco,_xtype);
    }

    inline double y() const
    {
        return _yScaleFactor*_model->_toDouble(_data+i*_row_size+_yco,_ytype);
    }

    inline TrickModelIterator& operator=(const TrickModelIterator &o)
    {
        // Don't  this->_valueScaleFactor = o._valueScaleFactor;
        // because it messes up "it=model->begin()"
        if ( this != &o ) {
            this->i = o.i;
            this->_model = o._model;
            this->_row_size = o._row_size;
            this->_data = o._data;
            this->_tcol = o._tcol;
            this->_xcol = o._xcol;
            this->_ycol = o._ycol;
            this->_tco = o._tco;
            this->_xco = o._xco;
            this->_yco = o._yco;
            this->_ttype = o._ttype;
            this->_xtype = o._xtype;
            this->_ytype = o._ytype;
        }

        return *this;
    }

    inline bool operator==(const TrickModelIterator &o) const
    {
        return (i == o.i && _tco == o._tco && _xco == o._xco && _yco == o._yco);
    }

    inline bool operator!=(const TrickModelIterator &o) const
    {
        return (i != o.i || _tco != o._tco || _xco != o._xco || _yco != o._yco);
    }

    inline TrickModelIterator &operator++() {
        i++;
        return *this;
    }

    inline TrickModelIterator operator++(int) {
        TrickModelIterator r = *this;
        i++;
        return r;
    }

    inline TrickModelIterator &operator--() {
        i--;
        return *this;
    }

    inline TrickModelIterator operator--(int) {
        TrickModelIterator r = *this;
        i--;
        return r;
    }

    //
    // TODO: I think I copied this from qmap iterator
    //       I think this is not necessary since rows
    //       are contiguous
    //       I think you can simply do something like:
    //                i += j;
    //                if ( i > _end ) i = end ;
    //                if ( i < 0 ) i = 0 ;
    //
    inline TrickModelIterator operator+(int j) const
    {
        TrickModelIterator r = *this;
        if (j > 0) {
            while (j--) {
                ++r;
            }
        } else {
            while (j++) {
                --r;
            }
        }

        return r;
    }

    inline TrickModelIterator operator-(int j) const { return operator+(-j); }
    inline TrickModelIterator &operator+=(int j) { return *this = *this + j; }
    inline TrickModelIterator &operator-=(int j) { return *this = *this - j; }

    inline TrickModelIterator &operator [](int n) {
        i = n;
        return *this;
    }

  private:

    int i;
    double _xScaleFactor;
    double _yScaleFactor;
    const TrickModel* _model;
    int _row_size;
    ptrdiff_t _data;
    int _tcol;
    int _xcol;
    int _ycol;
    qint64 _tco ;
    qint64 _xco ;
    qint64 _yco ;
    int _ttype ;
    int _xtype ;
    int _ytype ;
};


#endif // TRICKMODEL_H
