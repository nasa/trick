#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QMainWindow>

#include <mpv/client.h>

class QTextEdit;

class VideoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = 0);
    ~VideoWindow();
    bool focussed;

private slots:
    void on_file_open();
    void on_new_window();
    void on_mpv_events();
    void seek_time(double time);

signals:
    void mpv_events();
    void timechangedByMpv(double time);

private:
    QWidget *mpv_container;
    mpv_handle *mpv;
    QTextEdit *log;

    void append_log(const QString &text);

    void create_player();
    void handle_mpv_event(mpv_event *event);
};

#endif // QTEXAMPLE_H
