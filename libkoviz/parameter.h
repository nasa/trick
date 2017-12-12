#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>

class Parameter
{
public:
    Parameter();

    virtual void setName(const QString& name ) { _name = name; }
    virtual void setUnit(const QString& unit ) { _unit = unit; }
    virtual QString name() const { return _name; }
    virtual QString unit() const { return _unit; }

protected:
    QString _name;
    QString _unit;
};

#endif // PARAMETER_H
