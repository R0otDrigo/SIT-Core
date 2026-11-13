#include "view/MapBridge.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QResource>
#include <QStringList>
#include <algorithm>
#include <cmath>
#include <vector>
#include "common/Exceptions.h"
#include "model/GtfsData.h"
#include "model/GtfsRouteAdapter.h"
#include "view/MapModels.h"

MapBridge::MapBridge(QObject* parent)
    : QObject(parent)
    , routeModel_(new RouteListModel(this))
    , stopModel_(new StopListModel(this))
    , vehicleModel_(new VehicleListModel(this))
    , gtfsModel_(new GtfsRouteListModel(this)) {
    Q_INIT_RESOURCE(qml);
}

QAbstractListModel* MapBridge::routeModel() const { return routeModel_; }
QAbstractListModel* MapBridge::stopModel() const { return stopModel_; }
QAbstractListModel* MapBridge::vehicleModel() const { return vehicleModel_; }
QAbstractListModel* MapBridge::gtfsRouteModel() const { return gtfsModel_; }

void MapBridge::setShowGtfs(bool show) {
    if (show == showGtfs_) return;
    showGtfs_ = show;
    emit showGtfsChanged();
}

void MapBridge::loadGtfs(const QString& dataDir) {
    QStringList candidates;
    if (!dataDir.isEmpty()) candidates << dataDir;
    candidates << QDir(QCoreApplication::applicationDirPath()).filePath("data");
#ifdef SIT_DATA_DIR
    candidates << QString::fromUtf8(SIT_DATA_DIR);
#endif

    QString chosen;
    for (const QString& candidate : candidates) {
        if (QFileInfo::exists(candidate + "/routes.txt") &&
            QFileInfo::exists(candidate + "/trips.txt") &&
            QFileInfo::exists(candidate + "/shapes.txt")) {
            chosen = candidate;
            break;
        }
    }

    if (chosen.isEmpty()) {
        gtfsStatus_ = QStringLiteral("GTFS no disponible (faltan routes/trips/shapes)");
        emit gtfsStatusChanged();
        return;
    }

    try {
        sit::GtfsRouteAdapter adapter(chosen.toStdString());
        const sit::GtfsDataset data = adapter.load();
        gtfsModel_->resetFrom(data);
        gtfsStatus_ = QStringLiteral("GTFS %1 rutas / %2 puntos")
                          .arg(static_cast<qulonglong>(data.routes.size()))
                          .arg(static_cast<qulonglong>(data.shapePointCount));
    } catch (const sit::SitException& e) {
        gtfsStatus_ = QStringLiteral("GTFS: %1").arg(QString::fromStdString(e.what()));
    }
    emit gtfsStatusChanged();
}

void MapBridge::update(const sit::Snapshot& snapshot) {
    lastSnapshot_ = snapshot;
    if (!routesInitialized_ && !snapshot.routes.empty()) {
        routeModel_->resetFrom(snapshot.routes);
        routesInitialized_ = true;
        updateViewFrom(snapshot);
    }

    QStringList affected;
    QStringList banner;
    for (const auto& event : snapshot.events) {
        const QString routeId = QString::fromStdString(event.routeId);
        affected << routeId;
        banner << QStringLiteral("%1 en %2 (%3 s)")
                      .arg(QString::fromStdString(event.name), routeId)
                      .arg(static_cast<int>(event.remaining));
    }
    routeModel_->setAffectedRoutes(affected);

    stopModel_->sync(snapshot.stops);

    QHash<QString, QString> routeColors;
    for (const auto& route : snapshot.routes)
        routeColors.insert(QString::fromStdString(route.id), QString::fromStdString(route.color));
    vehicleModel_->sync(snapshot.vehicles, routeColors);

    const QString nextEventText =
        banner.isEmpty() ? QString() : QStringLiteral("ALERTA: ") + banner.join(QStringLiteral("  |  "));
    if (nextEventText != eventText_) {
        eventText_ = nextEventText;
        emit eventTextChanged();
    }
}

void MapBridge::updateViewFrom(const sit::Snapshot& snapshot) {
    double minLat = 90.0, maxLat = -90.0, minLon = 180.0, maxLon = -180.0;
    bool any = false;
    auto include = [&](const sit::GeoCoordinate& geo) {
        if (!geo.isValid()) return;
        any = true;
        minLat = std::min(minLat, geo.latitude);
        maxLat = std::max(maxLat, geo.latitude);
        minLon = std::min(minLon, geo.longitude);
        maxLon = std::max(maxLon, geo.longitude);
    };
    for (const auto& stop : snapshot.stops) include(stop.geo);
    for (const auto& route : snapshot.routes)
        for (const auto& geo : route.waypoints) include(geo);

    if (!any) return;

    minLatitude_ = minLat;
    maxLatitude_ = maxLat;
    minLongitude_ = minLon;
    maxLongitude_ = maxLon;
    centerLatitude_ = (minLat + maxLat) / 2.0;
    centerLongitude_ = (minLon + maxLon) / 2.0;

    const double span = std::max(maxLat - minLat, maxLon - minLon);
    if (span > 1e-9)
        initialZoom_ = std::clamp(13.5 - std::log2(span / 0.05), 10.5, 15.0);

    emit viewChanged();
}

void MapBridge::selectVehicle(const QString& vehicleId) {
    if (vehicleId.isEmpty()) {
        clearSelection();
        return;
    }

    const sit::VehicleView* vehicle = nullptr;
    for (const auto& item : lastSnapshot_.vehicles) {
        if (QString::number(item.id) == vehicleId) {
            vehicle = &item;
            break;
        }
    }
    if (!vehicle) {
        clearSelection();
        return;
    }

    QString routeLabel = QString::fromStdString(vehicle->routeId);
    for (const auto& route : lastSnapshot_.routes) {
        if (route.id == vehicle->routeId) {
            routeLabel = QStringLiteral("%1 - %2")
                             .arg(QString::fromStdString(route.id),
                                  QString::fromStdString(route.name));
            break;
        }
    }

    selectedVehicleId_ = vehicleId;
    selectedVehicleStats_ = QStringLiteral("Vehículo %1 | Ruta: %2 | Pasajeros: %3/%4 | %5")
                                .arg(vehicleId, routeLabel)
                                .arg(vehicle->passengers)
                                .arg(vehicle->capacity)
                                .arg(vehicle->dwelling ? QStringLiteral("en paradero")
                                                       : QStringLiteral("en ruta"));
    emit selectionChanged();
    emit busSelected(vehicle->id, QString::fromStdString(vehicle->type), routeLabel,
                     vehicle->passengers, vehicle->capacity, vehicle->dwelling,
                     QString::fromStdString(vehicle->nextStopName));
}

void MapBridge::clearSelection() {
    if (selectedVehicleId_.isEmpty() && selectedVehicleStats_.isEmpty()) return;
    selectedVehicleId_.clear();
    selectedVehicleStats_.clear();
    emit selectionChanged();
}

void MapBridge::setIncomingCall(const QString& vehicleId, const QString& name,
                                const QString& status) {
    incomingCallVehicleId_ = vehicleId;
    incomingCallName_ = name;
    incomingCallStatus_ = status;
    emit callChanged();
}

void MapBridge::clearIncomingCall() {
    if (incomingCallVehicleId_.isEmpty() && incomingCallName_.isEmpty() &&
        incomingCallStatus_.isEmpty())
        return;
    incomingCallVehicleId_.clear();
    incomingCallName_.clear();
    incomingCallStatus_.clear();
    emit callChanged();
}
