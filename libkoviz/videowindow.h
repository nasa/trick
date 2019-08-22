#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

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

#include <mpv/qthelper.hpp>
#include <mpv/client.h>

class VideoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = 0);
    ~VideoWindow();
    bool focussed;

private slots:
    void on_file_open();
    void on_mpv_events();
    void seek_time(double time);

signals:
    void mpv_events();
    void timechangedByMpv(double time);

private:
    QWidget *mpv_container;
    mpv_handle *mpv;

    void create_player();
    void handle_mpv_event(mpv_event *event);
};

#endif // QTEXAMPLE_H
