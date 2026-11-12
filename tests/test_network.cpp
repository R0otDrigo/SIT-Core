#include "check.h"
#include "common/Exceptions.h"
#include "model/Network.h"

using namespace sit;

// Red minima: A --100-- B --100-- C (troncal) y una alimentadora F1: C -- B.
static std::unique_ptr<Network> makeNet() {
    auto net = std::make_unique<Network>(1);
    auto a = StopFactory::create("terminal", "A", "A", {0, 0}, 100);
    auto b = StopFactory::create("regular", "B", "B", {100, 0}, 100);
    auto c = StopFactory::create("terminal", "C", "C", {200, 0}, 100);
    for (auto& s : {a, b, c}) net->addStop(s);
    auto t = std::make_shared<Route>("T", "Troncal", RouteKind::Trunk, "#f00");
    t->addStop(a); t->addStop(b); t->addStop(c);
    auto f = std::make_shared<Route>("F", "Alim", RouteKind::Feeder, "#0f0");
    f->addStop(c); f->addStop(b);
    net->addRoute(t); net->addRoute(f);
    net->setFleetLimit(2);
    return net;
}

int main() {
    auto net = makeNet();

    // --- Errores controlados ---
    CHECK_THROWS(net->addVehicle(VehicleType::Conventional, "ZZ"), InvalidRouteException);
    CHECK_THROWS(net->addVehicle(VehicleType::Feeder, "T"), InvalidRouteException);          // tipo incompatible
    CHECK_THROWS(net->addVehicle(VehicleType::Conventional, "F"), InvalidRouteException);

    // --- Un bus recorre la ruta y recoge pasajeros ---
    Vehicle& bus = net->addVehicle(VehicleType::Conventional, "T", 0);
    CHECK(bus.position() == (Position{0, 0}));
    net->stops().byId("B")->addWaiting(30);
    for (int i = 0; i < 40; ++i) net->step(0.5);            // 20 s: 100u / 10 u/s = 10 s hasta B
    CHECK(bus.passengers() > 0);                            // subio gente en B
    CHECK(net->stops().byId("B")->waiting() < 30);

    // --- Limite de flota ---
    net->addVehicle(VehicleType::Feeder, "F");
    CHECK_THROWS(net->addVehicle(VehicleType::Conventional, "T"), FleetLimitExceededException);
    CHECK(net->activeVehicles() == 2);

    // --- Eventos: accidente frena la ruta; protesta cierra paradero ---
    const auto& route = *net->routes().byId("T");
    CHECK_NEAR(net->speedFactorFor(route), 1.0, 1e-9);
    net->triggerEvent(EventType::Accident, "T");
    CHECK_NEAR(net->speedFactorFor(route), 0.2, 1e-9);
    CHECK_NEAR(net->speedFactorFor(*net->routes().byId("F")), 1.0, 1e-9);
    net->triggerEvent(EventType::Protest, "T", "B");
    CHECK(net->isStopClosed(*net->stops().byId("B")));
    CHECK(!net->isStopClosed(*net->stops().byId("A")));
    CHECK_THROWS(net->triggerEvent(EventType::Protest, "T", "NOPE"), InvalidEventException);
    for (int i = 0; i < 400; ++i) net->step(0.5);            // 200 s: todos expiran
    CHECK(net->events().empty());
    CHECK_NEAR(net->speedFactorFor(route), 1.0, 1e-9);

    // --- Estadisticas, reloj y retiro de buses ---
    CHECK(net->transported() > 0);
    CHECK(net->clockString().size() == 5);
    CHECK(net->demandMultiplier() > 0.0);
    CHECK(!net->drainLog().empty());
    CHECK(net->drainLog().empty());

    auto snap = net->snapshot();
    CHECK(snap.stops.size() == 3 && snap.routes.size() == 2 && snap.vehicles.size() == 2);

    // Dejamos buses vacios para poder retirarlos.
    for (auto& v : net->vehicles()) v->alight(1000);
    CHECK(net->retireVehicle("T"));
    CHECK(!net->retireVehicle("T"));
    return 0;
}
