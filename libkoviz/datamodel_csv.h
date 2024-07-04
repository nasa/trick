#ifndef CSV_MODEL_H
#define CSV_MODEL_H

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
#include "timeit_linux.h"

class CsvModel;
class CsvModelIterator;

class CsvModel : public DataModel
{
  Q_OBJECT

  friend class CsvModelIterator;

  public:

    explicit CsvModel(const QStringList &timeNames,
                      const QString &runPath,
                      const QString &csvfile,
                      QObject *parent = 0);
    ~CsvModel();

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

    static bool isValid( const QString& csvFile,
                         const QStringList& timeNames);

  private:

    QStringList _timeNames;
    QString _csvfile;

    int _nrows;
    int _ncols;
    int _timeCol;

    QHash<int,Parameter*> _col2param;
    QHash<QString,int> _paramName2col;
    CsvModelIterator* _iteratorTimeIndex;

    double* _data;

    static QString _err_string;
    static QTextStream _err_stream;

    void _init();
    int _idxAtTimeBinarySearch (CsvModelIterator *it,
                               int low, int high, double time);

    inline double _convert(const QString& s);
};

class CsvModelIterator : public ModelIterator
{
  public:

    inline CsvModelIterator(): i(0) {}

    inline CsvModelIterator(int row, // iterator pos
                            const CsvModel* model,
                            int tcol, int xcol, int ycol):
        i(row),
        _model(model),
        _tcol(tcol), _xcol(xcol), _ycol(ycol)
    {
    }

    virtual ~CsvModelIterator() {}

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

    virtual CsvModelIterator* at(int n)
    {
        i = n;
        return this;
    }

    inline double t() const
    {
        return _model->_data[i*_model->_ncols+_tcol];
    }

    inline double x() const
    {
        return _model->_data[i*_model->_ncols+_xcol];
    }

    inline double y() const
    {
        return _model->_data[i*_model->_ncols+_ycol];
    }

  private:

    int i;
    const CsvModel* _model;
    int _tcol;
    int _xcol;
    int _ycol;
};


#endif // CSVMODEL_H
