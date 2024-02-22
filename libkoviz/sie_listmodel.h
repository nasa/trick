#ifndef SIELISTMODEL_H
#define SIELISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QMimeData>
#include <QTcpSocket>
#include <QDomDocument>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QMutex>
#include <QProgressDialog>
#include <QThread>

class SieListModel : public QAbstractListModel
{
  Q_OBJECT
  public:
    explicit SieListModel(const QString& host, int port,
                          QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;

    QString paramUnit(const QString &param);
    int paramSize(const QString& param);
    bool isParamExists(const QString& paramIn);
    QStringList expandParam(const QString& param);

  public slots:
    int setRegexp(const QString &rx);

  signals:
    void sendMessage(const QString& msg);
    void modelLoaded();

  protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

  private:
    QString _host;
    int     _port;
    mutable QMutex _mutex;  // protect thread loaded _params string list
    mutable QMutex _mutexLoadSieModel;
    QStringList _params;
    QString _rx;
    QStringList _filteredParams;
    int _fetchCount;
    int _fetchChunkSize;
    bool _isModelResetting;

    QDomDocument _sieDoc;
    QHash<QString,QDomElement> _name2element;

    QTcpSocket* _vsSocketParamSizes;

    void _connectToSim();
    void _createSIEModel();
    bool _createSieDocument();
    bool __createSieDocument();
    void _loadSieElement(const QDomElement& element, QList<QDomElement> &path);
    void _expandParam(const QString& expandedParam,
                      const QString& param,
                      QStringList* params);
    QDomElement _domElement(const QString& param);

  private slots:
    void _init();
    void _socketDisconnect();
};

#endif // SIELISTMODEL_H
