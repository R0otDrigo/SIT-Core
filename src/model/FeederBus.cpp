#include "model/FeederBus.h"

namespace teoto {

FeederBus::FeederBus(QString id, std::shared_ptr<Route> route, int capacity, double maxSpeed)
    : Bus(std::move(id), capacity, maxSpeed, std::move(route))
{
}

VehicleType FeederBus::type() const
{
    return VehicleType::FeederBus;
}

} // namespace teoto
