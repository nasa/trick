#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <clocale>
#include <sstream>
#include <stdexcept>
#include <time.h>
#include <math.h>

#include <QMainWindow>
#include <QtGlobal>
#include <QFileDialog>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QGridLayout>
#include <QApplication>
#include <QTextEdit>
#include <QSettings>
#include <QList>
#include <QPair>
#include <QStringList>

#ifdef HAS_MPV
#include <mpv/client.h>
#endif

struct Video {
    QString fileName;
    double timeOffset;
    QWidget* mpv_container;
#ifdef HAS_MPV
    mpv_handle* mpv;
#endif
} ;

class VideoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoWindow(const QList<QPair<QString,double> >& videos,
                         QWidget *parent = 0);
    ~VideoWindow();
    void set_videos(const QList<QPair<QString,double> >& videos);
    void set_start(double startTime);
    void pause();
    void wrap_mpv_events();

protected:
     virtual void closeEvent(QCloseEvent *event);

public slots:
    void on_mpv_events();
    void seek_time(double time);

signals:
    void mpv_events();
    void timechangedByMpv(double time);
    void close();

private:
    QList<Video*> _videos;
    double _startTime;
    QGridLayout* _grid ;

    void create_player();
    void _resize_videos(const QList<QPair<QString,double> >& videos);
    Video* _create_video();

#ifdef HAS_MPV
    void handle_mpv_event(Video* video, mpv_event *event);
#endif
};

#endif
