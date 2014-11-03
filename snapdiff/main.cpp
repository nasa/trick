#include <QApplication>
#include <QObject>
#include <QThread>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "snapd.h"
#include "libsnapdata/runs.h"

void dircheck(const QString& dirName);
void usage(const QString& msg=QString());

int main(int argc, char *argv[])
{
    bool ok;

    try {
        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);
        if ( argc != 3 ) {
            usage("wrong number of arguments");
        }
        QString run1(argv[1]);
        QString run2(argv[2]);
        dircheck(run1);
        dircheck(run2);
        QStringList s;
        s.append(run1);
        s.append(run2);
        SnapDiff w(run1,run2);
        w.show();
        return a.exec();
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        exit(-1);
    }

    return 0;
}

void usage(const QString& msg)
{
    qDebug() << "usage: snapdiff <RUN_1> <RUN_2>" ;
    if ( !msg.isEmpty() ) {
        qDebug() << " msg: " << msg;
    }
    exit(-1);
}

void dircheck(const QString& dirName)
{
    QDir dir(dirName);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snapdiff [error] : directory does not exist: \"%s\".\n",
                       dirName.toAscii().constData());
        usage();
    }
}

