#include "options.h"
#include <stdio.h>
#include <stdlib.h>

// TODO: error checking
Quantifier::Quantifier() :
    _spec(QString()),
    _min(0),
    _max(0),
    _isValid(false)
{
}

Quantifier::Quantifier(const QString &spec) :
    _spec(spec),
    _min(0),
    _max(0),
    _isValid(false)
{
    set(_spec);
}

void Quantifier::set(const QString &spec)
{
    QString s(spec);

    // Remove wspace
    QRegExp rx("\\s");
    s = s.remove(rx);

    _isValid = true;

    if ( s == "*" ) {
        _min = 0;
        _max = UINT_MAX;
    } else if ( s == "?" ) {
        _min = 1;
        _max = 1;
    } else if ( s == "+" ) {
        _min = 1;
        _max = UINT_MAX;
    } else if ( s.startsWith("{") && s.endsWith("}") ) {
        // {m,n} or {n}
        s = s.remove(0,1);
        s.chop(1);
        QStringList l = s.split(',') ;
        bool ok;
        if ( l.size() == 2 ) {
            // {m,n}
            _min = l.at(0).toUInt(&ok);
            if ( !ok ) {
                _isValid = false;
            }
            _max = l.at(1).toUInt(&ok);
            if ( !ok ) {
                _isValid = false;
            }
        } else if ( l.size() == 1 ) {
            // {n}
            _min = l.at(0).toUInt(&ok);
            _max = _min;
            if ( !ok ) {
                _isValid = false;
            }
        }
    } else {
        // n, shortcut for {n}
        bool ok;
        _min = s.toUInt(&ok);
        _max = _min;
        if ( !ok ) {
            _isValid = false;
        }
    }
}

uint Quantifier::min()
{
    return _min;
}

uint Quantifier::max()
{
    return _max;
}

bool Quantifier::isValid()
{
    return _isValid;
}

Option::Option(const QString &nameSpec,
               void* addr,
               const QVariant &defaultValue,
               const QString &info,
               void* presetCB,
               void* postsetCB) :
    _nameSpec(nameSpec),
    _info(info),
    _defaultValue(defaultValue),
    _addrDouble(0),
    _addrInt(0),
    _addrUInt(0),
    _addrQString(0),
    _addrQStringList(0),
    _fpresetDouble(0),
    _fpresetInt(0),
    _fpresetUInt(0),
    _fpresetQString(0),
    _fpresetBool(0),
    _fpresetQStringList(0),
    _fpostsetDouble(0),
    _fpostsetInt(0),
    _fpostsetUInt(0),
    _fpostsetQString(0),
    _fpostsetBool(0),
    _fpostsetQStringList(0)
{
    // Remove wspace
    QString name = nameSpec;
    QString rx("\\s");
    name = name.remove(rx);

    _isMandatory = false;
    if ( name.startsWith("<") && name.endsWith(">") ) {
        _isMandatory = true;
    }

    _isRootOption = true;
    if (name.startsWith("<-") || name.startsWith("[-") || name.startsWith("-")){
        _isRootOption = false;
    }

    _quantifier = Quantifier("{1}");
    QStringList list = name.split(":");
    if ( list.size() >= 2 ) {
        QString q = list.last();
        if ( q.endsWith(">") || q.endsWith("]") ) {
            q.chop(1);
        }
        _quantifier = q;
    }

    if ( _defaultValue.type() == QVariant::Double ) {
        _addrDouble = (double*) addr;
        _fpresetDouble = (FPresetDouble*)presetCB;
        _fpostsetDouble = (FPostsetDouble*)postsetCB;
    } else if ( _defaultValue.type() == QVariant::Int ) {
        _addrInt = (int*) addr;
        _fpresetInt = (FPresetInt*)presetCB;
        _fpostsetInt = (FPostsetInt*)postsetCB;
    } else if ( _defaultValue.type() == QVariant::UInt ) {
        _addrUInt = (uint*) addr;
        _fpresetUInt = (FPresetUInt*)presetCB;
        _fpostsetUInt = (FPostsetUInt*)postsetCB;
    } else if ( _defaultValue.type() == QVariant::String ) {
        _addrQString = (QString*) addr;
        _fpresetQString = (FPresetQString*)presetCB;
        _fpostsetQString = (FPostsetQString*)postsetCB;
    } else if ( _defaultValue.type() == QVariant::Bool ) {
        _addrBool = (bool*) addr;
        _fpresetBool = (FPresetBool*)presetCB;
        _fpostsetBool = (FPostsetBool*)postsetCB;
    } else if ( _defaultValue.type() == QVariant::StringList ) {
        _addrQStringList = (QStringList*) addr;
        _fpresetQStringList = (FPresetQStringList*)presetCB;
        _fpostsetQStringList = (FPostsetQStringList*)postsetCB;
    } else {
        fprintf(stderr,"koviz [bad scoobs]: default value=\"%s\" "
                       "given to Option::Option() constructor "
                       "has an unsupported type of \"%s\"\n",
                       _defaultValue.toString().toLatin1().constData(),
                       _defaultValue.typeName());
        exit(-1);
    }
}

QString Option::nameSpec()
{
    return _nameSpec;
}

QString Option::name()
{
    QString s = nameSpec();

    // Remove wspace
    QRegExp rx("\\s");
    s = s.remove(rx);

    if ( s.startsWith("<") && s.endsWith(">") ) {
        s = s.remove(0,1);
        s.chop(1);
    }

    if ( s.startsWith("[") && s.endsWith("]") ) {
        s = s.remove(0,1);
        s.chop(1);
    }

    int i = s.lastIndexOf(":") ;
    if ( i > 0 ) {
        s = s.left(i);
    }

    return s;
}

QString Option::info()
{
    return _info;
}

QVariant Option::defaultValue()
{
    return _defaultValue;
}

Quantifier Option::valueQuantifier()
{
    return _quantifier;
}

bool Option::isMandatory()
{
    return _isMandatory;
}

bool Option::isRootOption()
{
    return _isRootOption;
}

QVariant::Type Option::type()
{
    return _defaultValue.type();
}

void Option::setValue(const QVariant &val, bool* ok)
{
    if ( val.type() == QVariant::Int ) {
        if ( _fpresetInt ) {
            _fpresetInt(_addrInt, val.toInt(), ok);
        }
        if ( *ok ) *_addrInt = val.toInt();
        if ( _fpostsetInt && *ok) {
            _fpostsetInt(_addrInt, ok);
        }
    } else if ( val.type() == QVariant::UInt ) {
        if (_fpresetUInt) {
            _fpresetUInt(_addrUInt, val.toUInt(), ok);
        }
        if ( *ok ) *_addrUInt = val.toUInt();
        if ( _fpostsetUInt && *ok) {
            _fpostsetUInt(_addrUInt, ok);
        }
    } else if ( val.type() == QVariant::Double ) {
        if ( _fpresetDouble ) {
            _fpresetDouble(_addrDouble, val.toDouble(), ok);
        }
        if ( *ok) *_addrDouble = val.toDouble();
        if ( _fpostsetDouble && *ok) {
            _fpostsetDouble(_addrDouble, ok);
        }
    } else if ( val.type() == QVariant::String ) {
        if ( _fpresetQString ) {
            _fpresetQString(_addrQString, val.toString(), ok);
        }
        if ( *ok) *_addrQString = val.toString();
        if ( _fpostsetQString && *ok) {
            _fpostsetQString(_addrQString, ok);
        }
    } else if ( val.type() == QVariant::Bool ) {
        if ( _fpresetBool ) {
            _fpresetBool(_addrBool, val.toBool(), ok);
        }
        if ( *ok) *_addrBool = val.toBool();
        if ( _fpostsetBool && *ok) {
            _fpostsetBool(_addrBool, ok);
        }
    } else if ( val.type() == QVariant::StringList ) {
        if ( _fpresetQStringList ) {
            _fpresetQStringList(_addrQStringList, val.toStringList(), ok);
        }
        if ( *ok) *_addrQStringList = val.toStringList();
        if ( _fpostsetQStringList && *ok) {
            _fpostsetQStringList(_addrQStringList, ok);
        }
    } else {
        fprintf(stderr,"koviz [bad scoobs]: bad type to Option::setValue()\n");
    }
}

void Option::setValue(const QVariantList &vals)
{
    Q_UNUSED(vals);
    fprintf(stderr,"koviz [TODO]: setValue(const QVariantList &vals)\n");
    exit(-1);
}

Options::Options() :
    _nRootOptions(0)
{
}

Options::~Options()
{
    foreach ( Option* opt, _opts ) {
        delete opt;
    }
}

QString Options::usage()
{
    QString u;
    QTextStream s(&u);

    s << Qt::endl;
    s << "usage: " <<  _programName << Qt::endl;

    QStringList optNames;
    foreach ( Option* opt, _opts.values() ) {
        optNames << opt->name();
    }
    optNames.sort();

    foreach ( QString optName, optNames ) {
        foreach ( Option* opt, _opts.values() ) {
            if ( opt->name() == optName ) {
                s << Qt::right << qSetFieldWidth(15) << opt->nameSpec()
                  << Qt::left << qSetFieldWidth(0) << "      "
                  << Qt::left << qSetFieldWidth(60) << opt->info()
                  << qSetFieldWidth(1) << Qt::endl;
                break;
            }
        }
    }
    s << Qt::endl;

    return u;
}

void Options::_addOption(const QString &nameSpec,
                             void* varAddr,
                             const QVariant& defaultValue,
                             const QString &info,
                             void* presetCB,
                             void* postsetCB)
{
    Option* opt = new  Option(nameSpec, varAddr, defaultValue, info,
                              presetCB, postsetCB);
    if ( opt->isRootOption() ) {
        _nRootOptions++;
        if ( _nRootOptions > 1 ) {
            fprintf(stderr,"koviz [error]: Only one root option "
                    "(non hyphenated) should be present.  Too many "
                    "were add()ed.\n"
                    "The offending option was %s\n",
                    nameSpec.toLatin1().constData());
            exit(-1);
        }
    }
    _opts.insert(opt->name(),opt);
}

// TODO: ret error or throw
void Options::add(const QString &nameSpec,
                   QString *varPtr, const QString& defaultValue,
                   const QString &info,
                   Option::FPresetQString* presetCB,
                   Option::FPostsetQString *postsetCB)
{
    QVariant v(defaultValue);
    _addOption(nameSpec,varPtr,v,info,(void*)presetCB,(void*)postsetCB);

    if ( varPtr ) {
        *varPtr = defaultValue;
    }
}

void Options::add(const QString &nameSpec,
                  bool *varPtr, const bool &defaultValue,
                  const QString &info,
                  Option::FPresetBool *presetCB,
                  Option::FPostsetBool *postsetCB)
{
    QVariant v(defaultValue);
    _addOption(nameSpec,varPtr,v,info,(void*)presetCB,(void*)postsetCB);

    if ( varPtr ) {
        *varPtr = defaultValue;
    }
}

void Options::add(const QString &nameSpec,
                  QStringList *varPtr,  const QStringList &defaultValue,
                  const QString &info,
                  Option::FPresetQStringList *presetCB,
                  Option::FPostsetQStringList *postsetCB)
{
    QVariant v(defaultValue);
    _addOption(nameSpec,varPtr,v,info,(void*)presetCB,(void*)postsetCB);

    if ( varPtr ) {
        *varPtr = defaultValue;
    }
}

// TODO: ret error or throw
void Options::add(const QString &nameSpec, double *varPtr,
                   double defaultValue, const QString &info,
                   Option::FPresetDouble* presetCB,
                   Option::FPostsetDouble* postsetCB)
{
    QVariant v(defaultValue);
    _addOption(nameSpec,varPtr,v,info,(void*)presetCB,(void*)postsetCB);

    if ( varPtr ) {
        *varPtr = defaultValue;
    }
}

// TODO: ret error or throw
void Options::add(const QString &nameSpec, uint *varPtr,
                   uint defaultValue, const QString &info,
                   Option::FPresetUInt* presetCB,
                   Option::FPostsetUInt* postsetCB)
{
    QVariant v(defaultValue);
    _addOption(nameSpec,varPtr,v,info,(void*)presetCB,(void*)postsetCB);

    if ( varPtr ) {
        *varPtr = defaultValue;
    }
}

void Options::parse(int argc, char **argv, const QString& programName,
                     bool *ok)
{
    *ok = true;

    _programName = programName;

    QStringList args;
    for ( int i = 1 ; i < argc ;  ++i ) {
        args << argv[i];
    }

    // Make sure mandatory nonRoot options present
    foreach ( Option* opt, _opts.values() ) {
        if ( opt->isMandatory() && !opt->isRootOption()) {
            int i = args.indexOf(opt->name());
            if ( i < 0 ) {
                // Mandatory option not found
                fprintf(stderr,"%s [error]: while parsing commandline. "
                        "Mandatory option=\"%s\" "
                        "was not specified.\n",
                        programName.toLatin1().constData(),
                        opt->nameSpec().toLatin1().constData());
                *ok = false;
                return;
            }
        }
    }
    if ( !*ok) return;

    // Get nonRoot option values
    QHash<Option*,QVariantList> opt2vals;
    foreach ( Option* opt, _opts.values() ) {
        if ( opt->isRootOption() ) continue;
        QVariantList v = _extractOptValsFromArgs(opt, args, ok);
        opt2vals.insert(opt,v);
    }
    if ( !*ok) return;

    // Remove nonRoot opt/vals from args
    // What is left at the end are the root values
    QStringList rootStringVals = args;
    foreach ( Option* opt, _opts.values() ) {
        if ( opt->isRootOption() ) continue;
        int i = rootStringVals.indexOf(opt->name());
        if ( i >= 0 ) {
            int nVals = opt2vals.value(opt).size();
            if ( nVals == 0 ) {
                // A boolean option may not have a "value"
                // In this case, the value is implicitly true
                if ( opt->type() == QVariant::Bool ) {
                    opt->setValue(QVariant(true),ok);
                } else {
                    fprintf(stderr,"koviz [error]: Option=\"%s\" "
                                   "does not have a value specified.\n",
                                   opt->name().toLatin1().constData());
                    *ok = false;
                }
                if ( !*ok ) return;
            }
            for ( int j = nVals + i; j >= i; --j ) {
                rootStringVals.removeAt(j);
            }
        }
    }

    // Make sure there are the correct number of root values
    // And map rootOption to its value list
    Option* rootOption = 0;
    foreach ( Option* opt, _opts.values() ) {
        if ( opt->isRootOption() ) {
            rootOption = opt;
        }
    }
    if ( rootOption ) {
        if ((uint)rootStringVals.size() < rootOption->valueQuantifier().min() ||
            (uint)rootStringVals.size() > rootOption->valueQuantifier().max()) {
            if ( rootStringVals.size() == 0 && rootOption->isMandatory() ) {
                *ok = false;
                fprintf(stderr,"%s [error]: while parsing commandline. ",
                        programName.toLatin1().constData());
                fprintf(stderr,"  Root option=\"%s\" has no values, but should "
                        "have at least %d specified",
                        rootOption->nameSpec().toLatin1().constData(),
                        rootOption->valueQuantifier().min());
            } else {
                uint min =  rootOption->valueQuantifier().min()  ;
                uint max =  rootOption->valueQuantifier().max()  ;
                if ( (uint) rootStringVals.size() < min ) {
                    *ok = false;
                    fprintf(stderr,"%s [error]: while parsing commandline. ",
                            programName.toLatin1().constData());
                    fprintf(stderr,"  Root option=\"%s\" needs "
                            "a minimum of %d "
                            "values, but only %d present: \n",
                            rootOption->nameSpec().toLatin1().constData(),
                            min,
                            rootStringVals.size());
                } else if ( (uint) rootStringVals.size() > max ) {
                    *ok = false;
                    fprintf(stderr,"%s [error]: while parsing commandline. ",
                            programName.toLatin1().constData());
                    fprintf(stderr,"  Root option=\"%s\" accepts "
                            "a max of %d values. There are %d present: \n",
                            rootOption->nameSpec().toLatin1().constData(),
                            max,
                            rootStringVals.size());
                }
                if ( !*ok ) {
                    foreach ( QString v, rootStringVals ) {
                        fprintf(stderr,"        %s\n", v.toLatin1().constData());
                    }
                }
            }
        }
        if ( !*ok) return;

        // Turn root values into a qvariant list
        QVariantList rootVals;
        foreach ( QString s, rootStringVals ) {
            if ( rootOption->type() == QVariant::Int ) {
                int v = s.toInt(ok);
                if ( *ok ) rootVals << QVariant(v);
            } else if ( rootOption->type() == QVariant::UInt ) {
                uint v = s.toUInt(ok);
                if ( *ok ) rootVals << QVariant(v);
            } else if ( rootOption->type() == QVariant::Double ) {
                double v = s.toDouble(ok);
                if ( *ok ) rootVals << QVariant(v);
            } else if ( rootOption->type() == QVariant::Bool ) {
                bool v = stringToBool(s,ok);
                if ( *ok ) rootVals << QVariant(v);
            } else {
                // string
                rootVals << QVariant(s);
            }
        }
        opt2vals.insert(rootOption,rootVals);
    }
    if ( !*ok) return;

    // Set the parsed value of the option to the
    // variable that is associated with the option
    foreach ( Option* opt, opt2vals.keys() ) {
        if ( opt->type() == QVariant::StringList ) {
            QStringList list;
            foreach ( QVariant v, opt2vals.value(opt) ) {
                list.append(v.toString());
            }
            QVariant vlist(list);
            opt->setValue(vlist,ok);
        } else {
            QVariantList v = opt2vals.value(opt);
            if ( v.size() > 0 ) {
                // TODO: multiple values
                opt->setValue(opt2vals.value(opt).at(0),ok);
            }
        }
        if ( !*ok ) {
            return;
        }
    }
}

QVariantList Options::_extractOptValsFromArgs(Option *opt,
                                              const QStringList& s,
                                               bool* ok)
{
    QVariantList listVals;
    *ok = true;

    int i = s.indexOf(opt->name());
    if ( i < 0 ) return listVals;

    for ( uint j = 0; j < opt->valueQuantifier().max() ; ++j ) {
        int k = i+j+1;
        if ( k >= s.size() ) {
            break;
        }
        QString valString = s.at(k);
        if ( _opts.keys().contains(valString)) {
            // This is an option, not a value
            break;

        }
        if ( opt->type() == QVariant::Int ) {
            int v = valString.toInt(ok);
            if ( *ok ) listVals << QVariant(v);
        } else if ( opt->type() == QVariant::UInt ) {
            uint v = valString.toUInt(ok);
            if ( *ok ) listVals << QVariant(v);
        } else if ( opt->type() == QVariant::Double ) {
            double v = valString.toDouble(ok);
            if ( *ok ) listVals << QVariant(v);
        } else if ( opt->type() == QVariant::StringList ) {
            if ( *ok ) listVals << QVariant(valString);
        } else if ( opt->type() == QVariant::Bool ) {
            bool v = stringToBool(valString,ok);
            if ( *ok ) listVals << QVariant(v);
        } else {
            // string
            listVals << QVariant(valString);
        }
        if ( ! *ok && j < opt->valueQuantifier().min() ) {
            fprintf(stderr, "Option=\"%s\" is of type %s but "
                            "had value \"%s\" which is wrong type\n",
                            opt->nameSpec().toLatin1().constData(),
                            opt->defaultValue().typeName(),
                            valString.toLatin1().constData());
            break;
        }
    }

    if ( *ok ) {
        if ( (uint) listVals.size() < opt->valueQuantifier().min() ) {
            fprintf(stderr, "Option=\"%s\" is missing value or "
                            "did not have minimum number of values specified.",
                            opt->nameSpec().toLatin1().constData());
            *ok = false;
        }
    }


    return listVals;
}

bool Options::stringToBool(const QString &s, bool* ok)
{
    bool ret = false;
    *ok = false;
    if ( s.compare("true",Qt::CaseInsensitive) == 0 ||
         s.compare("yes",Qt::CaseInsensitive) == 0 ||
         s.compare("ok",Qt::CaseInsensitive) == 0 ||
         s.compare("on",Qt::CaseInsensitive) == 0 ||
         s.compare("connect",Qt::CaseInsensitive) == 0 ||
         s.compare("active",Qt::CaseInsensitive) == 0 ||
         s.compare("go",Qt::CaseInsensitive) == 0 ||
         s.compare("high",Qt::CaseInsensitive) == 0 ||
         s.compare("enable",Qt::CaseInsensitive) == 0 ||
         s.compare("open",Qt::CaseInsensitive) == 0 ||
         s.compare("forward",Qt::CaseInsensitive) == 0 ||
         s.compare("good",Qt::CaseInsensitive) == 0 ||
         s.compare("independent",Qt::CaseInsensitive) == 0 ||
         s.compare("1",Qt::CaseInsensitive) == 0 ) {

        *ok = true;
        ret = true;
    } else if ( s.compare("false",Qt::CaseInsensitive) == 0 ||
                s.compare("no",Qt::CaseInsensitive) == 0 ||
                s.compare("off",Qt::CaseInsensitive) == 0 ||
                s.compare("inactive",Qt::CaseInsensitive) == 0 ||
                s.compare("backward",Qt::CaseInsensitive) == 0 ||
                s.compare("bad",Qt::CaseInsensitive) == 0 ||
                s.compare("disconnect",Qt::CaseInsensitive) == 0 ||
                s.compare("stop",Qt::CaseInsensitive) == 0 ||
                s.compare("low",Qt::CaseInsensitive) == 0 ||
                s.compare("disable",Qt::CaseInsensitive) == 0 ||
                s.compare("closed",Qt::CaseInsensitive) == 0 ||
                s.compare("dependent",Qt::CaseInsensitive) == 0 ||
                s.compare("0",Qt::CaseInsensitive) == 0 ) {
        *ok = true;
        ret = false;
    }

    return ret;
}
