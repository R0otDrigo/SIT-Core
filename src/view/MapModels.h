#pragma once
#include <QAbstractListModel>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <vector>
#include "model/GtfsData.h"
#include "model/Snapshot.h"

// Modelos estables para QML: los delegados (marcadores y trazados) se conservan entre
// snapshots y solo reciben dataChanged, evitando recrear RouteQuery/RouteModel cada 50 ms.
class RouteListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        RouteIdRole = Qt::UserRole + 1,
        NameRole,
        ColorRole,
        TrunkRole,
        WaypointsRole,
        AffectedRole
    };

    explicit RouteListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void resetFrom(const std::vector<sit::RouteView>& routes);
    void setAffectedRoutes(const QStringList& affected);

private:
    struct Row {
        QString id;
        QString name;
        QString color;
        bool trunk{};
        bool affected{};
        QVariantList waypoints;
    };
    std::vector<Row> rows_;
};

class StopListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        StopIdRole = Qt::UserRole + 1,
        NameRole,
        LatitudeRole,
        LongitudeRole,
        WaitingRole,
        SaturationRole,
        ClosedRole
    };

    explicit StopListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void sync(const std::vector<sit::StopView>& stops);

private:
    std::vector<sit::StopView> rows_;
};

class VehicleListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        VehicleIdRole = Qt::UserRole + 1,
        RouteIdRole,
        TypeRole,
        LatitudeRole,
        LongitudeRole,
        PassengersRole,
        CapacityRole,
        DwellingRole,
        ColorRole
    };

    explicit VehicleListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void sync(const std::vector<sit::VehicleView>& vehicles,
              const QHash<QString, QString>& routeColors);

private:
    struct Row {
        sit::VehicleView view;
        QString color;
    };
    std::vector<Row> rows_;
};

class GtfsRouteListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        RouteIdRole = Qt::UserRole + 1,
        ShortNameRole,
        LongNameRole,
        ColorRole,
        HeadsignRole,
        ShapeRole,
        PointCountRole
    };

    explicit GtfsRouteListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void resetFrom(const sit::GtfsDataset& data);

private:
    struct Row {
        QString id;
        QString shortName;
        QString longName;
        QString color;
        QString headsign;
        QVariantList shape;
    };
    std::vector<Row> rows_;
};
