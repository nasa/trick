/*
 *  Author: Keith Vetter
 *  Notes: I completely rewrote this in August 2017.
 */

#include "unit.h"

QHash<QPair<QString,QString>,double> Unit::_scales = Unit::_initScales();
QHash<QPair<QString,QString>,double> Unit::_biases = Unit::_initBiases();

Unit::Unit() :
    _name("--")
{
}

Unit::Unit(const QString &name) :
    _name(name)
{
}

Unit::~Unit()
{
}

QString Unit::name() const
{
    return _name;
}

void Unit::setName(const QString& name )
{
    _name = name ;
}

bool Unit::isEmpty() const
{
    return _name.isEmpty();
}

bool Unit::canConvert(const QString& from, const QString &to)
{
    bool can = false;

    QString fam1 = _family(from);
    QString fam2 = _family(to);

    if ( fam1 == fam2 ) {
        can = true;
    }

    return can;
}

bool Unit::isUnit(const QString &name)
{
    bool ok = false;

    QList<QPair<QString,QString> > pairs = _scales.keys();
    for ( int i = 0; i < pairs.size(); ++i ) {
        QPair<QString,QString> pair = pairs.at(i);
        if ( pair.second == name ) {
            ok = true;
            break;
        }
    }

    return ok;
}


double Unit::scale(const QString &from, const QString &to)
{
    if ( from == to ) {
        return 1.0;
    }

    if ( !isUnit(from) ) {
        fprintf(stderr,"koviz [error]: Attempting to convert "
                       "unsupported unit=\"%s\"\n",from.toLatin1().constData());
        exit(-1);
    }
    if ( !isUnit(to) ) {
        fprintf(stderr,"koviz [error]: Attempting to convert "
                       "unsupported unit=\"%s\"\n",to.toLatin1().constData());
        exit(-1);
    }

    QString fam1 = _family(from);
    QString fam2 = _family(to);

    if ( fam1 != fam2 ) {
        fprintf(stderr,"koviz [error]: Attempting to convert "
                       "unit=\"%s\" to unit=\"%s\"; however "
                       "these units are not in the same family.\n",
                        from.toLatin1().constData(), to.toLatin1().constData());
        exit(-1);
    }

    QString family = fam1 = fam2;
    double scale1 = _scales.value(qMakePair(family,from));
    double scale2 = _scales.value(qMakePair(family,to));
    double sf = scale1/scale2;

    /*
    fprintf(stderr,"scale factor from={%s} to={%s} is %g\n",
                    from.toLatin1().constData(),
                    to.toLatin1().constData(), sf);
    */

    return sf;
}

double Unit::bias(const QString &from, const QString &to)
{
    if ( from == to ) {
        return 0.0;
    }

    if ( !isUnit(from) ) {
        fprintf(stderr,"koviz [error]: Attempting to convert "
                       "unsupported unit=\"%s\"\n",from.toLatin1().constData());
        exit(-1);
    }
    if ( !isUnit(to) ) {
        fprintf(stderr,"koviz [error]: Attempting to convert "
                       "unsupported unit=\"%s\"\n",to.toLatin1().constData());
        exit(-1);
    }

    QString fam1 = _family(from);
    QString fam2 = _family(to);
    QString family = fam1 = fam2;

    if ( fam1 != fam2 ) {
        fprintf(stderr,"koviz [error]: Attempting to convert "
                       "unit=\"%s\" to unit=\"%s\"; however "
                       "these units are not in the same family.\n",
                        from.toLatin1().constData(), to.toLatin1().constData());
        exit(-1);
    }

    double b = 0.0;

    if ( family == "C" ) {
        double bias1 = _biases.value(qMakePair(family,from));
        double bias2 = _biases.value(qMakePair(family,to));
        double scale2 = _scales.value(qMakePair(family,to));
        b = (bias1-bias2)/scale2;
    }

    /*
    fprintf(stderr,"bias factor from={%s} to={%s} is %g\n",
                    from.toLatin1().constData(),
                    to.toLatin1().constData(), b);
    */

    return b;
}

QString Unit::next(const QString &unit)
{
    QString u;

    QString family = _family(unit);

    if ( family.isEmpty() ) {
        return u;  // return empty string if unit dne
    }

    QStringList units;
    QList<QPair<QString,QString> > pairs = _scales.keys();
    for ( int i = 0; i < pairs.size(); ++i ) {
        QPair<QString,QString> pair = pairs.at(i);
        if ( pair.first == family ) {
            units << pair.second;
        }
    }
    units = _sortUnits(units);

    for ( int i = 0; i < units.size(); ++i ) {
        if ( units.at(i) == unit ) {
            if ( i == units.size()-1 ) {
                u = units.at(0);
            } else {
                u = units.at(i+1);
            }
            break;
        }
    }

    return u;
}

QString Unit::prev(const QString &unit)
{
    QString u;

    QString family = _family(unit);

    if ( family.isEmpty() ) {
        return u;  // return empty string if unit dne
    }

    QStringList units;
    QList<QPair<QString,QString> > pairs = _scales.keys();
    for ( int i = 0; i < pairs.size(); ++i ) {
        QPair<QString,QString> pair = pairs.at(i);
        if ( pair.first == family ) {
            units << pair.second;
        }
    }
    units = _sortUnits(units);

    for ( int i = 0; i < units.size(); ++i ) {
        if ( units.at(i) == unit ) {
            if ( i == 0 ) {
                u = units.last();
            } else {
                u = units.at(i-1);
            }
            break;
        }
    }

    return u;
}

// Examples:
// map("m","in") returns "in"
// map("m/s","in") returns "in/s"
// map("N/m2","in") returns "lbf*in2"
// map("r/s","d") returns "d/s"
// map("r/s2","d") returns "d/s2"
//
// This is a helper function that works to allow a user to specify distance,
// for example, in inches and have koviz set all related units like pressure,
// velocity etc its "inches" counterpart "psi" and "in/s".
//
// This map is not extensive, but meant as a start as uses cases arise
//
// If there is no map, a common case, an empty unit is returned
Unit Unit::map(const Unit &u1, const Unit &u2)
{
    Unit u;
    u.setName("");

    QString fam1 = _family(u1.name());
    QString fam2 = _family(u2.name());
    QString name2 = u2.name();
    if ( name2 == "M" ) {
        name2 = "m";
    } else if ( name2 == "degree" ) {
        name2 = "d";
    }


    if ( fam1 == fam2 ) {
        // e.g. map(N*m,oz*in) returns oz*in
        u.setName(name2);
    } else  if ( fam2 == "m" ) {
        //
        // Distance
        //
        if ( fam1 == "m/s" ) {
            // Velocity
            QString velocity = name2 + "/s";
            u.setName(velocity);
        } else if ( fam1 == "m/s2" ) {
            // Acceleration
            QString acceleration = name2 + "/s2";
            u.setName(acceleration);
        } else if ( fam1 == "N" ) {
            // Force
            if ( name2 == "ft" || name2 == "in" ) {
                u.setName("lbf");
            } else if ( name2 == "m" ) {
                u.setName("N");
            }
        } else if ( fam1 == "N*m" ) {
            // Torque
            if ( name2 == "in" ) {
                u.setName("lbf*in");
            } else if ( name2 == "ft" ) {
                u.setName("lbf*ft");
            } else if ( name2 == "cm" ) {
                u.setName("N*cm");
            }
        } else if ( fam1 == "m2" ) {
            // Area
            if ( name2 == "m" || name2 == "cm"
                 || name2 == "ft" || name2 == "in" ) {
                QString area = name2 + "2";
                u.setName(area);
            }
        } else if ( fam1 == "m3" ) {
            // Volume
            if ( name2 == "m" || name2 == "cm"
                 || name2 == "ft" || name2 == "in" ) {
                QString volume = name2 + "3";
                u.setName(volume);
            }
        } else if ( fam1 == "m3/s" ) {
            // Volume rate
            if ( name2 == "m" || name2 == "cm"
                 || name2 == "ft" || name2 == "in" ) {
                QString rate = name2 + "3/s";
                u.setName(rate);
            }
        } else if ( fam1 == "N*m*s" ) {
            // Angular Impulse
            if ( name2 == "ft" || name2 == "in" ) {
                u.setName("lbf*ft*s");
            } else if ( name2 == "m" ) {
                u.setName("N*m*s");
            }
        } else if ( fam1 == "N/m2" ) {
            // Pressure
            if ( name2 == "in" || name2 == "ft" ) {
                u.setName("psi");
            } else if ( name2 == "m" ) {
                u.setName("N/m2");
            }
        } else if ( fam1 == "N/m" ) {
            // Stiffness
            if ( name2 == "in" || name2 == "ft" ) {
                u.setName("lbf/ft");
            } else if ( name2 == "m" ) {
                u.setName("N/m");
            }
        } else if ( fam1 == "N*s/m" ) {
            // Friction constant
            if ( name2 == "in" || name2 == "ft" ) {
                u.setName("lbf*s/ft");
            } else if ( name2 == "m" ) {
                u.setName("N*s/m");
            }
        } else if ( fam1 == "kg/m3" ) {
            // Density
            if ( name2 == "in" ) {
                u.setName("oz/in3");
            } else if ( name2 == "ft" ) {
                u.setName("lb/ft3");
            } else if ( name2 == "m" ) {
                u.setName("kg/m3");
            }
        } else if ( fam1 == "N*r/m2" ) {
            // Q-Bar-Alpha - dyn pressure and angle of attack
            if ( name2 == "ft" || name2 == "in" ) {
                u.setName("lbf*d/ft2");
            } else if ( name2 == "m" ) {
                u.setName("N*r/m2");
            }
        }
    } else if ( fam2 == "r" ) {
        //
        // Angle
        //
        if ( fam1 == "r/s" ) {
            // Angular Velocity
            if ( name2 == "r" ) {
                u.setName("r/s");
            } else if ( name2 == "d" ) {
                u.setName("d/s");
            }
        } else if ( fam1 == "r/s2" ) {
            // Angular Acceleration
            if ( name2 == "r" ) {
                u.setName("r/s2");
            } else if ( name2 == "d" ) {
                u.setName("d/s2");
            }
        } else if ( fam1 == "1/r" ) {
            // 1/angle
            if ( name2 == "r" ) {
                u.setName("1/r");
            } else if ( name2 == "d" ) {
                u.setName("1/d");
            }
        } else if ( fam1 == "N*r/m2" ) {
            // Q-Bar-Alpha - dyn pressure and angle of attack
            if ( name2 == "r" ) {
                u.setName("N*r/m2");
            } else if ( name2 == "d" ) {
                u.setName("lbf*d/ft2");
            }
        }
    }

    return u;
}

QString Unit::showUnits()
{
    QString msg;

    QHash<QString,QStringList> fam2mems;
    QList<QPair<QString,QString> > pairs = _scales.keys();
    foreach ( QString family, _families() ) {
        QStringList mems;
        for ( int i = 0; i < pairs.size(); ++i ) {
            QPair<QString,QString> pair = pairs.at(i);
            if ( pair.first == family && pair.second != family ) {
                mems << pair.second;
            }
        }
        mems.sort();
        fam2mems.insert(family,mems);
    }

    foreach ( QString family, _families() ) {
        msg += family + "\n";
        foreach ( QString mem, fam2mems.value(family) ) {
            msg += "  " + mem + "\n";
        }
    }

    return msg;
}

QString Unit::derivative(const QString &unit)
{
    QString derivUnit;

    QRegularExpression re0("/s$");
    QRegularExpression re1("/s(\\d+)$");
    QRegularExpressionMatch match0 = re0.match(unit);
    QRegularExpressionMatch match1 = re1.match(unit);
    if ( match0.hasMatch() ) {
        derivUnit = unit;
        derivUnit.replace(re0,"/s2");
    } else if ( match1.hasMatch() ) {
        bool ok;
        int n = match1.captured(1).toInt(&ok);
        if ( ok ) {
            QString s = QString("/s%1").arg(++n);
            derivUnit.replace(re1,s);
        }
    } else {
        derivUnit = unit + "/s";
    }

    if ( !Unit::isUnit(derivUnit) ) {
        derivUnit = "--";
    }

    return derivUnit;
}

QString Unit::integral(const QString &unit)
{
    QString integUnit;

    QRegularExpression re0("(\\w+)/s$");
    QRegularExpression re1("(\\w+)/s(\\d+)$");
    QRegularExpressionMatch match0 = re0.match(unit);
    QRegularExpressionMatch match1 = re1.match(unit);
    if ( match0.hasMatch() ) {
        integUnit = match0.captured(1);
    } else if ( match1.hasMatch() ) {
        bool ok;
        QString u = match1.captured(1);
        int n = match1.captured(2).toInt(&ok);
        if ( ok ) {
            if ( n == 2 ) {
                integUnit = QString("%1/s").arg(u);
            } else {
                integUnit = QString("%1/s%2").arg(u).arg(--n);
            }
        }
    } else if ( unit == "N" ) {
        integUnit = "N*s";
    } else if ( unit == "N*m" ) {
        integUnit = "N*m*s";
    } else {
        integUnit = "--";
    }

    if ( !Unit::isUnit(integUnit) ) {
        integUnit = "--";
    }

    return integUnit;
}

QHash<QPair<QString, QString>, double> Unit::_initScales()
{
    QHash<QPair<QString, QString>, double> map;

    // Time
    map.insert(QPair<QString,QString>("s","s"),       1.0);
    map.insert(QPair<QString,QString>("s","min"),    60.0);
    map.insert(QPair<QString,QString>("s","hr"),   3600.0);
    map.insert(QPair<QString,QString>("s","day"), 86400.0);
    map.insert(QPair<QString,QString>("s","ms"),      0.001);
    map.insert(QPair<QString,QString>("s","us"),      0.000001);

    // Distance
    map.insert(QPair<QString,QString>("m","m"),       1.0);
    map.insert(QPair<QString,QString>("m","M"),       1.0);
    map.insert(QPair<QString,QString>("m","ft"),      0.3048);
    map.insert(QPair<QString,QString>("m","in"),      0.0254);
    map.insert(QPair<QString,QString>("m","mm"),      0.001);
    map.insert(QPair<QString,QString>("m","cm"),      0.01);
    map.insert(QPair<QString,QString>("m","km"),   1000.0);
    map.insert(QPair<QString,QString>("m","yd"),      0.9143999999999998);
    map.insert(QPair<QString,QString>("m","mi"),   1609.344);
    map.insert(QPair<QString,QString>("m","nmi"),  1852.0);
    map.insert(QPair<QString,QString>("m","kft"),   304.8);

    // Speed
    map.insert(QPair<QString,QString>("m/s","m/s"),       1.0);
    map.insert(QPair<QString,QString>("m/s","M/s"),       1.0);
    map.insert(QPair<QString,QString>("m/s","cm/s"),      0.01);
    map.insert(QPair<QString,QString>("m/s","mm/s"),      0.001);
    map.insert(QPair<QString,QString>("m/s","in/s"),      0.0254);
    map.insert(QPair<QString,QString>("m/s","ft/s"),      0.3048);
    map.insert(QPair<QString,QString>("m/s","mph"),    1609.344/3600.0);
    map.insert(QPair<QString,QString>("m/s","km/hr"),  1000.0/3600.0);
    map.insert(QPair<QString,QString>("m/s","kmh"),    1000.0/3600.0);

    // Acceleration
    map.insert(QPair<QString,QString>("m/s2","m/s2"),       1.0);
    map.insert(QPair<QString,QString>("m/s2","M/s2"),       1.0);
    map.insert(QPair<QString,QString>("m/s2","cm/s2"),      0.01);
    map.insert(QPair<QString,QString>("m/s2","mm/s2"),      0.001);
    map.insert(QPair<QString,QString>("m/s2","in/s2"),      0.0254);
    map.insert(QPair<QString,QString>("m/s2","ft/s2"),      0.3048);

    // Angle
    map.insert(QPair<QString,QString>("r","r"),      1.0);
    map.insert(QPair<QString,QString>("r","rad"),    1.0);
    map.insert(QPair<QString,QString>("r","radian"),    1.0);
    map.insert(QPair<QString,QString>("r","d"),      0.0174532925199433);
    map.insert(QPair<QString,QString>("r","degree"), 0.0174532925199433);
    map.insert(QPair<QString,QString>("r","as"),     4.848136811095362e-06);
    map.insert(QPair<QString,QString>("r","am"),     0.0002908882086657216);
    map.insert(QPair<QString,QString>("r","rev"),    6.28318530717958647);
    map.insert(QPair<QString,QString>("r","mr"),     0.001000000000000);

    // Angular velocity
    map.insert(QPair<QString,QString>("r/s","r/s"),     1.0);
    map.insert(QPair<QString,QString>("r/s","rad/s"),   1.0);
    map.insert(QPair<QString,QString>("r/s","radian/s"),1.0);
    map.insert(QPair<QString,QString>("r/s","d/s"),      0.0174532925199433);
    map.insert(QPair<QString,QString>("r/s","degrees/s"),0.0174532925199433);
    map.insert(QPair<QString,QString>("r/s","degree/s"), 0.0174532925199433);
    map.insert(QPair<QString,QString>("r/s","rev/s"), 6.28318530717958647);
    map.insert(QPair<QString,QString>("r/s","rpm"), 6.28318530717958647/60.0);
    map.insert(QPair<QString,QString>("r/s","rev/min"),
                                                    6.28318530717958647/60.0);

    // Angular acceleration
    map.insert(QPair<QString,QString>("r/s2","r/s2"),      1.0);
    map.insert(QPair<QString,QString>("r/s2","rad/s2"),    1.0);
    map.insert(QPair<QString,QString>("r/s2","radian/s2"), 1.0);
    map.insert(QPair<QString,QString>("r/s2","d/s2"),      0.0174532925199433);
    map.insert(QPair<QString,QString>("r/s2","degree/s2"), 0.0174532925199433);
    map.insert(QPair<QString,QString>("r/s2","rev/s2"),    6.28318530717958647);

    // Mass
    map.insert(QPair<QString,QString>("kg","kg"),      1.0);
    map.insert(QPair<QString,QString>("kg","sl"),     14.59390293000000);//Trk17
    //map.insert(QPair<QString,QString>("kg","sl"),   14.5939029372064); //Trk07
    map.insert(QPair<QString,QString>("kg","lbm"),     0.4535923697760192);
    map.insert(QPair<QString,QString>("kg","lb"),      0.4535923697760192);
    map.insert(QPair<QString,QString>("kg","g"),       0.001);
    map.insert(QPair<QString,QString>("kg","mt"),   1000.0);
    map.insert(QPair<QString,QString>("kg","slinch"),175.126835246476);

    // Force
    map.insert(QPair<QString,QString>("N","N"),      1.0);
    map.insert(QPair<QString,QString>("N","kN"),  1000.0);
    map.insert(QPair<QString,QString>("N","oz"),     0.2780138509537812);
    map.insert(QPair<QString,QString>("N","lbf"),    4.4482216152605);
    map.insert(QPair<QString,QString>("N","kip"), 4448.2216152605);

    // Torque
    map.insert(QPair<QString,QString>("N*m","N*m"),      1.0);
    map.insert(QPair<QString,QString>("N*m","N*M"),      1.0);
    map.insert(QPair<QString,QString>("N*m","Nm"),       1.0);
    map.insert(QPair<QString,QString>("N*m","N.m"),      1.0);
    map.insert(QPair<QString,QString>("N*m","lbf*ft"),   1.35581795);
    map.insert(QPair<QString,QString>("N*m","kip-ft"),   1355.81795);
    map.insert(QPair<QString,QString>("N*m","lbf*in"),   4.4482216152605*0.0254);
    map.insert(QPair<QString,QString>("N*m","oz*in"),    0.00706155195);
    map.insert(QPair<QString,QString>("N*m","N*cm"),     0.01);

    // Linear Impulse
    map.insert(QPair<QString,QString>("N*s","N*s"),     1.0);
    map.insert(QPair<QString,QString>("N*s","kg*m/s"),  1.0);
    map.insert(QPair<QString,QString>("N*s","kg*M/s"),  1.0);

    // Angular Impulse
    map.insert(QPair<QString,QString>("N*m*s","N*m*s"),    1.0);
    map.insert(QPair<QString,QString>("N*m*s","N*M*s"),    1.0);
    map.insert(QPair<QString,QString>("N*m*s","kg*m2/s"),  1.0);
    map.insert(QPair<QString,QString>("N*m*s","kg*M2/s"),  1.0);
    map.insert(QPair<QString,QString>("N*m*s","lbf*ft*s"), 1.35581795);

    // Pressure
    map.insert(QPair<QString,QString>("N/m2","N/m2"),      1.0);
    map.insert(QPair<QString,QString>("N/m2","Pa"),        1.0);
    map.insert(QPair<QString,QString>("N/m2","kPa"),    1000.0);
    map.insert(QPair<QString,QString>("N/m2","psi"),     6894.75719);
    map.insert(QPair<QString,QString>("N/m2","lbf/in2"), 6894.75719);
    map.insert(QPair<QString,QString>("N/m2","oz/in2"),  430.922332);

    // Stiffness
    map.insert(QPair<QString,QString>("N/m","N/m"),      1.0);
    map.insert(QPair<QString,QString>("N/m","N/mm"),  1000.0);
    map.insert(QPair<QString,QString>("N/m","lbf/ft"),  4.4482216152605/0.3048);

    // Torsional spring stiffness
    map.insert(QPair<QString,QString>("N*m/r","N*m/r"), 1.0);
    map.insert(QPair<QString,QString>("N*m/r","N*m/d"), 1.0/0.0174532925199433);

    // Friction constant
    map.insert(QPair<QString,QString>("N*s/m","N*s/m"), 1.0);
    map.insert(QPair<QString,QString>("N*s/m","N*s/mm"), 1000.0);
    map.insert(QPair<QString,QString>("N*s/m","lbf*s/ft"), 4.4482216152605/0.3048);

    // Torsional friction
    map.insert(QPair<QString,QString>("N*m*s/r","N*m*s/r"), 1.0);
    map.insert(QPair<QString,QString>("N*m*s/r","N*cm*min/rev"), 60.0/(100.0*6.28318530717958647));
    map.insert(QPair<QString,QString>("N*m*s/r","N*m*s/d"), 1.0/0.0174532925199433);

    // Density
    map.insert(QPair<QString,QString>("kg/m3","kg/m3"),     1.0);
    map.insert(QPair<QString,QString>("kg/m3","g/cm3"),  1000.0);
    map.insert(QPair<QString,QString>("kg/m3","lb/ft3"),   16.018463374);
    map.insert(QPair<QString,QString>("kg/m3","oz/in3"), 1729.994044387);

    // Voltage
    map.insert(QPair<QString,QString>("v","v"),      1.0);
    map.insert(QPair<QString,QString>("v","V"),      1.0);
    map.insert(QPair<QString,QString>("v","kv"),  1000.0);

    // Current
    map.insert(QPair<QString,QString>("amp","amp"),   1.0);
    map.insert(QPair<QString,QString>("amp","mamp"),  0.001);
    map.insert(QPair<QString,QString>("amp","mA"),    0.001);

    // Resistance
    map.insert(QPair<QString,QString>("ohm","ohm"),   1.0);
    map.insert(QPair<QString,QString>("ohm","mohm"),  0.001);

    // Energy
    map.insert(QPair<QString,QString>("J","J"),        1.0);
    map.insert(QPair<QString,QString>("J","mJ"),       0.001);
    map.insert(QPair<QString,QString>("J","uJ"),       0.000001);
    map.insert(QPair<QString,QString>("J","kJ"),    1000.0);
    map.insert(QPair<QString,QString>("J","MJ"), 1000000.0);

    // Temperature (uses bias)
    map.insert(QPair<QString,QString>("C","C"),  1.0);
    map.insert(QPair<QString,QString>("C","K"),  1.0);
    map.insert(QPair<QString,QString>("C","R"),  5.0/9.0);
    map.insert(QPair<QString,QString>("C","F"),  5.0/9.0);
    map.insert(QPair<QString,QString>("C","degF"),  5.0/9.0);

    // Power
    map.insert(QPair<QString,QString>("W","W"),  1.0);

    // Volume
    map.insert(QPair<QString,QString>("m3","m3"),   1.0);
    map.insert(QPair<QString,QString>("m3","cm3"),  0.01*0.01*0.01);
    map.insert(QPair<QString,QString>("m3","ft3"),  0.3048*0.3048*0.3048);
    map.insert(QPair<QString,QString>("m3","in3"),  0.0254*0.0254*0.0254);

    // Volume rate
    map.insert(QPair<QString,QString>("m3/s","m3/s"),  1.0);
    map.insert(QPair<QString,QString>("m3/s","cm3/s"), 0.01*0.01*0.01);
    map.insert(QPair<QString,QString>("m3/s","ft3/s"), 0.3048*0.3048*0.3048);
    map.insert(QPair<QString,QString>("m3/s","in3/s"), 0.0254*0.0254*0.0254);

    // Mass rate
    map.insert(QPair<QString,QString>("kg/s","kg/s"),  1.0);
    map.insert(QPair<QString,QString>("kg/s","lbm/s"), 0.4535923697760192);
    map.insert(QPair<QString,QString>("kg/s","lb/s"),  0.4535923697760192);

    // Area
    map.insert(QPair<QString,QString>("m2","m2"),  1.0);
    map.insert(QPair<QString,QString>("m2","cm2"), 0.01*0.01);
    map.insert(QPair<QString,QString>("m2","ft2"), 0.3048*0.3048);
    map.insert(QPair<QString,QString>("m2","in2"), 0.0254*0.0254);

    // Unitless
    map.insert(QPair<QString,QString>("--","--"),  1.0);
    map.insert(QPair<QString,QString>("--","cnt"), 1.0);
    map.insert(QPair<QString,QString>("--","one"), 1.0);
    map.insert(QPair<QString,QString>("--","1"),   1.0);
    map.insert(QPair<QString,QString>("--","dB"),  1.0);
    map.insert(QPair<QString,QString>("--","*"),   1.0);
    map.insert(QPair<QString,QString>("--","N/A"), 1.0);

    // Micro-gravity
    map.insert(QPair<QString,QString>("micro-g","micro-g"), 1.0);
    map.insert(QPair<QString,QString>("micro-g","μg"), 1.0);

    // 1/angle
    map.insert(QPair<QString,QString>("1/r","1/r"),   1.0);
    map.insert(QPair<QString,QString>("1/r","one/r"), 1.0);
    map.insert(QPair<QString,QString>("1/r","1/d"),   1.0/0.0174532925199433);
    map.insert(QPair<QString,QString>("1/r","one/d"), 1.0/0.0174532925199433);
    map.insert(QPair<QString,QString>("1/r","one/degree"), 1.0/0.0174532925199433);

    // Q-Bar-Alpha (has to do with dynamic pressure and angle of attack)
    map.insert(QPair<QString,QString>("N*r/m2","N*r/m2"), 1.0);
    map.insert(QPair<QString,QString>("N*r/m2","N*rad/m2"), 1.0);
    map.insert(QPair<QString,QString>("N*r/m2","lbf*d/ft2"),
                            4.4482216152605*0.0174532925199433/(0.3048*0.3048));
    map.insert(QPair<QString,QString>("N*r/m2","lbf*degree/ft2"),
                            4.4482216152605*0.0174532925199433/(0.3048*0.3048));

    // Frequency
    map.insert(QPair<QString,QString>("Hz","Hz"), 1.0);
    map.insert(QPair<QString,QString>("Hz","1/s"), 1.0);

    // Miscellaneous
    map.insert(QPair<QString,QString>("rad2/s2","rad2/s2"), 1.0);
    map.insert(QPair<QString,QString>("m2/s2","m2/s2"), 1.0);
    map.insert(QPair<QString,QString>("kg*m2","kg*m2"), 1.0);
    map.insert(QPair<QString,QString>("kg*m/s2","kg*m/s2"), 1.0);
    map.insert(QPair<QString,QString>("kg*m2/s","kg*m2/s"), 1.0);
    map.insert(QPair<QString,QString>("kg*m2/s2","kg*m2/s2"), 1.0);
    map.insert(QPair<QString,QString>("m2/s3","m2/s3"), 1.0);
    map.insert(QPair<QString,QString>("N*s/m2","N*s/m2"), 1.0);
    map.insert(QPair<QString,QString>("s/s","s/s"), 1.0);
    map.insert(QPair<QString,QString>("s/s2","s/s2"), 1.0);

    return map;
}

// All biases are 0 - except temperature
QHash<QPair<QString, QString>, double> Unit::_initBiases()
{
    QHash<QPair<QString, QString>, double> map;

    // Temperature
    map.insert(QPair<QString,QString>("C","C"),    0.0);
    map.insert(QPair<QString,QString>("C","R"), -273.15);
    map.insert(QPair<QString,QString>("C","K"), -273.15);
    map.insert(QPair<QString,QString>("C","F"),    -17.777777778);
    map.insert(QPair<QString,QString>("C","degF"), -17.777777778);

    return map;
}

QString Unit::_family(const QString &name)
{
    QString family;

    QList<QPair<QString,QString> > pairs = _scales.keys();
    for ( int i = 0; i < pairs.size(); ++i ) {
        QPair<QString,QString> pair = pairs.at(i);
        if ( pair.second == name ) {
            family = pair.first;
            break;
        }
    }

    return family;
}

QStringList Unit::_sortUnits(const QStringList &unitsIn)
{
    QStringList units = unitsIn;

    units.sort();

    if ( units.contains("m") ) {
        units.clear();
        units << "mm"  << "cm" << "m" << "km"
              << "in" << "ft" << "yd" << "mi";
    } else if ( units.contains("m/s") ) {
        units.clear();
        units << "mm/s" << "cm/s" << "m/s" << "kmh"
              << "in/s" << "ft/s" << "mph" ;
    } else if ( units.contains("m/s2") ) {
        units.clear();
        units << "cm/s2" << "m/s2" << "in/s2" << "ft/s2";
    }

    return units;
}

QStringList Unit::_families()
{
    QStringList families;

    QList<QPair<QString,QString> > pairs = _scales.keys();
    for ( int i = 0; i < pairs.size(); ++i ) {
        QPair<QString,QString> pair = pairs.at(i);
        QString family = pair.first;
        if ( !families.contains(family) ) {
            families.append(family);
        }
    }

    families.sort();

    return families;
}

/*
    // TODO: make unit test with the following
    convert(1.0,"s","s");    // 1
    convert(1.0,"min","s");  // 60
    convert(1.0,"hr","min"); // 60
    convert(1.0,"day","hr"); // 24
    convert(1.0,"s","ms");   // 1000
    convert(1.0,"ms","us");  // 1000

    convert(1.0,"m","m");    // 1
    convert(1.0,"m","M");    // 1
    convert(1.0,"ft","in");  // 12
    convert(1.0,"cm","mm");  // 10
    convert(1.0,"mi","yd");  // 1760
    convert(1.0,"mi","ft");  // 5280
    convert(5.0,"km","mi");  // 3.10685595
    convert(1.0,"mi","nm");  // 0.86897624
    convert(1.0,"kft","ft"); // 1000
    convert(1.0,"ft","M"); // 0.3048

    convert(1.0,"m/s","m/s");    // 1
    convert(1.0,"in/s","cm/s");  // 2.54
    convert(1.0,"mph","ft/s");   // 1.46666667
    convert(55.0,"mph","km/hr"); // 88.5139203

    convert(1.0,"m/s2","m/s2");    // 1
    convert(9.81,"m/s2","ft/s2");  // 32.1850394
    convert(1.0,"in/s2","cm/s2");  // 2.54

    convert(1.0,"r","r");    //      1.0
    convert(1.0,"r","d");    //     57.295779513
    convert(1.0,"r","as");   // 206264.8062471 (seconds)
    convert(1.0,"r","am");   //   3437.746770785 (minutes)
    convert(1.0,"r","mr");   //   1000.0 (milliradians)
    convert(720,"d","rev");   //     2.0

    convert(1.0,"r/s","r/s");   // 1
    convert(1.0,"r/s","d/s");   // 57.2957779
    convert(1.0,"rev/s","rpm"); // 60
    convert(6.0,"d/s","rpm");   // 1

    convert(1.0,"r/s2","r/s2");     // 1
    convert(1.0,"r/s2","d/s2");     // 57.2957779
    convert(720.0,"d/s2","rev/s2"); // 2

    convert(1.0,"kg/m3","kg/m3");    // 1
    convert(1.0,"kg/m3","g/cm3");    // 0.001
    convert(1.0,"lb/ft3","oz/in3");  // 0.009259259

    convert(1.0,"N/m2","N/m2");    // 1
    convert(1.0,"psi","oz/in2");   // 16

    convert(-50.0,"C","F");  // -58.0F
    convert( 21.0,"C","K");  // 294.15K (room temp)
    convert( 100.0,"C","R"); // 671.67R
    convert( 212.0,"F","C"); // 100C
    convert( 90.0,"F","K");  // 305.37K
    convert( 100.0,"F","R"); // 559.67R
    convert( 300.0,"K","F"); // 80.33F
    convert( 60.0,"K","F");  // -213.15C
    convert( 220.0,"K","R"); // 396R
    convert( 300.0,"R","F"); // -159.67F
    convert( 500.0,"R","C"); // 4.63C
    convert( 180.0,"R","K"); // 100K

    convert(1.0,"kg","kg");   // 1
    convert(1.0,"sl","lbm");  // 32.174
    convert(1.0,"g","mt");    // 1e-06

    convert(1.0,"N","N");    // 1
    convert(1.0,"N","oz");   // 3.59694309
    convert(1.0,"kN","lbf"); // 224.8089431

    convert(1.0,"N*m","N*m");       // 1
    convert(1.0,"N*m","oz*in");     // 141.611928936
    convert(1.0,"lbf*ft","lbf*in"); // 12

    convert(1.0,"v","v");    // 1
    convert(1.0,"kv","v");   // 1000

    convert(1.0,"amp","amp");    // 1
    convert(1.0,"amp","mamp");   // 1000

    convert(1.0,"ohm","ohm");    // 1
    convert(1.0,"ohm","mohm");   // 1000

    convert(1.0,"dB","dB");    // 1

    convert(1.0,"J","J");    // 1
    convert(1.0,"J","mJ");   // 1000
    convert(1.0,"J","uJ");   // 1.0e6
    convert(1.0,"J","kJ");   // 0.001
    convert(1.0,"J","MJ");   // 1.0e-6

    convert(1.0,"--","--");     // 1
    convert(1.0,"cnt","one");   // 1
    convert(1.0,"cnt","1");     // 1
*/
