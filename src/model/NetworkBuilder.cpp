#include "model/NetworkBuilder.h"
#include <set>
#include "common/Exceptions.h"

namespace sit {

static RouteKind parseKind(const std::string& k) {
    if (k == "trunk" || k == "troncal") return RouteKind::Trunk;
    if (k == "feeder" || k == "alimentadora") return RouteKind::Feeder;
    throw DataLoadException("tipo de ruta desconocido: " + k);
}

std::unique_ptr<Network> NetworkBuilder::build() const {
    if (data_.stops.empty() || data_.routes.empty()) throw DataLoadException("la red no tiene paraderos o rutas");

    auto net = std::make_unique<Network>(seed_);
    net->setFleetLimit(data_.fleetLimit);

    std::set<std::string> seen;
    for (const auto& s : data_.stops) {
        if (!seen.insert(s.id).second) throw DataLoadException("paradero repetido: " + s.id);
        net->addStop(StopFactory::create(s.type, s.id, s.name, Position{s.x, s.y}, s.capacity));
    }

    seen.clear();
    for (const auto& r : data_.routes) {
        if (!seen.insert(r.id).second) throw DataLoadException("ruta repetida: " + r.id);
        if (r.stops.size() < 2) throw InvalidRouteException(r.id + " necesita al menos 2 paraderos");
        auto route = std::make_shared<Route>(r.id, r.name, parseKind(r.kind), r.color);
        for (const auto& sid : r.stops) {
            auto stop = net->stops().byId(sid);
            if (!stop) throw InvalidRouteException(r.id + " usa un paradero inexistente: " + sid);
            route->addStop(stop);
        }
        net->addRoute(route);
    }

    for (const auto& f : data_.fleet)
        net->addVehicle(VehicleFactory::parse(f.vehicleType), f.routeId, f.startIndex);

    net->drainLog();                              // el armado inicial no cuenta como "evento"
    return net;
}

} // namespace sit
