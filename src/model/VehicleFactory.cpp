#include "model/VehicleFactory.h"

#include <utility>

#include "model/FeederBus.h"
#include "model/TrunkBus.h"

namespace teoto {

std::unique_ptr<Vehicle> VehicleFactory::create(
    VehicleType type, QString id, std::shared_ptr<Route> route)
{
    switch (type) {
    case VehicleType::TrunkBus:
        return std::make_unique<TrunkBus>(std::move(id), std::move(route));
    case VehicleType::FeederBus:
        return std::make_unique<FeederBus>(std::move(id), std::move(route));
    }
    return nullptr;
}

std::unique_ptr<Vehicle> VehicleFactory::create(
    const QString &type, QString id, std::shared_ptr<Route> route)
{
    if (type.compare(QStringLiteral("troncal"), Qt::CaseInsensitive) == 0
        || type.compare(QStringLiteral("trunkbus"), Qt::CaseInsensitive) == 0) {
        return create(VehicleType::TrunkBus, std::move(id), std::move(route));
    }
    if (type.compare(QStringLiteral("alimentador"), Qt::CaseInsensitive) == 0
        || type.compare(QStringLiteral("feederbus"), Qt::CaseInsensitive) == 0) {
        return create(VehicleType::FeederBus, std::move(id), std::move(route));
    }
    return nullptr;
}

} // namespace teoto
