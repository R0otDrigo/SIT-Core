#pragma once

#include "model/Bus.h"
#include "utils/Constants.h"

namespace teoto {

/// Bus alimentador: menor capacidad y velocidad (OBJ-05).
class FeederBus : public Bus
{
public:
    explicit FeederBus(QString id,
                       std::shared_ptr<Route> route,
                       int capacity = constants::FeederBusCapacity,
                       double maxSpeed = constants::FeederBusMaxSpeedKmh);

    VehicleType type() const override;
};

} // namespace teoto
