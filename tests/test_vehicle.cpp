#include "check.h"
#include "model/VehicleFactory.h"
#include "model/Bus.h"
#include "model/Route.h"

using namespace teoto;

int main() {
    auto route = std::make_shared<Route>("R1", "Ruta de prueba");
    route->addStop(Stop("S1", "Inicio", QGeoCoordinate(-16.4290, -71.5210)));
    route->addStop(Stop("S2", "Centro", QGeoCoordinate(-16.3989, -71.5369)));

    auto vehicle = VehicleFactory::create(VehicleType::TrunkBus, "BUS-1", route);
    CHECK(vehicle != nullptr);
    CHECK(vehicle->type() == VehicleType::TrunkBus);
    CHECK(vehicle->capacity() == 90);

    auto *bus = dynamic_cast<Bus *>(vehicle.get());
    CHECK(bus != nullptr);
    for (int i = 0; i < bus->capacity(); ++i)
        CHECK(bus->boardPassenger());
    CHECK(!bus->boardPassenger());
    CHECK(bus->occupancy() == 90);
    CHECK(bus->disembark(30) == 30);
    CHECK(bus->occupancy() == 60);

    const auto start = bus->position();
    bus->update(10.0);
    CHECK(bus->position() != start);

    const auto feeder = VehicleFactory::create(QStringLiteral("alimentador"), "F-1", route);
    CHECK(feeder != nullptr);
    CHECK(feeder->type() == VehicleType::FeederBus);
    CHECK(feeder->capacity() == 40);
    CHECK(VehicleFactory::create(QStringLiteral("unknown"), "X", route) == nullptr);
    return 0;
}
