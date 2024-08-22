#include "curvemodelparameter.h"

CurveModelParameter::CurveModelParameter() :
    _name(""),
    _unit(""),
    _bias(0.0),
    _scale(1.0)
{
}

QString CurveModelParameter::name() const
{
    return _name;
}

QString CurveModelParameter::unit() const
{
    return _unit;
}

double CurveModelParameter::bias() const
{
    return _bias;
}

double CurveModelParameter::scale() const
{
    return _scale;
}

void CurveModelParameter::setName(const QString& name )
{
    _name = name;
}

void CurveModelParameter::setUnit(const QString& unit )
{
    if ( _unit.isEmpty() || _unit == "--" || _unit == "1" ) {
        _unit = unit;
    } else if ( unit == _unit ) {
        // Do nothing
    } else {
        fprintf(stderr,"koviz [error]: Attempting to set the unit for "
                       "var=%s, but the internal units for this variable "
                       "are %s.  One may only override the units for "
                       "a variable when the units are unset or "
                       "unitless e.g. {--}.\n",
                _name.toLatin1().constData(),
                _unit.toLatin1().constData());
        exit(-1);
    }
}

void CurveModelParameter::setBias(double bias)
{
    _bias = bias;
}

void CurveModelParameter::setScale(double scale)
{
    _scale = scale;
}
