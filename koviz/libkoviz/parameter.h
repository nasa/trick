#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>
#include <QStringList>

class Parameter
{
public:
    Parameter();
    Parameter(const QString& str);
    Parameter(const QString& name, const QString& unit);
    virtual ~Parameter() {}

    virtual void setName(const QString& name ) { _name = name; }
    virtual void setUnit(const QString& unit ) { _unit = unit; }
    virtual QString name() const { return _name; }
    virtual QString unit() const { return _unit; }

    static QString nameFromString(const QString& str);
    static QString unitFromString(const QString& str);

protected:
    QString _name;
    QString _unit;
};

#endif // PARAMETER_H
