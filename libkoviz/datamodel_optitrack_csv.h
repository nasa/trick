#ifndef OPTITRACK_CSV__MODEL_H
#define OPTITRACK_CSV__MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QTextStream>
#include <QProgressDialog>
#include <QFileInfo>
#include <stdexcept>

#include "datamodel.h"
#include "parameter.h"
#include "unit.h"

class OptiTrackCsvModel;
class OptiTrackCsvModelIterator;

class OptiTrackCsvModel : public DataModel
{
  Q_OBJECT

  friend class OptiTrackCsvModelIterator;

  public:

    explicit OptiTrackCsvModel(const QStringList &timeNames,
                      const QString &runPath,
                      const QString &csvFile,
                      QObject *parent = 0);
    ~OptiTrackCsvModel();

    virtual const Parameter* param(int col) const ;
    virtual void map();
    virtual void unmap();
    virtual int paramColumn(const QString& paramName) const ;
    virtual ModelIterator* begin(int tcol, int xcol, int ycol) const ;
    int indexAtTime(double time);

    virtual int rowCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex & pidx = QModelIndex() ) const;
    virtual QVariant data (const QModelIndex & index,
                           int role = Qt::DisplayRole ) const;

     static bool isValid(const QString &fileName);

  private:

    QStringList _timeNames;
    QString _csvFile;
    double _frameRate;

    int _nrows;
    int _ncols;
    int _timeCol;

    QHash<int,Parameter*> _col2param;
    QHash<QString,int> _paramName2col;
    OptiTrackCsvModelIterator* _iteratorTimeIndex;

    double* _data;

    static QString _err_string;
    static QTextStream _err_stream;

    void _init();
    int _idxAtTimeBinarySearch (OptiTrackCsvModelIterator *it,
                               int low, int high, double time);

    inline double _convert(const QString& s);
};

class OptiTrackCsvModelIterator : public ModelIterator
{
  public:

    inline OptiTrackCsvModelIterator(): i(0) {}

    inline OptiTrackCsvModelIterator(int row, // iterator pos
                            const OptiTrackCsvModel* model,
                            int tcol, int xcol, int ycol):
        i(row),
        _model(model),
        _xcol(xcol), _ycol(ycol)
    {
    }

    virtual ~OptiTrackCsvModelIterator() {}

    virtual void start()
    {
        i = 0;
    }

    virtual void next()
    {
        ++i;
    }

    virtual bool isDone() const
    {
        return ( i >= _model->rowCount() ) ;
    }

    virtual OptiTrackCsvModelIterator* at(int n)
    {
        i = n;
        return this;
    }

    // Time is calculated from the data but not in _data
    inline double t() const
    {
        // 2 is the column of the device frame
        double deviceFrame = _model->_data[i*(_model->_ncols-1)+2];
        return deviceFrame/(_model->_frameRate);
    }

    inline double x() const
    {
        if ( _xcol == _model->_timeCol ) {
            return t();
        } else {
            return _model->_data[i*(_model->_ncols-1)+(_xcol-1)];
        }
    }

    inline double y() const
    {
        if ( _ycol == _model->_timeCol ) {
            return t();
        } else {
            return _model->_data[i*(_model->_ncols-1)+(_ycol-1)];
        }
    }

  private:

    int i;
    const OptiTrackCsvModel* _model;
    int _xcol;
    int _ycol;
};


#endif // OPTITRACK_CSV_MODEL_H
