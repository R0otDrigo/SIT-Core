#include "view/MapModels.h"
#include <QGeoCoordinate>
#include <QSet>

namespace {

QVariantList toCoordinates(const std::vector<sit::GeoCoordinate>& points) {
    QVariantList out;
    out.reserve(static_cast<int>(points.size()));
    for (const auto& p : points) {
        if (!p.isValid()) continue;
        out.append(QVariant::fromValue(QGeoCoordinate(p.latitude, p.longitude)));
    }
    return out;
}

QVariantList toCoordinates(const sit::GtfsRoute& route) {
    return toCoordinates(route.shape);
}

QVector<int> routeRoles() {
    return {RouteListModel::RouteIdRole, RouteListModel::NameRole, RouteListModel::ColorRole,
            RouteListModel::TrunkRole, RouteListModel::WaypointsRole, RouteListModel::AffectedRole,
            RouteListModel::PathRole};
}

QVector<int> stopRoles() {
    return {StopListModel::StopIdRole, StopListModel::NameRole, StopListModel::LatitudeRole,
            StopListModel::LongitudeRole, StopListModel::WaitingRole, StopListModel::SaturationRole,
            StopListModel::ClosedRole};
}

QVector<int> vehicleRoles() {
    return {VehicleListModel::VehicleIdRole, VehicleListModel::RouteIdRole,
            VehicleListModel::TypeRole,     VehicleListModel::LatitudeRole,
            VehicleListModel::LongitudeRole, VehicleListModel::PassengersRole,
            VehicleListModel::CapacityRole, VehicleListModel::DwellingRole,
            VehicleListModel::ColorRole};
}

QVector<int> gtfsRoles() {
    return {GtfsRouteListModel::RouteIdRole, GtfsRouteListModel::ShortNameRole,
            GtfsRouteListModel::LongNameRole, GtfsRouteListModel::ColorRole,
            GtfsRouteListModel::HeadsignRole, GtfsRouteListModel::ShapeRole,
            GtfsRouteListModel::PointCountRole};
}

} // namespace

int RouteListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant RouteListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(rows_.size()))
        return {};
    const Row& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case RouteIdRole: return row.id;
    case NameRole: return row.name;
    case ColorRole: return row.color;
    case TrunkRole: return row.trunk;
    case WaypointsRole: return row.waypoints;
    case AffectedRole: return row.affected;
    case PathRole: return row.path;
    default: return {};
    }
}

QHash<int, QByteArray> RouteListModel::roleNames() const {
    return {{RouteIdRole, "routeId"},     {NameRole, "name"},           {ColorRole, "color"},
            {TrunkRole, "trunk"},         {WaypointsRole, "waypoints"}, {AffectedRole, "affected"},
            {PathRole, "path"}};
}

void RouteListModel::resetFrom(const std::vector<sit::RouteView>& routes) {
    beginResetModel();
    rows_.clear();
    rows_.reserve(routes.size());
    for (const auto& route : routes) {
        Row row;
        row.id = QString::fromStdString(route.id);
        row.name = QString::fromStdString(route.name);
        row.color = QString::fromStdString(route.color);
        row.trunk = route.trunk;
        row.waypoints = toCoordinates(route.waypoints);
        row.path = toCoordinates(route.path);
        rows_.push_back(std::move(row));
    }
    endResetModel();
}

void RouteListModel::setAffectedRoutes(const QStringList& affected) {
    const QSet<QString> hit(affected.begin(), affected.end());
    for (int i = 0; i < static_cast<int>(rows_.size()); ++i) {
        const bool next = hit.contains(rows_[static_cast<std::size_t>(i)].id);
        if (next == rows_[static_cast<std::size_t>(i)].affected) continue;
        rows_[static_cast<std::size_t>(i)].affected = next;
        const QModelIndex idx = index(i, 0);
        emit dataChanged(idx, idx, {AffectedRole});
    }
}

int StopListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant StopListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(rows_.size()))
        return {};
    const sit::StopView& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case StopIdRole: return QString::fromStdString(row.id);
    case NameRole: return QString::fromStdString(row.name);
    case LatitudeRole: return row.geo.latitude;
    case LongitudeRole: return row.geo.longitude;
    case WaitingRole: return row.waiting;
    case SaturationRole: return row.saturation;
    case ClosedRole: return row.closed;
    default: return {};
    }
}

QHash<int, QByteArray> StopListModel::roleNames() const {
    return {{StopIdRole, "stopId"},       {NameRole, "name"},
            {LatitudeRole, "latitude"},   {LongitudeRole, "longitude"},
            {WaitingRole, "waiting"},     {SaturationRole, "saturation"},
            {ClosedRole, "closed"}};
}

void StopListModel::sync(const std::vector<sit::StopView>& stops) {
    if (rows_.size() != stops.size()) {
        beginResetModel();
        rows_ = stops;
        endResetModel();
        return;
    }
    if (stops.empty()) return;
    rows_ = stops;
    emit dataChanged(index(0, 0), index(static_cast<int>(rows_.size()) - 1, 0), stopRoles());
}

int VehicleListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant VehicleListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(rows_.size()))
        return {};
    const Row& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case VehicleIdRole: return row.view.id;
    case RouteIdRole: return QString::fromStdString(row.view.routeId);
    case TypeRole: return QString::fromStdString(row.view.type);
    case LatitudeRole: return row.view.geo.latitude;
    case LongitudeRole: return row.view.geo.longitude;
    case PassengersRole: return row.view.passengers;
    case CapacityRole: return row.view.capacity;
    case DwellingRole: return row.view.dwelling;
    case ColorRole: return row.color;
    default: return {};
    }
}

QHash<int, QByteArray> VehicleListModel::roleNames() const {
    return {{VehicleIdRole, "vehicleId"},   {RouteIdRole, "routeId"},
            {TypeRole, "type"},             {LatitudeRole, "latitude"},
            {LongitudeRole, "longitude"},   {PassengersRole, "passengers"},
            {CapacityRole, "capacity"},     {DwellingRole, "dwelling"},
            {ColorRole, "color"}};
}

void VehicleListModel::sync(const std::vector<sit::VehicleView>& vehicles,
                            const QHash<QString, QString>& routeColors) {
    if (rows_.size() != vehicles.size()) {
        beginResetModel();
        rows_.clear();
        rows_.reserve(vehicles.size());
        for (const auto& vehicle : vehicles)
            rows_.push_back({vehicle, routeColors.value(QString::fromStdString(vehicle.routeId),
                                                        QStringLiteral("#6b7280"))});
        endResetModel();
        return;
    }
    if (vehicles.empty()) return;
    for (std::size_t i = 0; i < vehicles.size(); ++i)
        rows_[i] = {vehicles[i],
                    routeColors.value(QString::fromStdString(vehicles[i].routeId),
                                      QStringLiteral("#6b7280"))};
    emit dataChanged(index(0, 0), index(static_cast<int>(rows_.size()) - 1, 0), vehicleRoles());
}

int GtfsRouteListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant GtfsRouteListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(rows_.size()))
        return {};
    const Row& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case RouteIdRole: return row.id;
    case ShortNameRole: return row.shortName;
    case LongNameRole: return row.longName;
    case ColorRole: return row.color;
    case HeadsignRole: return row.headsign;
    case ShapeRole: return row.shape;
    case PointCountRole: return row.shape.size();
    default: return {};
    }
}

QHash<int, QByteArray> GtfsRouteListModel::roleNames() const {
    return {{RouteIdRole, "routeId"},     {ShortNameRole, "shortName"},
            {LongNameRole, "longName"},   {ColorRole, "color"},
            {HeadsignRole, "headsign"},   {ShapeRole, "shape"},
            {PointCountRole, "pointCount"}};
}

void GtfsRouteListModel::resetFrom(const sit::GtfsDataset& data) {
    beginResetModel();
    rows_.clear();
    rows_.reserve(data.routes.size());
    for (const auto& route : data.routes) {
        Row row;
        row.id = QString::fromStdString(route.routeId);
        row.shortName = QString::fromStdString(route.shortName);
        row.longName = QString::fromStdString(route.longName);
        row.color = QString::fromStdString(route.color);
        row.headsign = QString::fromStdString(route.headsign);
        row.shape = toCoordinates(route);
        if (!row.shape.isEmpty()) rows_.push_back(std::move(row));
    }
    endResetModel();
}
