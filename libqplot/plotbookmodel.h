#ifndef PLOTBOOKMODEL_H
#define PLOTBOOKMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>
#include "libsnapdata/montemodel.h"
#include "libsnaprt/utils.h"

class PlotBookModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PlotBookModel( MonteModel* monteModel,
                            QObject *parent = 0);
    explicit PlotBookModel( MonteModel* monteModel,
                            int rows, int columns, QObject * parent = 0 );

    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const;

    QModelIndex sessionStartIdx() const ;
    QModelIndex sessionStopIdx() const ;

    bool isPageIdx(const QModelIndex& idx) const ;
    QModelIndex pageIdx(const QModelIndex& idx) const ;
    QModelIndexList pageIdxs() const ;

    QModelIndexList plotIdxs(const QModelIndex& pageIdx) const ;

    QModelIndex curvesIdx(const QModelIndex& plotIdx) const ;
    QModelIndexList curveIdxs(const QModelIndex& curvesIdx) const ;
    QModelIndex curveLineColorIdx(const QModelIndex& curveIdx) const ;
    bool isCurveLineColorIdx(const QModelIndex& idx) const;

    QModelIndex pageBGColorIndex(const QModelIndex& pageIdx) const;

    enum IdxEnum
    {
        Invalid,
        SessionStartTime,
        SessionStopTime,
        Page,
            PageTitle,
            PageStartTime,
            PageStopTime,
            PageBGColor,
            PageFGColor,
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
                            CurveRunID,
                            CurveData,
                            CurveLineColor,
                            CurveXScale,
                            CurveYScale,
                            CurveXBias,
                            CurveYBias,
                            CurveSymbolStyle,
                            CurveSymbolSize,
                            CurveLineStyle,
                            CurveYLabel,
                    PlotTitle,
                    PlotXMin,
                    PlotXMax,
                    PlotYMin,
                    PlotYMax,
                    PlotStartTime,
                    PlotStopTime,
                    PlotGrid,
                    PlotGridColor,
                    PlotBGColor,
                    PlotFGColor,
                    PlotFont
    };

    IdxEnum indexEnum(const QModelIndex& idx) const;

signals:
    
public slots:

private:
    MonteModel* _monteModel;
    void _initModel();
    
};

#endif // PLOTBOOKMODEL_H
