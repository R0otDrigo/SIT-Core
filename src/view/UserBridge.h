#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include "model/Snapshot.h"

class UserBroadcast;
class UnitRegistry;
class VehicleListModel;

// Puente exclusivo de la vista de usuario: expone unicamente el bus asignado a ese uid.
// La vista de administrador usa MapBridge y no comparte este contexto QML.
class UserBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString userId READ userId NOTIFY userChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY userChanged)
    Q_PROPERTY(bool hasVehicle READ hasVehicle NOTIFY vehicleChanged)
    Q_PROPERTY(QString vehicleId READ vehicleId NOTIFY vehicleChanged)
    Q_PROPERTY(QString routeId READ routeId NOTIFY vehicleChanged)
    Q_PROPERTY(QString routeLabel READ routeLabel NOTIFY vehicleChanged)
    Q_PROPERTY(QString vehicleType READ vehicleType NOTIFY vehicleChanged)
    Q_PROPERTY(QString color READ color NOTIFY vehicleChanged)
    Q_PROPERTY(int passengers READ passengers NOTIFY vehicleChanged)
    Q_PROPERTY(int capacity READ capacity NOTIFY vehicleChanged)
    Q_PROPERTY(bool dwelling READ dwelling NOTIFY vehicleChanged)
    Q_PROPERTY(double latitude READ latitude NOTIFY vehicleChanged)
    Q_PROPERTY(double longitude READ longitude NOTIFY vehicleChanged)
    Q_PROPERTY(double kilometers READ kilometers NOTIFY vehicleChanged)
    Q_PROPERTY(QString info READ info NOTIFY vehicleChanged)
    Q_PROPERTY(QString antecedentes READ antecedentes NOTIFY broadcastChanged)
    Q_PROPERTY(QString evento READ evento NOTIFY broadcastChanged)
    Q_PROPERTY(QString placa READ placa NOTIFY vehicleChanged)
    Q_PROPERTY(QString marca READ marca NOTIFY vehicleChanged)
    Q_PROPERTY(double centerLatitude READ centerLatitude NOTIFY vehicleChanged)
    Q_PROPERTY(double centerLongitude READ centerLongitude NOTIFY vehicleChanged)
    Q_PROPERTY(double initialZoom READ initialZoom CONSTANT)
    Q_PROPERTY(QAbstractListModel* vehicleModel READ vehicleModel CONSTANT)

public:
    explicit UserBridge(QObject* parent = nullptr);

    void setUser(const QString& userId);
    void setBroadcast(UserBroadcast* broadcast);
    void setUnitRegistry(UnitRegistry* registry);
    void update(const sit::Snapshot& snapshot);

    QString userId() const { return userId_; }
    QString displayName() const { return displayName_; }
    bool hasVehicle() const { return assignedId_ >= 0; }
    QString vehicleId() const;
    QString routeId() const { return routeId_; }
    QString routeLabel() const { return routeLabel_; }
    QString vehicleType() const { return vehicleType_; }
    QString color() const { return color_; }
    int passengers() const { return passengers_; }
    int capacity() const { return capacity_; }
    bool dwelling() const { return dwelling_; }
    double latitude() const { return latitude_; }
    double longitude() const { return longitude_; }
    double kilometers() const { return kilometers_; }
    QString info() const { return info_; }
    QString antecedentes() const;
    QString evento() const;
    QString placa() const;
    QString marca() const { return vehicleType_; }
    double centerLatitude() const { return latitude_; }
    double centerLongitude() const { return longitude_; }
    double initialZoom() const { return 14.0; }
    QAbstractListModel* vehicleModel() const;

signals:
    void userChanged();
    void vehicleChanged();
    void broadcastChanged();

private:
    void clearVehicle();

    QString userId_;
    QString displayName_;
    int assignedId_{-1};
    sit::GeoCoordinate lastGeo_;
    double kilometers_{0.0};

    QString routeId_;
    QString routeLabel_;
    QString vehicleType_;
    QString color_;
    int passengers_{0};
    int capacity_{0};
    bool dwelling_{false};
    double latitude_{-16.3989};
    double longitude_{-71.5369};
    QString info_;
    QString simulationEvent_;
    UserBroadcast* broadcast_{};
    UnitRegistry* registry_{};
    QString registeredUnitId_;

    VehicleListModel* vehicleModel_{};
};
