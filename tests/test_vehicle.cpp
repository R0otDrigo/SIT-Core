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
    CHECK(bus->freeSeats() == 90);

    auto feeder = VehicleFactory::create("alimentador", 2);
    CHECK(feeder->typeName() == "Alimentador");
    CHECK_THROWS(VehicleFactory::create("avion", 3), InvalidVehicleTypeException);

    bus->moveTowards({100, 0}, 1.0);     // velocidad 8
    CHECK(bus->position() == (Position{8, 0}));

    auto route = std::make_shared<Route>("T1", "Troncal", RouteKind::Trunk, "#336699");
    route->addStop(std::make_shared<RegularStop>("A", "Inicio", Position{0, 0}, 100));
    route->addStop(std::make_shared<RegularStop>("B", "Centro", Position{10, 0}, 100));
    route->addStop(std::make_shared<RegularStop>("C", "Terminal", Position{20, 0}, 100));
    bus->assignRoute(route);
    CHECK(bus->nextStop()->name() == "Centro");
    CHECK(bus->update(2.0, 1.0));
    CHECK(bus->targetStop()->name() == "Centro");
    CHECK(bus->nextStop()->name() == "Terminal");
    bus->finishService(0.0);
    bus->update(0.1, 1.0);
    CHECK(bus->nextStop()->name() == "Terminal");
    CHECK(bus->update(2.0, 1.0));
    CHECK(bus->nextStop()->name() == "Centro"); // da la vuelta en el terminal

    auto feederAtEnd = VehicleFactory::create(VehicleType::Feeder, 3);
    feederAtEnd->assignRoute(route, 2);
    CHECK(feederAtEnd->nextStop()->name() == "Centro");
    return 0;
}
