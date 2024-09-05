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
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <stdio.h>
#include <stdlib.h>

class Unit {

  public:

    Unit();
    Unit(const QString& name);
    ~Unit();

    QString name() const;
    void setName(const QString& name);
    bool isEmpty() const;

    static bool canConvert(const QString& from,const QString& to);
    static bool isUnit(const QString& name);
    static double scale(const QString& from, const QString& to);
    static double bias(const QString& from, const QString& to);
    static QString next(const QString& unit);
    static QString prev(const QString& unit);
    static Unit map(const Unit& u1, const Unit& u2);
    static QString showUnits();
    static QString derivative(const QString& unit);
    static QString integral(const QString& unit);

  private:

    QString _name;
    static QHash<QPair<QString,QString>,double> _scales;
    static QHash<QPair<QString,QString>,double> _biases;
    static QHash<QPair<QString,QString>,double> _initScales();
    static QHash<QPair<QString,QString>,double> _initBiases();
    static QString _family(const QString& name);
    static QStringList _sortUnits(const QStringList& unitsIn);
    static QStringList _families();

};
#endif
