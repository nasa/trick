#ifndef ROLEPARAM_H
#define ROLEPARAM_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>

#include "role.h"

class Param : public Role
{
  public:

    enum Roles
    {
        Name      = Qt::UserRole+20,
        Unit      = Qt::UserRole+21,
        Type      = Qt::UserRole+22,
        Size      = Qt::UserRole+23
    };

    Param() :
        name(QString("")),
        unit(QString("--")),
        type(0),             // void
        size(0)
    {}

    virtual ~Param() {}        // Destructor does nothing

    QString name;
    QString unit;
    int     type;      // e.g. TRICK_DOUBLE
    int     size;


    QVariant value(int role)
    {
        QVariant ret;

        switch (role)
        {
        case Name: ret = QVariant(name); break;
        case Unit: ret = QVariant(unit); break;
        case Type: ret = QVariant(type); break;
        case Size: ret = QVariant(size); break;
        default:
            break;
        };

        return ret;
    }

    bool setValue(int role, QVariant val)
    {
        bool ret = false;

        if ( val.type() == QVariant::String ) {
            switch (role)
            {
            case Name: name = val.toString(); ret = true; break;
            case Unit: unit = val.toString(); ret = true; break;
            default: break;
            };
        }
        if ( val.type() == QVariant::Int ) {
            switch (role)
            {
            case Size: size = val.toInt(); ret = true; break;
            case Type: type = val.toInt(); ret = true; break;
            default: break;
            }
        }

        return ret;
    }
};


#endif // ROLEPARAM_H
