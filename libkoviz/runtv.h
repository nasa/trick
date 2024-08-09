#ifndef RUNTV_H
#define RUNTV_H

#include <QString>
#include <QStringList>
#include "run.h"

// For now, this class is a placeholder that returns empty

class RunTV : public Run
{
public:
    RunTV(const QString& host_port_path); // e.g. trick://127.0.0.1:4545
    virtual QStringList params();
    virtual DataModel *dataModel(const QString& param);
private:
    QString _host_port_path;
    DataModel* _model;
    QStringList _params;
};

#endif // RUNTV_H
