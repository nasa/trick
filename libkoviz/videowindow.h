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
#include <QHBoxLayout>
#include <QApplication>
#include <QTextEdit>
#include <QSettings>
#include <QList>
#include <QPair>
#include <QStringList>
#include <QFrame>
#include <QMouseEvent>

#ifdef HAS_MPV
#include <mpv/client.h>
#endif

class ClickFrame : public QFrame
{
  public:
    ClickFrame(QWidget* parent = nullptr) : QFrame(parent) {}

  protected:

    void enterEvent(QEvent* event) override
    {
        Q_UNUSED(event);
        setStyleSheet("background-color: #6666aa;"
                      "border: 1px solid #ededed;");
    }

    void leaveEvent(QEvent* event) override
    {
        Q_UNUSED(event);
        setStyleSheet("background-color: black;"
                      "border: 1px solid #ededed;");
    }
};

class VideoWidget : public QWidget
{
    Q_OBJECT

  public:
    VideoWidget(QWidget *parent = nullptr) : QWidget(parent) {}

    QHBoxLayout* hbox;
    ClickFrame*  whiteBox;
    QWidget* helperWidget;
    QWidget* mpvContainer;
};

struct Video
{
    QString fileName;
    double timeOffset;
    VideoWidget* videoWidget;
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
    bool eventFilter(QObject* obj, QEvent* event);

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
    QGridLayout* _grid;

    void create_player();
    void _resize_videos(const QList<QPair<QString,double> >& videos);
    Video* _create_video();

#ifdef HAS_MPV
    void handle_mpv_event(Video* video, mpv_event *event);
#endif
};

#endif
