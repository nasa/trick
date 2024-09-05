#ifndef BOOKTABLEVIEW_H
#define BOOKTABLEVIEW_H

#include <QAbstractItemView>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QFontInfo>
#include <QPalette>
#include <QBrush>
#include <QPen>
#include <QScrollBar>
#include <QList>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QKeyEvent>
#include <limits.h>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
    #include <QGuiApplication>
#else
    #include <QtGui/QApplication>
#endif

#include "bookmodel.h"
#include "curvemodel.h"
#include "unit.h"
#include "timestamps.h"

class BookTableView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit BookTableView(QWidget *parent = 0);

public:
    virtual QModelIndex indexAt( const QPoint& point) const;
    virtual QRect visualRect(const QModelIndex &index) const;
    virtual void scrollTo(const QModelIndex &index,
                          ScrollHint hint = EnsureVisible);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

protected:
    virtual QModelIndex moveCursor(CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers);
    virtual int horizontalOffset() const;
    virtual int verticalOffset() const;
    virtual bool isIndexHidden(const QModelIndex &index) const;
    virtual void setSelection(const QRect &rect,
                              QItemSelectionModel::SelectionFlags command);
    virtual QRegion visualRegionForSelection(
                              const QItemSelection &selection) const;
    virtual void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *e);

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight,
                             const QVector<int> &roles = QVector<int>());
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);

private:
    PlotBookModel* _bookModel() const;
    QList<double> _timeStamps;
    int _mTop;
    int _mBot;
    int _mLft;
    int _mRgt;

    QStringList _columnLabels() const;

    QStringList _format(const QList<double>& vals);
    QStringList __format(const QList<double>& vals, const QString &format);

signals:

public slots:

};

#endif // BOOKTABLEVIEW_H
