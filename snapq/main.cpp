#include <QApplication>
#include <QObject>
#include <QThread>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "montewindow.h"
#include "libopts/options.h"
#include "libsnapdata/runs.h"


Options::func_presetCB_DougString preset_montedir;
Options::func_presetCB_uint preset_beginRun;
Options::func_presetCB_uint preset_endRun;
class SnapOptions : public Options
{
  public:
    DougString montedir;
    unsigned int beginRun;
    unsigned int endRun;
};

SnapOptions opts;


int main(int argc, char *argv[])
{

    bool ok;

    opts.add(&opts.montedir,"<RUN_dir>", "",
             "RUN_directory with RUNs",
             preset_montedir);
    opts.add(&opts.beginRun,"-beginRun",0,
             "begin run (inclusive) in set of RUNs to plot",
             preset_beginRun);
    opts.add(&opts.endRun,"-endRun",100000,
             "end run (inclusive) in set of RUNs to plot",
             preset_endRun);
    opts.parse(argc,argv, "snapq", &ok);

    if ( !ok ) {
        fprintf(stderr,"%s\n",opts.usage().c_str());
        exit(-1);
    }

    try {
        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);
        QString rundir(opts.montedir.get().c_str());
        MonteWindow w(rundir);
        w.show();
        return a.exec();
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().c_str());
        exit(-1);
    }

    return 0;
}

void preset_montedir(DougString* curr_montedir,const char* new_montedir,bool* ok)
{
    Q_UNUSED(curr_montedir);

    QString montedir(new_montedir);

    QDir dir(montedir);
    if ( ! dir.exists() ) {
        fprintf(stderr,"snap [error] : couldn't find monte directory: \"%s\".\n",
                       montedir.toAscii().constData());
        *ok = false;
        return;
    }
}

void preset_beginRun(uint* beginRunId, const char* sval, bool* ok)
{
    Q_UNUSED(beginRunId);

    uint runId;
    Options::str_to_uint(sval, &runId, ok);

    if ( *ok ) {
        // Start time should be less than stop time
        if ( runId > opts.endRun ) {
            fprintf(stderr,"snap [error] : option -beginRun, set to %d, "
                    "must be greater than "
                    " -endRun which is set to %d\n",
                    runId, opts.endRun);
            *ok = false;
        }
    }
}

void preset_endRun(uint* endRunId, const char* sval, bool* ok)
{
    Q_UNUSED(endRunId);

    *ok = true;

    uint runId;
    Options::str_to_uint(sval, &runId, ok);

    if ( *ok ) {
        // Start time should be less than stop time
        if ( runId < opts.beginRun ) {
            fprintf(stderr,"snap [error] : option -endRun, set to %d, "
                    "must be greater than "
                    "-beginRun which is set to %d\n",
                    runId,opts.beginRun);
            *ok = false;
        }
    }
}
