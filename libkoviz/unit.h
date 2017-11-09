/*
 *  Author: Keith Vetter
 *  Notes: I completely rewrote this in August 2017.
 */

#ifndef _DP_UNIT_
#define _DP_UNIT_

#include <QHash>
#include <QPair>
#include <QString>
#include <QStringList>
#include <stdio.h>

class Unit {

  public:

    Unit();
    Unit(const QString& name);
    ~Unit();

    QString name() const;
    void setName(const QString& name);
    bool canConvert(const Unit& to) const;
    static bool isUnit(const QString& name);
    static double scale(const QString& from, const QString& to);
    static double bias(const QString& from, const QString& to);
    static QString next(const QString& unit);
    static QString prev(const QString& unit);

  private:

    QString _name;
    static QHash<QPair<QString,QString>,double> _scales;
    static QHash<QPair<QString,QString>,double> _biases;
    static QHash<QPair<QString,QString>,double> _initScales();
    static QHash<QPair<QString,QString>,double> _initBiases();
    static QString _family(const QString& name);

};
#endif
