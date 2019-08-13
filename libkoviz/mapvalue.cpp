#include "mapvalue.h"

MapValue::MapValue(const QString &strIn) :
    _name(""),
    _unit(""),
    _bias(0.0),
    _scale(1.0)
{
    QString str(strIn);
    int i;

    // Scale
    i = str.indexOf("scale(");
    if ( i > 0 ) {
        int j = i+6;
        int k = str.indexOf(')',j);
        if ( k < 0 ) {
            fprintf(stderr, "koviz [error]: Malformed mapvalue=%s\n",
                    strIn.toLatin1().constData());
            exit(-1);
        }
        QString scaleStr = str.mid(j,k-j).trimmed();
        bool ok;
        _scale = scaleStr.toDouble(&ok);
        if ( !ok ) {
            fprintf(stderr, "koviz [error]: Malformed scale value=%s in "
                            "mapvalue=%s\n",
                    scaleStr.toLatin1().constData(),
                    strIn.toLatin1().constData());
            exit(-1);
        }
        str.remove(i,k-i+1);
    }

    // Bias
    i = str.indexOf("bias(");
    if ( i > 0 ) {
        int j = i+5;
        int k = str.indexOf(')',j);
        if ( k < 0 ) {
            fprintf(stderr, "koviz [error]: Malformed mapvalue=%s\n",
                    strIn.toLatin1().constData());
            exit(-1);
        }
        QString biasStr = str.mid(j,k-j).trimmed();
        bool ok;
        _bias = biasStr.toDouble(&ok);
        if ( !ok ) {
            fprintf(stderr, "koviz [error]: Malformed bias value=%s in "
                            "mapvalue=%s\n",
                    biasStr.toLatin1().constData(),
                    strIn.toLatin1().constData());
            exit(-1);
        }
        str.remove(i,k-i+1);
    }

    // Unit
    i = str.indexOf('{');
    if ( i > 0 ) {
        int j = i+1;
        int k = str.indexOf('}',j);
        if ( k < 0 ) {
            fprintf(stderr, "koviz [error]: Malformed mapvalue=%s\n",
                    strIn.toLatin1().constData());
            exit(-1);
        }
        _unit = str.mid(j,k-j).trimmed();
        str.remove(i,k-i+1);
    }

    // Name
    _name = str.trimmed();

    /*
    fprintf(stderr, "_name=%s\n", _name.toLatin1().constData());
    fprintf(stderr, "_unit=%s\n", _unit.toLatin1().constData());
    fprintf(stderr, "_bias=%g\n", _bias);
    fprintf(stderr, "_scale=%g\n", _scale);
    */
}

QString MapValue::name() const
{
    return _name;
}

QString MapValue::unit() const
{
    return _unit;
}

double MapValue::bias() const
{
    return _bias;
}

double MapValue::scale() const
{
    return _scale;
}
