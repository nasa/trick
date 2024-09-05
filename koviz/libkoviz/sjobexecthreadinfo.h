#ifndef SJOBEXECTHREADINFO_H
#define SJOBEXECTHREADINFO_H

#include <stdexcept>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QRegExp>
#include <QFile>
#include "versionnumber.h"

class SJobExecThreadInfo
{
  public:

    SJobExecThreadInfo(const QString& runDir, int threadId=-1);

    void setThreadId(int threadId);

    bool    hasInfo()     const { return _hasInfo; }
    int     threadId()    const { return _threadId; }
    double  frequency()   const { return _freq; }  // TODO: Trick bug on thread0
    QString kind()        const { return _kind; }
    QString rtCPUNumber() const { return _rtCPUNumber; }

  private:

    QString _runDir;
    bool _hasInfo;
    int _threadId;
    QString _kind;  // scheduled,amf or async I think
    double _freq;
    QString _sJobExecutionFileName;
    QString _rtCPUNumber;
    VersionNumber _trickVersion;

    void _calcThreadInfo();

    static QString _err_string;
    static QTextStream _err_stream;
};

#endif // SJOBEXECTHREADINFO_H
