#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>   // usage()
#include <QHash>
#include <QVariant>
#include <QRegExp>
#include <limits.h> // INT_MAX etc (editor will not pick it up)

// Handles specs like  {2,3} or + or * or ? or {2}
class Quantifier
{
public:
    Quantifier();
    Quantifier(const QString& spec);
    void set(const QString& spec);
    uint min();
    uint max();
    bool isValid();
private:
    QString _spec;
    uint _min;
    uint _max;
    bool _isValid;
};

class Option
{
public:
    typedef void(FPresetDouble)(double* varPtr, double val, bool* ok);
    typedef void(FPresetInt)(      int* varPtr,    int val, bool* ok);
    typedef void(FPresetUInt)(    uint* varPtr,   uint val, bool* ok);
    typedef void(FPresetQString)(QString* varPtr, const QString& val, bool* ok);
    typedef void(FPresetBool)(bool* varPtr, const bool& val, bool* ok);
    typedef void(FPresetQStringList)(QStringList* varPtr,
                                     const QStringList& val,
                                     bool* ok);

    typedef void(FPostsetDouble)(double* varPtr,  bool* ok);
    typedef void(FPostsetInt)(      int* varPtr,  bool* ok);
    typedef void(FPostsetUInt)(    uint* varPtr,  bool* ok);
    typedef void(FPostsetQString)(QString* varPtr, bool* ok);
    typedef void(FPostsetBool)(bool* varPtr, bool* ok);
    typedef void(FPostsetQStringList)(QStringList* varPtr, bool* ok);

public:
    Option(const QString& nameSpec,   // e.g. <-point:{2,3}>
           void* addr,
           const QVariant& defaultValue,
           const QString& info,
           void* presetCB=0,
           void* postsetCB=0);

    QString nameSpec();   // e.g. <-point:{2,3}>
    QString name();       // e.g returns "-point"
    QString info();
    QVariant defaultValue();
    bool isMandatory();
    bool isRootOption();

    Quantifier valueQuantifier(); // how many (possible) values e.g. 1,{2,3},?,+
    QVariant::Type type();

    void setValue(const QVariant& val, bool *ok);
    void setValue(const QVariantList& vals);

private:
    QString _nameSpec;
    QString _info;
    QVariant _defaultValue;  // this could be a QList<QVariant>
    bool _isMandatory;     // e.g. treat <SIM_dir> # SIM_dir is mandatory

    bool _isRootOption;    // this is for an "option" that has no -o <option>
                          // e.g. koviz -startTime 100 <RUN_dir>
                          // RUN_dir is a rootOption, -startTime is not

    Quantifier _quantifier;   // e.g. koviz <RUN1> <RUN2>, the root option
                          // take 2 vals so quantSpec is {2}

    // Instead of using a set() method, the user passes
    // the address of the variable that corresponds to
    // the option.  This may seem willy nilly, but
    // it allows the user to set the option's variable
    // without having to go through the options interface
    // which makes it much more convenient to use.
    // Maybe I should use a union
    double* _addrDouble;
    int* _addrInt;
    uint* _addrUInt;
    QString* _addrQString;
    bool* _addrBool;
    QStringList* _addrQStringList;

    // Pre/post function pointers
    FPresetDouble* _fpresetDouble;
    FPresetInt* _fpresetInt;
    FPresetUInt* _fpresetUInt;
    FPresetQString* _fpresetQString;
    FPresetBool* _fpresetBool;
    FPresetQStringList* _fpresetQStringList;
    FPostsetDouble* _fpostsetDouble;
    FPostsetInt* _fpostsetInt;
    FPostsetUInt* _fpostsetUInt;
    FPostsetQString* _fpostsetQString;
    FPostsetBool* _fpostsetBool;
    FPostsetQStringList* _fpostsetQStringList;
};



// The optionName has some encoded info
//     Names surrounded by <> are mandatory
//          e.g. add("<RUN_1>", &sim->dirName, "RUN_test", "Run directory");
//     Names surrounded by [] are optional
//          e.g. add("[-stopTime]", &sim->stopTime, 1.e-20, "Sim stop time");
//     Root names can have have multiple specs separated by '|'
//          e.g. add("<Monte_dir>|<RUN_dir>",
//     TODO: Range of values for option with quantifiers
//          e.g. <-rgb:{3}> expects 3 ints for the -rgb "mandatory option"
//          e.g. <RUN_dir:+> expects 1 or more RUN dirs
//          e.g. <RUN_dir:*> expects 0 or more RUN dirs (and is mandatory!)
//          e.g. <RUN_dir:?> expects 0 or 1 RUN dirs
//          e.g. <RUN_dir:{,2}> expects at most 2 RUN dirs
//          e.g. [-point:{2,3}] expects x,y or x,y,z, and is optional
//     TODO: Think about this
//          e.g. add("<Monte_dir>:{1}|<RUN_dir>:+",...);
//          also, should only root options can have infinite num of values?
//
class Options
{
public:
    Options();
    ~Options();

    void add(const QString& nameSpec,
             QString* varPtr, const QString& defaultValue,
             const QString& info,
             Option::FPresetQString* presetCB=0,
             Option::FPostsetQString* postsetCB=0);
    void add(const QString& nameSpec,
             bool* varPtr, const bool& defaultValue,
             const QString& info,
             Option::FPresetBool* presetCB=0,
             Option::FPostsetBool* postsetCB=0);
    void add(const QString& nameSpec,
             QStringList* varPtr, const QStringList& defaultValue,
             const QString& info,
             Option::FPresetQStringList* presetCB=0,
             Option::FPostsetQStringList* postsetCB=0);
    void add(const QString& nameSpec,
             double* varPtr,  double defaultValue,
             const QString& info,
             Option::FPresetDouble* presetCB=0,
             Option::FPostsetDouble* postsetCB=0);
    void add(const QString& nameSpec,
             uint* varPtr, uint defaultValue,
             const QString& info,
             Option::FPresetUInt* presetCB=0,
             Option::FPostsetUInt* postsetCB=0);

    void parse(int argc, char **argv, const QString &programName, bool *ok);
    QString usage() ;

    static bool stringToBool(const QString& s, bool *ok);

private:
    QString _programName;
    int _nRootOptions;
    QHash<QString,Option*> _opts;
    void _addOption(const QString& nameSpec,
                       void* varAddr,
                       const QVariant &defaultValue,
                       const QString& info,
                       void* presetCB=0,
                       void* postsetCB=0);
    QVariantList _extractOptValsFromArgs(Option* opt, const QStringList &s,
                                         bool *ok);
} ;

    
#endif // OPTIONS_H
