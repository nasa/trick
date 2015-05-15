#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>

class Parameter
{
public:
    Parameter();

    void setName(const QString& name ) { _name = name; }
    void setUnit(const QString& unit ) { _unit = unit; }
    void setType(int type) { _type = type; }
    void setSize(int size) { _size = size; }

    QString name() const { return _name; }
    QString unit() const { return _unit; }
    int     type() const { return _type; }   // Delete???
    int     size() const { return _size; }   // Delete???
private:
    QString _name;
    QString _unit;
    int     _type;
    int     _size;
};

#endif // PARAMETER_H
