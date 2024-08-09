#include "runtv.h"

RunTV::RunTV(const QString &host_port_path) :
    _host_port_path(host_port_path)
{
}

QStringList RunTV::params()
{
    return _params;
}

DataModel *RunTV::dataModel(const QString &param)
{
    DataModel* model = 0;
    return model;
}
