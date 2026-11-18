#include <memory>
#include "check.h"
#include "model/CsvRouteAdapter.h"
#include "model/Network.h"
#include "model/NetworkBuilder.h"

using namespace sit;

int main() {
    // --- Tipo geografico basico ---
    const GeoCoordinate a{-16.3989, -71.5369};
    const GeoCoordinate b{-16.4989, -71.5369};
    CHECK(a.isValid());
    CHECK(!GeoCoordinate{}.isValid());
    CHECK(a.distanceTo(b) > 10000.0 && a.distanceTo(b) < 12000.0);

    // --- Red legacy sin lat/lon: se proyecta desde x/y ---
    Network legacy(1);
    auto s1 = StopFactory::create("terminal", "A", "A", {0, 0}, 50);
    auto s2 = StopFactory::create("regular", "B", "B", {100, 0}, 50);
    auto s3 = StopFactory::create("terminal", "C", "C", {200, 0}, 50);
    for (const auto& stop : {s1, s2, s3}) legacy.addStop(stop);
    auto route = std::make_shared<Route>("T", "Troncal", RouteKind::Trunk, "#f00");
    route->addStop(s1);
    route->addStop(s2);
    route->addStop(s3);
    legacy.addRoute(route);
    legacy.calibrateGeo();
    CHECK(legacy.stops().byId("A")->hasGeo());
    CHECK(legacy.stops().byId("C")->hasGeo());
    CHECK(legacy.geoBounds().first.isValid());
    CHECK(legacy.geoBounds().second.isValid());
    CHECK(legacy.geoCenter().isValid());
    CHECK(legacy.geoFor(Position{50, 20}).isValid());
    const Position roundTrip = legacy.positionFor(legacy.geoFor(Position{50, 20}));
    CHECK_NEAR(roundTrip.x, 50.0, 1e-6);
    CHECK_NEAR(roundTrip.y, 20.0, 1e-6);

    // --- Red real con columnas LAT/LON ---
    CsvRouteAdapter csv(std::string(SIT_DATA_DIR) + "/network.csv");
    NetworkData data = csv.load();
    CHECK(data.stops.size() == 288);
    CHECK(data.stops.front().geo.isValid());

    auto network = NetworkBuilder().withData(data).build();
    CHECK(network->stops().byId("10270810751")->hasGeo());
    CHECK_NEAR(network->stops().byId("10270810751")->geoPosition().latitude, -16.4044512, 1e-6);
    CHECK_NEAR(network->stops().byId("10270810751")->geoPosition().longitude, -71.4896626, 1e-6);
    CHECK(network->routes().byId("17643070")->hasGeo());
    CHECK(network->routes().byId("17643070")->waypoints().size() == 36);
    CHECK(network->routes().byId("17643070")->geoLength() > 0.0);
    CHECK(network->routes().byId("17643070")->hasPath());

    const Snapshot snap = network->snapshot();
    CHECK(!snap.stops.empty() && snap.stops.front().geo.isValid());
    CHECK(!snap.routes.empty() && snap.routes.front().waypoints.size() == 36);
    CHECK(!snap.vehicles.empty() && snap.vehicles.front().geo.isValid());
    return 0;
}
