#include "parameter.h"

Parameter::Parameter() :
    _name(""),
    _unit("--")
{
}

Parameter::Parameter(const QString &str)
{
    _name = Parameter::nameFromString(str);
    _unit = Parameter::unitFromString(str);
}

Parameter::Parameter(const QString &name, const QString& unit) :
    _name(name),
    _unit(unit)
{
}

// String may have units
// If str == "position {m}"
// return "position"
QString Parameter::nameFromString(const QString &str)
{
    QString name(str);

    if ( str.contains("{") ) {
        name = str.split('{').at(0).trimmed();
    }

    return name;
}

// String may have units
// If str == "position {m}"
// return "m"
QString Parameter::unitFromString(const QString &str)
{
    QString unit("--");

    if ( str.contains("{") && str.contains("}") ) {
        // Unit is in curly braces
        int i = str.indexOf('{');
        int j = str.indexOf('}');
        if ( j > i ) {
            QString u = str.mid(i+1,j-i-1).trimmed();
            if ( !u.isEmpty() ) {
                unit = u;
            }
        }
    }

    return unit;
}
