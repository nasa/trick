#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <clocale>
#include <sstream>
#include <stdexcept>

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
    QWidget *mpv_container;
    double _timeOffset;
    void create_player();

#ifdef HAS_MPV
    mpv_handle *mpv;
    void handle_mpv_event(mpv_event *event);
#endif
};

#endif
