#ifndef CURVEMODELPARAMETER_H
#define CURVEMODELPARAMETER_H

#include <stdio.h>
#include <stdlib.h>
#include "parameter.h"

class CurveModelParameter : public Parameter
{
public:
    CurveModelParameter();

    virtual QString name() const;
    virtual QString unit() const;
    double bias() const;
    double scale() const;

    virtual void setName(const QString& name );
    virtual void setUnit(const QString& unit );
    void setBias(double bias);
    void setScale(double scale);

private:
    QString _name;
    QString _unit;
    double _bias;
    double _scale;
};

#endif // CURVEMODELPARAMETER_H
