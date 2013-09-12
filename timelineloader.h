#ifndef TIMELINELOADER_H
#define TIMELINELOADER_H

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QVector>
#include <stdlib.h>
#include "snaptable.h"

class FrameReader;

class TimeLineLoader
{
  public:
    TimeLineLoader(const QString& timelinecsv);
    ~TimeLineLoader();
    SnapTable* table(int itid,
                     double ibegin_time=0.0, double iend_time=1.0e20,
                     double ijid=-1.0, bool is_exclude_jid=false);

private:
    TimeLineLoader() {}

    void _load_header();
    QVector<int> _threadids;
    QFile* _file;
    QTextStream* _csv;
    qint64 _pos_begin_data;
};

#endif // TIMELINELOADER_H
