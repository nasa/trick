#include "timelineloader.h"

TimeLineLoader::TimeLineLoader(const QString &timelinecsv)
{
    _file = new QFile(timelinecsv);
    if ( ! _file->exists() || !(_file->permissions()&QFile::ReadUser) ) {
        fprintf(stderr,"snap [error] : can't read or find file %s\n",
                         timelinecsv.toAscii().constData());
        exit(-1);
    }

    if (!_file->open(QIODevice::ReadOnly)) {
        fprintf(stderr,"Snap: [error] could not open %s\n",
                timelinecsv.toAscii().constData());
        exit(-1);
    }
    _csv = new QTextStream(_file);

    _load_header();
}

TimeLineLoader::~TimeLineLoader()
{
    _file->close();
    delete _file;
    delete _csv;
}

//
// Return timeline table for thread or job
// Default jobid is -1, which means return all jobs i.e. thread
//
// is_exclude_jid will show all jobs *except* the jobid specified
// good for sim sync time to see when thread 0 is running
SnapTable* TimeLineLoader::table(int itid,
                                 double ibegin_time, double iend_time,
                                 double ijid, bool is_exclude_jid)
{
    SnapTable* table = new SnapTable();
    table->insertColumns(0,2);
    QString title;
    if ( ijid == -1.0 ) {
        title = QString("Time Line - Thread %1").arg(itid);
    } else {
        title = QString("Time Line - Job %1 Thread %2").arg(ijid).arg(itid);
    }
    table->setHeaderData(1,Qt::Horizontal,QVariant("Begin Running"));
    table->setHeaderData(2,Qt::Horizontal,QVariant("End Running"));

    _csv->seek(_pos_begin_data);

    // Body
    while ( 1 ) {
        int tid = 0 ;
        double jid = 0.0;
        double beg_timestamp = 0.0;
        double end_timestamp = 0.0;

        // begin 4 line read (skip it)
        QString line = _csv->readLine();
        if ( line.isEmpty() ) {
            break;
        }

        // Second line in 4 line record
        line = _csv->readLine();
        QStringList fields = line.split(',',QString::SkipEmptyParts);
        beg_timestamp = fields.at(0).toDouble();
        if ( fields.at(1) != "0" ) {
            tid = 0;
            jid = fields.at(1).toDouble();
        } else if ( fields.at(2) != "0" ) {
            jid = fields.at(2).toDouble();
            tid = 0;
        } else {
            for ( int ii = 3; ii < fields.size(); ++ii) {
                if ( fields.at(ii) == "0" ) {
                    continue ;
                }
                tid = _threadids.at(ii-2);
                jid = fields.at(ii).toDouble();
            }
        }

        // Third line in 4 line record
        line = _csv->readLine();
        int a = line.indexOf(',');
        end_timestamp = line.left(a).toDouble();

        // end 4 line read (skip it)
        line = _csv->readLine();

        bool is_jid = false;
        if ( ijid == -1.0 ) {
            is_jid = true;
        } else if ( !is_exclude_jid && jid == ijid ) {
            is_jid = true;
        } else if ( is_exclude_jid && jid != ijid ) {
            is_jid = true;
        }
        if ( tid == itid && is_jid &&
             beg_timestamp >= ibegin_time &&  end_timestamp <= iend_time) {

            int rc = table->rowCount();
            table->insertRows(rc,1);

            QModelIndex x = table->index(rc,0);
            QModelIndex y = table->index(rc,1);
            table->setData(x,QVariant(beg_timestamp));
            table->setData(y,QVariant(end_timestamp));
        }
    }

    return table;
}

void TimeLineLoader::_load_header()
{
    QString line = _csv->readLine();
    QStringList fields = line.split(",",QString::SkipEmptyParts);
    int ii = 0 ;
    _threadids.append(0);
    foreach ( QString s, fields ) {
        if ( ii < 3 ) {
            ii++;
            continue;
        }
        int a = s.lastIndexOf('C');
        int b = s.lastIndexOf('_');
        QString stid = s.mid(a+1,b-a-1);
        bool ok;
        int tid = stid.toInt(&ok);
        if ( !ok ) {
            qDebug() << "snap [bad scoobies]: parse error of timeline csv";
            exit(-1);
        }
        _threadids.append(tid);
    }
    _pos_begin_data = _csv->pos();
}
