#include "model/TrunkBus.h"

namespace teoto {

TrunkBus::TrunkBus(QString id, std::shared_ptr<Route> route, int capacity, double maxSpeed)
    : Bus(std::move(id), capacity, maxSpeed, std::move(route))
{
}

VehicleType TrunkBus::type() const
{
    return VehicleType::TrunkBus;
}

} // namespace teoto
