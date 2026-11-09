#include "check.h"
#include "common/Exceptions.h"
#include "model/VehicleFactory.h"

using namespace sit;

int main() {
    auto bus = VehicleFactory::create(VehicleType::Articulated, 1);
    CHECK(bus->capacity() == 160);

    bus->board(100);
    CHECK(bus->passengers() == 100);
    CHECK_THROWS(bus->board(100), CapacityExceededException);
    CHECK(bus->alight(30) == 30);

    auto feeder = VehicleFactory::create("alimentador", 2);
    CHECK(feeder->typeName() == "Alimentador");
    CHECK_THROWS(VehicleFactory::create("avion", 3), InvalidVehicleTypeException);

    bus->moveTowards({100, 0}, 1.0);     // velocidad 8
    CHECK(bus->position() == (Position{8, 0}));
    return 0;
}
