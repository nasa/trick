#include "programmodel.h"

QString ProgramModel::_err_string;
QTextStream ProgramModel::_err_stream(&ProgramModel::_err_string);

ProgramModel::ProgramModel(const QList<CurveModel *> &inputCurves,
                           const QList<Parameter> &inputParams,
                           const QStringList& outputNames,
                           const QStringList& timeNames,
                           const QString& programfile,
                           QObject *parent) :
    DataModel(timeNames, programfile, programfile, parent),
    _timeNames(timeNames),_programfile(programfile),
    _nrows(0), _ncols(0),_iteratorTimeIndex(0),
    _data(0), _library(0)
{
    _init(inputCurves,inputParams,outputNames);
}

void ProgramModel::_init(const QList<CurveModel*>& inputCurves,
                         const QList<Parameter> &inputParams,
                         const QStringList& outputNames)
{
    const int nOutputs = outputNames.size();
    const int nInputs = inputCurves.size();

    // Load external shared library program
    _library = new QLibrary(_programfile);
    _library->load();
    if ( !_library->isLoaded() ) {
        fprintf(stderr, "koviz [error]: Could not load external program "
                        "\"%s\"\n",_programfile.toLatin1().constData());
        exit(-1);
    }
    _program = (ExternalProgram) _library->resolve("kovizProgram");
    if ( !_program ) {
        fprintf(stderr, "koviz [error]: Could not find symbol "
                        "\"kovizProgram\" in %s\n",
                        _library->fileName().toLatin1().constData());
        exit(-1);
    }

    QString timeName;
    bool ok = false;
    foreach ( CurveModel* curveModel, inputCurves ) {
        if ( curveModel->t()->unit() != "s" ) {
            fprintf(stderr, "koviz [error]: ProgramModel cannot handle input "
                    "curve where time units are not seconds.\n");
            exit(-1);
        }
        if ( timeName.isEmpty() ) {
            timeName = curveModel->t()->name();
            ok = true;
        } else {
            if ( curveModel->t()->name() != timeName ) {
                ok = false;
                break;
            }
        }
    }
    if ( !ok || timeName.isEmpty() ) {
        fprintf(stderr, "koviz [error]: ProgramModel cannot handle input "
                "curves where input curve time names are different "
                "or empty.\n");
        exit(-1);
    }

    // Time param inserted in any case
    Parameter* param = new Parameter;
    param->setName(timeName);
    param->setUnit("s");
    int col = 0;
    _col2param.insert(col,param);
    _paramName2col.insert(timeName,col);
    _timeCol = col;
    ++col;

    // Make param foreach output
    foreach ( QString name, outputNames ) {
        Parameter* param = new Parameter(name);
        _col2param.insert(col,param);
        _paramName2col.insert(param->name(),col);
        ++col;
    }

    _ncols = col;

    _iteratorTimeIndex = new ProgramModelIterator(0,this,
                                                  _timeCol,_timeCol,_timeCol);

    // Get number of data rows in program file
    foreach ( CurveModel* curveModel, inputCurves ) {
        curveModel->map();
        ModelIterator* it = curveModel->begin();

        int i = 0;
        while ( !it->isDone() ) {

            double t = it->t();

            if ( i >= _timeStamps.size() ) {
                _timeStamps.append(t);
                it->next();
                ++i;
                continue;
            }

            double timeStamp = _timeStamps.at(i);
            if ( t == timeStamp ) {
                it->next();
            } else if ( t < timeStamp ) {
                _timeStamps.insert(i,t);
                it->next();
            }
            ++i;
        }

        delete it;
        curveModel->unmap();
    }
    _nrows = _timeStamps.size();

    // Allocate to hold *all* parsed data
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    // Copy timestamps to _data (column 0)
    int row = 0;
    foreach (double timeStamp, _timeStamps ) {
        _data[row*_ncols] = timeStamp;
        ++row;
    }

    // Load input curve data into an array
    double* input_data = (double*)malloc(_nrows*nInputs*sizeof(double));
    row = 0;
    col = 0;
    foreach ( CurveModel* curveModel, inputCurves ) {
        Parameter inputParam = inputParams.at(col);
        double sf = 1.0;
        double bias = 0.0;
        if ( inputParam.unit() != "--" ) {
            if ( !Unit::canConvert(curveModel->y()->unit(),inputParam.unit())) {
                fprintf(stderr, "koviz [error]: DP program input cannot be "
                                "converted from logged units:\n"
                                "    DPProgramInputName=%s\n"
                                "    DPProgramInputUnit=%s\n"
                                "    LoggedUnit=%s\n",
                        inputParam.name().toLatin1().constData(),
                        inputParam.unit().toLatin1().constData(),
                        curveModel->y()->unit().toLatin1().constData());
                exit(-1);
            }
            sf = Unit::scale(curveModel->y()->unit(), inputParam.unit());
            bias = Unit::bias(curveModel->y()->unit(), inputParam.unit());
        }
        curveModel->map();
        ModelIterator* it = curveModel->begin();
        row = 0;
        while ( !it->isDone() ) {

            double timeStamp = _data[row*_ncols];

            double t = it->t();

            if ( t == timeStamp ) {
                input_data[row*nInputs+col] = it->y()*sf+bias;
            } else if ( timeStamp < t ) {
                // Interpolate
            } else {
                fprintf(stderr, "koviz [bad scoobs]: ProgramModel::_init() "
                                "time stamp weirdness... abort!!!"
                                "t=%g timeStamp=%g row=%d\n",
                                t,timeStamp,row);
                exit(-1);
            }

            it->next();
            ++row;
        }

        delete it;
        curveModel->unmap();
        ++col;
    }

    // Generate output curves
    double* in;
    double* out = (double*)malloc(nOutputs);
    col = 1;
    for ( int row = 0; row < _nrows; ++row ) {
        in = &input_data[row*nInputs];
        _program(in,nInputs,out,nOutputs);
        for (int i = 0; i < nOutputs; ++i ) {
            _data[row*_ncols+i+1] = out[i]; // 1 for timestamp
        }
    }
    free(out);

    free(input_data);
}

void ProgramModel::map()
{
}

void ProgramModel::unmap()
{
}

int ProgramModel::paramColumn(const QString &paramName) const
{
    return _paramName2col.value(paramName,-1);
}

ModelIterator *ProgramModel::begin(int tcol, int xcol, int ycol) const
{
    return new ProgramModelIterator(0,this,tcol,xcol,ycol);
}

ProgramModel::~ProgramModel()
{
    foreach ( Parameter* param, _col2param.values() ) {
        delete param;
    }
    if ( _data ) {
        free(_data);
        _data = 0;
    }

    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
        _iteratorTimeIndex = 0;
    }
}

const Parameter* ProgramModel::param(int col) const
{
    return _col2param.value(col);
}

int ProgramModel::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int ProgramModel::_idxAtTimeBinarySearch (ProgramModelIterator* it,
                                       int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                // Time not found, choose closest near high
                double t1 = it->at(high-1)->t();
                double t2 = it->at(high)->t();
                double t3 = t2;
                it = it->at(high+1);
                if ( !it->isDone() ) {
                    t3 = it->at(high+1)->t();
                }

                int i;
                if ( qAbs(time-t1) < qAbs(time-t2) ) {
                    if ( qAbs(time-t1) < qAbs(time-t3) ) {
                        i = high-1;
                    } else {
                        if ( !it->isDone() ) {
                            i = high+1;
                        } else {
                            i = high;
                        }
                    }
                } else {
                    if ( qAbs(time-t2) < qAbs(time-t3) ) {
                        i = high;
                    } else {
                        if ( !it->isDone() ) {
                            i = high+1;
                        } else {
                            i = high;
                        }
                    }
                }
                return i;
        } else {
                int mid = (low + high)/2;
                if (time == it->at(mid)->t()) {
                        return mid;
                } else if ( time < it->at(mid)->t() ) {
                        return _idxAtTimeBinarySearch(it,
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(it,
                                                      mid+1, high, time);
                }
        }
}

int ProgramModel::rowCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int ProgramModel::columnCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _ncols;
    } else {
        return 0;
    }
}


QVariant ProgramModel::data(const QModelIndex &idx, int role) const
{
    Q_UNUSED(role);
    QVariant val;

    if ( idx.isValid() && _data ) {
        int row = idx.row();
        int col = idx.column();
        val = _data[row*_ncols+col];
    }

    return val;
}
