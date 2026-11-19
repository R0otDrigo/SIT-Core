#include "view/UserBridge.h"
#include <QHash>
#include <QStringList>
#include <algorithm>
#include "view/MapModels.h"
#include "view/UserBroadcast.h"

UserBridge::UserBridge(QObject* parent)
    : QObject(parent), vehicleModel_(new VehicleListModel(this)) {}

QAbstractListModel* UserBridge::vehicleModel() const { return vehicleModel_; }

QString UserBridge::vehicleId() const
{
    return assignedId_ >= 0 ? QString::number(assignedId_) : QString();
}

QString UserBridge::placa() const
{
    return assignedId_ >= 0 ? QStringLiteral("SIT-%1").arg(assignedId_, 4, 10, QChar('0'))
                            : QString();
}

QString UserBridge::antecedentes() const
{
    if (broadcast_ && !broadcast_->antecedentes().trimmed().isEmpty())
        return broadcast_->antecedentes();
    return tr("Sin antecedentes registrados");
}

QString UserBridge::evento() const
{
    if (broadcast_ && !broadcast_->evento().trimmed().isEmpty())
        return broadcast_->evento();
    if (!simulationEvent_.isEmpty()) return simulationEvent_;
    return tr("Sin eventos activos");
}

void UserBridge::setBroadcast(UserBroadcast* broadcast)
{
    if (broadcast_ == broadcast) return;
    if (broadcast_) disconnect(broadcast_, nullptr, this, nullptr);
    broadcast_ = broadcast;
    if (broadcast_) {
        connect(broadcast_, &UserBroadcast::antecedentesChanged,
                this, &UserBridge::broadcastChanged);
        connect(broadcast_, &UserBroadcast::eventoChanged,
                this, &UserBridge::broadcastChanged);
    }
    emit broadcastChanged();
}

void UserBridge::setUser(const QString& userId)
{
    if (userId_ == userId) return;
    userId_ = userId;
    displayName_ = userId.isEmpty() ? tr("Usuario") : tr("Usuario %1").arg(userId.left(6));
    assignedId_ = -1;
    lastGeo_ = {};
    kilometers_ = 0.0;
    emit userChanged();
    clearVehicle();
}

void UserBridge::clearVehicle()
{
    routeId_.clear();
    routeLabel_.clear();
    vehicleType_.clear();
    color_.clear();
    passengers_ = 0;
    capacity_ = 0;
    dwelling_ = false;
    latitude_ = -16.3989;
    longitude_ = -71.5369;
    info_ = tr("Sin bus asignado");
    if (!simulationEvent_.isEmpty()) {
        simulationEvent_.clear();
        emit broadcastChanged();
    }
    vehicleModel_->sync({}, {});
    emit vehicleChanged();
}

void UserBridge::update(const sit::Snapshot& snapshot)
{
    QHash<QString, QString> routeColors;
    for (const auto& route : snapshot.routes)
        routeColors.insert(QString::fromStdString(route.id),
                           QString::fromStdString(route.color));

    const sit::VehicleView* assigned = nullptr;
    for (const auto& vehicle : snapshot.vehicles) {
        if (assignedId_ >= 0 && vehicle.id == assignedId_) {
            assigned = &vehicle;
            break;
        }
    }
    if (!assigned && !snapshot.vehicles.empty()) {
        const std::size_t index =
            static_cast<std::size_t>(qHash(userId_)) % snapshot.vehicles.size();
        assignedId_ = snapshot.vehicles[index].id;
        lastGeo_ = {};
        assigned = &snapshot.vehicles[index];
    }
    if (!assigned) {
        assignedId_ = -1;
        clearVehicle();
        return;
    }

    if (assigned->geo.isValid()) {
        if (lastGeo_.isValid())
            kilometers_ += lastGeo_.distanceTo(assigned->geo) / 1000.0;
        lastGeo_ = assigned->geo;
        latitude_ = assigned->geo.latitude;
        longitude_ = assigned->geo.longitude;
    }

    routeId_ = QString::fromStdString(assigned->routeId);
    routeLabel_.clear();
    for (const auto& route : snapshot.routes) {
        if (route.id == assigned->routeId) {
            routeLabel_ = QString::fromStdString(route.name);
            break;
        }
    }
    vehicleType_ = QString::fromStdString(assigned->type);
    color_ = routeColors.value(routeId_, QStringLiteral("#6b7280"));
    passengers_ = assigned->passengers;
    capacity_ = assigned->capacity;
    dwelling_ = assigned->dwelling;

    QStringList parts;
    parts << tr("Ruta %1").arg(routeLabel_.isEmpty() ? routeId_ : routeLabel_);
    parts << (dwelling_ ? tr("En paradero") : tr("En ruta"));
    if (capacity_ > 0 && passengers_ >= capacity_) parts << tr("Bus lleno");
    info_ = parts.join(QStringLiteral(" | "));

    QString nextEvent;
    for (const auto& event : snapshot.events) {
        if (!assigned->routeId.empty() && event.routeId != assigned->routeId) continue;
        nextEvent = tr("%1 en %2 (%3 s)")
                        .arg(QString::fromStdString(event.name))
                        .arg(QString::fromStdString(event.routeId))
                        .arg(static_cast<int>(event.remaining));
        break;
    }
    if (nextEvent != simulationEvent_) {
        simulationEvent_ = nextEvent;
        emit broadcastChanged();
    }

    vehicleModel_->sync({*assigned}, routeColors);
    emit vehicleChanged();
}
