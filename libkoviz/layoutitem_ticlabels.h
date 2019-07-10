#ifndef LAYOUTITEM_TICLABELS_H
#define LAYOUTITEM_TICLABELS_H

#include <QFontMetrics>
#include <QList>
#include <cmath>
#include "layoutitem_paintable.h"
#include "bookmodel.h"

struct RulerLabelBox
{
    QPointF center; // preferred center
    QString strVal;
    QRectF  bb;     // actual center is center of bb
};


class TicLabelsLayoutItem : public PaintableLayoutItem
{
public:
    TicLabelsLayoutItem(const QFontMetrics& fontMetrics,
                        const QFontMetrics& fm8,
                        PlotBookModel* bookModel,
                        const QModelIndex& plotIdx);
    ~TicLabelsLayoutItem();
    virtual Qt::Orientations expandingDirections() const;
    virtual QRect  geometry() const;
    virtual bool  isEmpty() const;
    virtual QSize  maximumSize() const;
    virtual QSize  minimumSize() const;
    virtual void  setGeometry(const QRect &r);
    virtual QSize  sizeHint() const;
    virtual void paint(QPainter* painter,
                       const QRect& R, const QRect& RG,
                       const QRect& C, const QRectF& M);

private:
    QFontMetrics _fontMetrics;
    QFontMetrics _fm8;
    PlotBookModel* _bookModel;
    QModelIndex _plotIdx;
    QRect _rect;

    //  +---++---++---++---++---++----------------------++---+
    //  |   ||   ||   ||   || C ||        Tics          || C |
    //  |   ||   ||   ||   |+---++----------------------++---+
    //  |   ||   || Y ||   |+---++----------------------++---+
    //  | Y ||   ||   ||   ||   ||      Title           ||   |
    //  |   || _ || T || _ ||   |+----------------------+|   |
    //  | A || m || i || m ||   |+----------------------+|   |
    //  | x || 1 || c || 2 || T ||                      || T |
    //  | i ||   ||   ||   || i ||                      || i |
    //  | s ||   || L ||   || c ||        Curves        || c |
    //  |   ||   || a ||   || s ||                      || s |
    //  | L ||   || b ||   ||   ||                      ||   |
    //  | a ||   || e ||   ||   ||                      ||   |
    //  | b ||   || l ||   ||   ||                      ||   |
    //  | e ||   || s ||   |+---++----------------------++---+
    //  | l ||   ||   ||   |+---++-----------------------+---+
    //  |   ||   ||   ||   || C ||       Tics            | C |
    //  +---++---++---++---++---++-----------------------+---+
    //  +----------------------------------------------------+
    //  |                       _m3                          +
    //  +----------------------------------------------------+
    //  +----------------------------------------------------+
    //  |                     X Tic Labels                   |
    //  +----------------------------------------------------+
    int _m1;
    int _m2;
    int _m3;

    QList<double> _majorYTics(const QModelIndex &plotIdx) const;
    QRectF _plotMathRect(const QModelIndex& plotIdx) const;
    QModelIndex _plotMathRectIdx(const QModelIndex &plotIdx) const;
    QString _format(double tic) const;
    QRect _boundingRect(const QString &strVal) const;
    void _paint10Exponent(QPainter* painter,
                          const RulerLabelBox& box) const;
    QSize _sizeHintLeft() const;
    QSize _sizeHintBottom() const;
};

#endif // LAYOUTITEM_TICLABELS_H
