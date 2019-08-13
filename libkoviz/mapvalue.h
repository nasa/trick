#ifndef MAPVALUE_H
#define MAPVALUE_H

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <stdlib.h>
#include <stdio.h>

class MapValue
{
public:
    MapValue(const QString& strIn);
    QString name() const ;
    QString unit() const;
    double bias() const ;
    double scale() const ;

private:
    QString _name;
    QString _unit;
    double _bias;
    double _scale;
};

#endif // MAPVALUE_H
