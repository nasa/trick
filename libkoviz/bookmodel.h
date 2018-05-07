#ifndef PLOTBOOKMODEL_H
#define PLOTBOOKMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>
#include <QHash>
#include <QPainterPath>
#include <QPen>
#include <QVector>
#include "runs.h"
#include "unit.h"
#include "utils.h"
#include "curvemodel.h"

#include <QList>
#include <QColor>
#include <math.h>

class PlotBookModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PlotBookModel(const QStringList &timeNames, Runs* runs,
                            QObject *parent = 0);
    explicit PlotBookModel(const QStringList &timeNames, Runs* runs,
                            int rows, int columns, QObject * parent = 0 );
    ~PlotBookModel();

    virtual bool setData(const QModelIndex &idx,
                         const QVariant &value, int role=Qt::EditRole);

public:
    double xScale(const QModelIndex& curveIdx) const;
    double yScale(const QModelIndex& curveIdx) const;
    double xBias(const QModelIndex& curveIdx) const;
    double yBias(const QModelIndex& curveIdx) const;
    QRectF calcCurvesBBox(const QModelIndex& curvesIdx) const;

    QStandardItem* addChild(QStandardItem* parentItem,
                            const QString& childTitle,
                            const QVariant &childValue=QVariant());

    bool isIndex(const QModelIndex& idx, const QString& tag) const;

    bool isChildIndex(const QModelIndex& pidx,
                      const QString &expectedParentItemText,
                      const QString& childItemText) const;

    QModelIndex getIndex(const QModelIndex& startIdx,
                        const QString& searchItemText,
                        const QString &expectedStartIdxText=QString()) const;

    QModelIndex getDataIndex(const QModelIndex& startIdx,
                          const QString& searchItemText,
                          const QString &expectedStartIdxText=QString()) const;

    QString getDataString(const QModelIndex& startIdx,
                          const QString& searchItemText,
                          const QString &expectedStartIdxText=QString()) const;
    double getDataDouble(const QModelIndex& startIdx,
                         const QString& searchItemText,
                         const QString &expectedStartIdxText=QString()) const;
    int getDataInt(const QModelIndex& startIdx,
                   const QString& searchItemText,
                   const QString &expectedStartIdxText=QString()) const;
    QHash<QString,QVariant> getDataHash(const QModelIndex& startIdx,
                          const QString& searchItemText,
                          const QString &expectedStartIdxText=QString()) const;

    // Convenience for getting QPen line style (see Qt doc)
    QVector<qreal> getLineStylePattern(const QModelIndex& curveIdx) const;

    QStringList lineStyles() const;

    CurveModel* createCurve(int row, const QString& tName,
                            const QString& xName, const QString& yName);
    CurveModel* getCurveModel(const QModelIndex& curvesIdx, int i) const;
    CurveModel* getCurveModel(const QModelIndex& curveIdx) const;

    QPainterPath* getCurvePainterPath(const QModelIndex& curveIdx) const;
    QPainterPath* getCurvesErrorPath(const QModelIndex& curvesIdx);
    QString getCurvesXUnit(const QModelIndex& curvesIdx);
    QString getCurvesYUnit(const QModelIndex& curvesIdx);
    bool isXTime(const QModelIndex& plotIdx) const;

    QModelIndexList getIndexList(const QModelIndex& startIdx,
                        const QString& searchItemText,
                        const QString &expectedStartIdxText=QString()) const;

    // Convenience wrappers for getIndexList
    QModelIndexList pageIdxs() const ;
    QModelIndexList plotIdxs(const QModelIndex& pageIdx) const ;
    QModelIndexList curveIdxs(const QModelIndex& curvesIdx) const ;

    // Curve Colors
    QList<QColor> createCurveColors(int nCurves);
    QColor errorLineColor() const;

    // Convenience wrappers for get/setting PlotMathRect
    QRectF getPlotMathRect(const QModelIndex &plotIdx);
    void setPlotMathRect(const QRectF& mathRect, const QModelIndex &plotIdx);

    // Utility for abbreviating a list of run:var names
    QStringList abbreviateLabels(const QStringList &labels) const;

    // Legend
    QStringList legendSymbols(const QModelIndex &plotIdx) const ;
    QStringList legendColors(const QModelIndex &plotIdx) const ;
    QStringList legendLinestyles(const QModelIndex &plotIdx) const ;
    QStringList legendLabels(const QModelIndex &plotIdx) const ;
    QList<QPen*> legendPens(const QModelIndex &plotIdx) const ;
    bool isPageLegend(const QModelIndex& pageIdx) const ;
    bool isPlotLegendsSame(const QModelIndex& pageIdx) const ;

    // Page Foreground and Background colors
    QColor pageBackgroundColor(const QModelIndex& pageIdx) const;
    QColor pageForegroundColor(const QModelIndex& pageIdx) const;

signals:
    
public slots:

private:
    QStringList _timeNames;
    Runs* _runs;
    void _initModel();
    QModelIndex _pageIdx(const QModelIndex& idx) const ;
    QModelIndex _plotIdx(const QModelIndex& idx) const ;
    QModelIndex _ancestorIdx(const QModelIndex &startIdx,
                        const QString& ancestorText,
                        const QString &expectedStartIdxText=QString()) const;

    QHash<CurveModel*,QPainterPath*> _curve2path;
    QPainterPath* _createPainterPath(CurveModel *curveModel,
                                     double startTime, double stopTime);
    QPainterPath* _createCurvesErrorPath(const QModelIndex& curvesIdx) const;

    QString _commonRootName(const QStringList& names, const QString& sep) const;
    QString __commonRootName(const QString& a, const QString& b,
                             const QString& sep) const;

};

#endif // PLOTBOOKMODEL_H
