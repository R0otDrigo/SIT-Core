#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include <QVector>

struct UnitInfo {
    QString id;
    qint64 x{0};
    qint64 y{0};
    QString driverUid;
    QString status;
};

class UnitRegistry : public QObject {
    Q_OBJECT
public:
    explicit UnitRegistry(QObject* parent = nullptr);

    void configure(const QString& projectId, const QString& idToken);
    void loadUnits();
    void registerUnit(const QString& unitId, const QString& driverUid,
                      qint64 x, qint64 y, const QString& status);
    void setStatus(const QString& unitId, const QString& status);

    QString projectId() const { return projectId_; }
    QString idToken() const { return idToken_; }
    QVector<UnitInfo> units() const { return units_; }

signals:
    void unitsLoaded(const QVector<UnitInfo>& units);
    void errorOccurred(const QString& message);

private slots:
    void onLoadFinished();
    void onWriteFinished();

private:
    void readConfig();
    bool ready(QString* error) const;
    QNetworkRequest requestFor(const QString& path) const;

    QNetworkAccessManager* network_{};
    QString projectId_;
    QString idToken_;
    QVector<UnitInfo> units_;
};
