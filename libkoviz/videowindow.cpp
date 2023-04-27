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

VideoWindow::VideoWindow(QWidget *parent) :
    QMainWindow(parent),
    _timeOffset(0.0)
{
#ifdef HAS_MPV
    std::setlocale(LC_NUMERIC, "C");
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Koviz MPV");
    setMinimumSize(640, 480);

    QMenu *menu = menuBar()->addMenu(tr("&File"));
    QAction *on_open = new QAction(tr("&Open"), this);
    connect(on_open, &QAction::triggered, this, &VideoWindow::on_file_open);
    menu->addAction(on_open);

    statusBar();

    for (int i = 0; i < 6; ++i ) {
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

        mpvs.append(mpv);
        mpv_containers.append(mpv_container);
    }

    _timeOffsets.append(15.8450);
    _timeOffsets.append(15.6080);
    _timeOffsets.append(15.6060);
    _timeOffsets.append(15.6150);
    _timeOffsets.append(15.4820);
    _timeOffsets.append(15.5170);

    QGridLayout* grid = new QGridLayout();
    grid->addWidget(mpv_containers.at(0), 0, 0);
    grid->addWidget(mpv_containers.at(1), 0, 1);
    grid->addWidget(mpv_containers.at(2), 0, 2);
    grid->addWidget(mpv_containers.at(3), 1, 0);
    grid->addWidget(mpv_containers.at(4), 1, 1);
    grid->addWidget(mpv_containers.at(5), 1, 2);

    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 1);
    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 1);

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
    foreach (mpv_handle* mpv, mpvs) {
        int ret = mpv_get_property(mpv,"core-idle",MPV_FORMAT_FLAG,&isIdle);
        if ( ret >= 0 ) {
            if ( isIdle && parentWidget()->isActiveWindow() ) {
                // Koviz is driving time
                double timeOffset = _timeOffsets.at(i);
                QString com = QString("seek %1 absolute").arg(time+timeOffset);
                mpv_command_string(mpv, com.toLocal8Bit().data());
            }
        }
        ++i;
    }
#endif
}

#ifdef HAS_MPV
void VideoWindow::handle_mpv_event(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        if (strcmp(prop->name, "time-pos") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data-_timeOffset;
                std::stringstream ss;
                ss << "At: " << time;
                statusBar()->showMessage(QString::fromStdString(ss.str()));
                int isIdle;
                foreach (mpv_handle* mpv, mpvs) {
                    int ret = mpv_get_property(mpv,"core-idle",MPV_FORMAT_FLAG,
                                               &isIdle);
                    if ( ret >= 0 ) {
                        if ( !isIdle || isActiveWindow() ) {
                            // If mpv playing, update time
                            // If mpv paused but still active, update time
                            // If mpv paused and koviz active, don't emit signal
                            emit timechangedByMpv(time);
                        }
                    }
                }
            } else if (prop->format == MPV_FORMAT_NONE) {
                statusBar()->showMessage("");
            }
        }
        break;
    }
    case MPV_EVENT_SHUTDOWN: {
        for (int i = 0; i < mpvs.size(); ++i) {
            mpv_terminate_destroy(mpvs[i]);
            mpvs[i] = NULL;
        }
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
    foreach (mpv_handle* mpv, mpvs) {
        while (mpv) {
            mpv_event *event = mpv_wait_event(mpv, 0);
            if (event->event_id == MPV_EVENT_NONE) {
                break;
            }
            handle_mpv_event(event);
        }
    }
#endif
}

void VideoWindow::on_file_open()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file");
    set_file(filename);
}

void VideoWindow::set_file(const QString &fnameIn)
{
#ifdef HAS_MPV
    QStringList files;
    QString dir("/home/kvetter/dev/es/video-offset/"
                "RUN_D13C1A.20230322.02.D.H12T-H12T.4506.5p00000."
                "ComplexComboPlusRatesMax/Video/");
    files.append(dir + "2023-03-22T0939_R02_Cam01_REC1_3.mp4");
    files.append(dir + "2023-03-22T0939_R02_Cam02_REC2_3.mp4");
    files.append(dir + "2023-03-22T0939_R02_Cam03_REC3_3.mp4");
    files.append(dir + "2023-03-22T0939_R02_CamCL_REC4_3.mp4");
    files.append(dir + "2023-03-22T0939_R02_CamF1_REC5_3.mp4");
    files.append(dir + "2023-03-22T0939_R02_CamF2_REC6_3.mp4");

    int i = 0;
    foreach (mpv_handle* mpv, mpvs) {
        if (mpv) {
            QString fname = files.at(i);
            const QByteArray c_filename = fname.toUtf8();
            const char *args[] = {"loadfile", c_filename.data(), NULL};
            mpv_command(mpv, args);

            // Without sleeping mpv will sometimes miss loading videos
            struct timespec req = {0, 500000000};
            nanosleep(&req, NULL);
        } else {
            fprintf(stderr, "koviz [bad scoobs]: VideoWindow::set_file()\n");
            exit(-1);
        }
        ++i;
    }
#endif
}

void VideoWindow::set_offset(double timeOffset)
{
    _timeOffset = timeOffset;
}

VideoWindow::~VideoWindow()
{
#ifdef HAS_MPV
    foreach (mpv_handle* mpv, mpvs) {
        if (mpv) {

            mpv_event *event;
            while ((event = mpv_wait_event(mpv, 0))) {
                if (event->event_id == MPV_EVENT_SHUTDOWN) {
                    break;
                }
            }

            mpv_terminate_destroy(mpv);
        }
    }
#endif
}
