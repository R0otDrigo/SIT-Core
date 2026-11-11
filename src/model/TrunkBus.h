#pragma once

#include "model/Bus.h"
#include "utils/Constants.h"

namespace teoto {

/// Bus troncal: mayor capacidad y velocidad (OBJ-05).
class TrunkBus : public Bus
{
public:
    explicit TrunkBus(QString id,
                      std::shared_ptr<Route> route,
                      int capacity = constants::TrunkBusCapacity,
                      double maxSpeed = constants::TrunkBusMaxSpeedKmh);

    VehicleType type() const override;
};

} // namespace teoto
