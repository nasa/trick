#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <clocale>
#include <sstream>
#include <stdexcept>
#include <time.h>

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
#include <QStringList>

#ifdef HAS_MPV
#include <mpv/client.h>
#endif

class VideoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = 0);
    ~VideoWindow();
    void set_file(const QString& fname);
    void set_offset(double timeOffset);
    void wrap_mpv_events();

protected:
     virtual void closeEvent(QCloseEvent *event);

public slots:
    void on_file_open();
    void on_mpv_events();
    void seek_time(double time);

signals:
    void mpv_events();
    void timechangedByMpv(double time);

private:
    //QWidget *mpv_container;
    //QWidget *mpv_container2;
    QList<QWidget*> mpv_containers;
    double _timeOffset;
    QList<double> _timeOffsets;
    void create_player();

#ifdef HAS_MPV
    //mpv_handle *mpv;
    //mpv_handle *mpv2;
    QList<mpv_handle*> mpvs;
    void handle_mpv_event(mpv_event *event);
#endif
};

#endif
