#pragma once
#include "model/Snapshot.h"
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QVector>

struct UnitInfo {
  QString id;
  qint64 posX{0};
  qint64 posY{0};
  QString driverUid;
  QString status;
  sit::VehicleView vehicle;
};

class UnitRegistry : public QObject {
  Q_OBJECT
public:
  explicit UnitRegistry(QObject *parent = nullptr);

  static UnitRegistry &instance();

  UnitRegistry(const UnitRegistry &) = delete;
  UnitRegistry &operator=(const UnitRegistry &) = delete;

  void configure(const QString &projectId, const QString &idToken);
  void clearSession();
  void loadUnits();
  void syncVehicle(const sit::VehicleView &vehicle, const QString &driverUid,
                   const QString &status = QString());
  void registerUnit(const QString &unitId, const QString &driverUid,
                    qint64 posX, qint64 posY, const QString &status);
  void setStatus(const QString &unitId, const QString &status);

  QString projectId() const { return projectId_; }
  QString idToken() const { return idToken_; }
  QVector<UnitInfo> units() const { return units_; }
  sit::VehicleView vehicle() const { return vehicle_; }

  const UnitInfo *findUnit(const QString &unitId) const;
  UnitInfo *findUnit(const QString &unitId);

signals:
  void unitsLoaded(const QVector<UnitInfo> &units);
  void errorOccurred(const QString &message);

private slots:
  void onLoadFinished();
  void onWriteFinished();

private:
  void readConfig();
  bool ready(QString *error) const;
  QNetworkRequest requestFor(const QString &path) const;

  QNetworkAccessManager *network_{};
  QString projectId_;
  QString idToken_;
  QVector<UnitInfo> units_;
  sit::VehicleView vehicle_;
  QString syncedUnitId_;
  QString syncedDriver_;
  qint64 syncedPosX_{0};
  qint64 syncedPosY_{0};
};
