#include "videowindow.h"

/* Keith's comment to remove:
  Temporary comment to help me remember how to test on this branch
  Completely hard coded videofiles with offsets
  See: VideoWindow::set_file(const QString &fnameIn)
       line 208
  To test:
  cd /home/kvetter/dev/es/video-offset/RUN_D13C1A.20230322.02.D.H12T-H12T.4506.5p00000.ComplexComboPlusRatesMax
  kv DATA_Simhost -video Video/2023-03-22T0939_R02_Cam01_REC1_3.mp4
*/

static void wakeup(void *ctx)
{
    VideoWindow *mainwindow = (VideoWindow *)ctx;
    mainwindow->wrap_mpv_events();
}

VideoWindow::VideoWindow(const QList<QPair<QString, double> > &videos,
                         QWidget *parent) :
    QMainWindow(parent),
    _startTime(0.0)
{
#ifdef HAS_MPV
    std::setlocale(LC_NUMERIC, "C");
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Koviz MPV");
    setMinimumSize(640, 480);

    statusBar();

    QPair<QString,double> videoIn;
    foreach (videoIn, videos) {
        mpv_handle* mpv = mpv_create();
        if (!mpv) {
            throw std::runtime_error("can't create mpv instance");
        }

        QWidget* mpv_container = new QWidget(this);

        mpv_container->setAttribute(Qt::WA_DontCreateNativeAncestors);
        mpv_container->setAttribute(Qt::WA_NativeWindow);

        mpv_set_option_string(mpv, "input-default-bindings", "yes");
        mpv_set_option_string(mpv, "input-vo-keyboard", "yes");
        mpv_set_option_string(mpv, "keep-open", "always");
        mpv_set_option_string(mpv, "osd-level", "0");

        mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);

        mpv_set_wakeup_callback(mpv, wakeup, this);

        int64_t wid = mpv_container->winId();
        mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);

        if (mpv_initialize(mpv) < 0) {
            throw std::runtime_error("mpv failed to initialize");
        }

        Video* video = new Video();
        video->fileName = videoIn.first;
        video->timeOffset = videoIn.second;
        video->mpv = mpv;
        video->mpv_container = mpv_container;
        _videos.append(video);
    }

    QGridLayout* grid = new QGridLayout();
    if ( _videos.size() == 1 ) {
        grid->addWidget(_videos.at(0)->mpv_container, 0, 0);
        grid->setColumnStretch(0, 1);
        grid->setRowStretch(0, 1);
    } else if ( _videos.size() == 6 ) {
        grid->addWidget(_videos.at(0)->mpv_container, 0, 0);
        grid->addWidget(_videos.at(1)->mpv_container, 0, 1);
        grid->addWidget(_videos.at(2)->mpv_container, 0, 2);
        grid->addWidget(_videos.at(3)->mpv_container, 1, 0);
        grid->addWidget(_videos.at(4)->mpv_container, 1, 1);
        grid->addWidget(_videos.at(5)->mpv_container, 1, 2);

        grid->setColumnStretch(0, 1);
        grid->setColumnStretch(1, 1);
        grid->setColumnStretch(2, 1);
        grid->setRowStretch(0, 1);
        grid->setRowStretch(1, 1);
    }

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(grid);
    setCentralWidget(centralWidget);

    connect(this, &VideoWindow::mpv_events,
            this, &VideoWindow::on_mpv_events,
            Qt::QueuedConnection);
#endif
}

void VideoWindow::wrap_mpv_events()
{
    emit mpv_events();
}

void VideoWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    QSettings settings("JSC", "koviz");

    settings.beginGroup("VideoWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void VideoWindow::seek_time(double time) {
#ifdef HAS_MPV
    int isIdle;
    int i = 0;
    foreach (Video* video, _videos) {
        int ret = mpv_get_property(video->mpv,"core-idle",
                                   MPV_FORMAT_FLAG,&isIdle);
        if ( ret >= 0 ) {
            if ( isIdle && parentWidget()->isActiveWindow() ) {
                // Koviz is driving time
                double timeOffset = video->timeOffset;
                QString com = QString("seek %1 absolute").arg(time+timeOffset);
                mpv_command_string(video->mpv, com.toLocal8Bit().data());
            }
        }
        ++i;
    }
#endif
}

#ifdef HAS_MPV
void VideoWindow::handle_mpv_event(Video* video, mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        if (strcmp(prop->name, "time-pos") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data - video->timeOffset;
                std::stringstream ss;
                ss << "At: " << time;
                statusBar()->showMessage(QString::fromStdString(ss.str()));
                int isIdle;
                int ret = mpv_get_property(video->mpv,"core-idle",
                                           MPV_FORMAT_FLAG, &isIdle);
                if ( ret >= 0 ) {
                    if ( !isIdle || isActiveWindow() ) {
                        // If mpv playing, update time
                        // If mpv paused but still active, update time
                        // If mpv paused and koviz active, don't emit signal
                        emit timechangedByMpv(time);
                    }
                }
            } else if (prop->format == MPV_FORMAT_NONE) {
                statusBar()->showMessage("");
            }
        }
        break;
    }
    case MPV_EVENT_SHUTDOWN: {
        mpv_terminate_destroy(video->mpv);
        video->mpv = NULL;
        break;
    }
    default: ;
        // Ignore event
    }
}
#endif

// This slot is invoked by wakeup() (through the mpv_events signal).
void VideoWindow::on_mpv_events()
{
#ifdef HAS_MPV
    // Process all events, until the event queue is empty.
    foreach (Video* video, _videos) {
        while (video->mpv) {
            mpv_event *event = mpv_wait_event(video->mpv, 0);
            if (event->event_id == MPV_EVENT_NONE) {
                break;
            }
            handle_mpv_event(video,event);
        }
    }
#endif
}

// Input is a list of "filename,timeoffset" pairs
void VideoWindow::set_videos(const QList<QPair<QString,double> >& videos)
{
#ifdef HAS_MPV
    // TODO: Handle case when number of videos change
    if ( videos.size() != _videos.size() ) {
        //_videos.clear();
        fprintf(stderr, "koviz [todo]: Handle number of videos changing\n");
        exit(-1);
    }

    QPair<QString,double> videoIn;
    int i = 0 ;
    foreach ( videoIn, videos ) {
        Video* video = _videos.at(i);
        video->fileName = videoIn.first;
        video->timeOffset = videoIn.second;
        ++i;
    }

    foreach (Video* video, _videos ) {
        if (video->mpv) {
            if ( _startTime + video->timeOffset > 0 ) {
                QString offset = QString("%1").arg(_startTime +
                                                   video->timeOffset);
                mpv_set_option_string(video->mpv,"start",
                                      offset.toLatin1().constData());
            }
            QString fname = video->fileName;
            const QByteArray c_filename = fname.toUtf8();
            const char *args[] = {"loadfile", c_filename.data(), NULL};
            mpv_command(video->mpv, args);

            // Without sleeping mpv will sometimes miss loading videos
            struct timespec req = {0, 500000000};
            nanosleep(&req, NULL);
        } else {
            fprintf(stderr, "koviz [bad scoobs]: VideoWindow::set_videos()\n");
            exit(-1);
        }
        ++i;
    }
#endif
}

void VideoWindow::set_start(double startTime)
{
    _startTime = startTime;
}

void VideoWindow::pause()
{
#ifdef HAS_MPV
    foreach (Video* video, _videos) {
        if (video->mpv) {
            mpv_set_option_string(video->mpv,"pause","yes");
        }
    }
#endif
}


VideoWindow::~VideoWindow()
{
#ifdef HAS_MPV
    foreach (Video* video, _videos) {
        if (video->mpv) {
            mpv_event *event;
            while ((event = mpv_wait_event(video->mpv, 0))) {
                if (event->event_id == MPV_EVENT_SHUTDOWN) {
                    break;
                }
            }
            mpv_terminate_destroy(video->mpv);
        }
        delete video;
    }
#endif
}
