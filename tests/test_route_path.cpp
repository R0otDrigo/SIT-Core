#include "check.h"
#include "model/Network.h"
#include "model/Route.h"

using namespace sit;

static std::unique_ptr<Network> makeNet(bool withPath) {
    auto net = std::make_unique<Network>(1);
    auto a = StopFactory::create("terminal", "A", "A", {0, 0}, 100);
    auto b = StopFactory::create("regular", "B", "B", {100, 0}, 100);
    auto c = StopFactory::create("terminal", "C", "C", {100, 100}, 100);
    for (auto& s : {a, b, c}) net->addStop(s);

    auto t = std::make_shared<Route>("T", "Troncal", RouteKind::Trunk, "#f00");
    t->addStop(a); t->addStop(b); t->addStop(c);
    if (withPath) {
        CHECK(t->setPath({{0, 0}, {50, 0}, {100, 0}, {100, 50}, {100, 100}}));
        CHECK(t->hasPath());
        CHECK_NEAR(t->stopAnchor(0), 0.0, 1e-9);
        CHECK_NEAR(t->stopAnchor(1), 100.0, 1e-9);
        CHECK_NEAR(t->stopAnchor(2), 200.0, 1e-9);
        CHECK_NEAR(t->pointAtDistance(125.0).x, 100.0, 1e-9);
        CHECK_NEAR(t->pointAtDistance(125.0).y, 25.0, 1e-9);
    }
    net->addRoute(t);
    net->setFleetLimit(2);
    net->calibrateGeo();
    return net;
}

int main() {
    {
        auto net = makeNet(true);
        Vehicle& bus = net->addVehicle(VehicleType::Conventional, "T", 0);
        CHECK(bus.position() == (Position{0, 0}));
        CHECK_NEAR(bus.pathDistance(), 0.0, 1e-9);

        for (int i = 0; i < 15; ++i) net->step(0.5);
        CHECK_NEAR(bus.position().x, 75.0, 1e-6);
        CHECK_NEAR(bus.position().y, 0.0, 1e-6);

        for (int i = 0; i < 20; ++i) net->step(0.5);
        CHECK_NEAR(bus.position().x, 100.0, 1e-6);
        CHECK(bus.position().y > 10.0);

        net->stops().byId("C")->addWaiting(10);
        for (int i = 0; i < 20; ++i) net->step(0.5);
        CHECK(bus.passengers() > 0);

        for (int i = 0; i < 40; ++i) net->step(0.5);
        CHECK(bus.position().y < 100.0);
        CHECK(bus.pathDistance() < 150.0);

        const Snapshot snap = net->snapshot();
        CHECK(snap.routes.front().path.size() == 5);
        CHECK(snap.routes.front().waypoints.size() == 3);
    }

    {
        auto net = makeNet(true);
        auto straight = std::make_shared<Route>("S", "Recto", RouteKind::Trunk, "#0f0");
        straight->addStop(net->stops().byId("A"));
        straight->addStop(net->stops().byId("B"));
        CHECK(!straight->setPath({{100, 100}, {0, 0}}));
        CHECK(!straight->hasPath());
    }

    {
        auto net = std::make_unique<Network>(1);
        auto a = StopFactory::create("terminal", "A", "A", {0, 0}, 100);
        auto c = StopFactory::create("terminal", "C", "C", {100, 100}, 100);
        net->addStop(a); net->addStop(c);
        auto t = std::make_shared<Route>("AC", "Recto", RouteKind::Trunk, "#f00");
        t->addStop(a); t->addStop(c);
        net->addRoute(t);
        net->setFleetLimit(1);
        net->calibrateGeo();

        Vehicle& bus = net->addVehicle(VehicleType::Conventional, "AC", 0);
        for (int i = 0; i < 10; ++i) net->step(0.5);
        CHECK_NEAR(bus.position().x, 35.355, 1e-2);
        CHECK_NEAR(bus.position().y, 35.355, 1e-2);
        CHECK(net->snapshot().routes.front().path.empty());
    }

    return 0;
}
