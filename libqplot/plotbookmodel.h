#ifndef PLOTBOOKMODEL_H
#define PLOTBOOKMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>

class PlotBookModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PlotBookModel(QObject *parent = 0);
    explicit PlotBookModel( int rows, int columns, QObject * parent = 0 );

    QModelIndexList pageIdxs() const ;
    QModelIndexList plotIdxs(const QModelIndex& pageIdx) const ;
    QModelIndex curvesIdx(const QModelIndex& plotIdx) const ;
    QModelIndexList curveIdxs(const QModelIndex& curvesIdx) const ;
    QModelIndex curveLineColorIdx(const QModelIndex& curveIdx) const ;

    bool isPageIdx(const QModelIndex& idx) const ;
    bool isCurveLineColorIdx(const QModelIndex& idx) const;

    QModelIndex sessionStartIdx() const ;
    QModelIndex sessionStopIdx() const ;

    enum IdxEnum
    {
        Invalid,
        Page,
            SessionStartTime,
            SessionStopTime,
            PageTitle,
            PageStartTime,
            PageStopTime,
                Plot,
                    PlotXAxisLabel,
                    PlotYAxisLabel,
                    Curves,
                        Curve,
                            CurveTime,
                            CurveX,
                            CurveY,
                            CurveTimeUnit,
                            CurveXUnit,
                            CurveYUnit,
                            CurveRun,
                            CurveLineColor,
                    PlotTitle,
                    PlotXMin,
                    PlotXMax,
                    PlotYMin,
                    PlotYMax,
                    PlotStartTime,
                    PlotStopTime
    };

    IdxEnum indexEnum(const QModelIndex& idx);

signals:
    
public slots:

private:
    void _initModel();
    
};

#endif // PLOTBOOKMODEL_H
