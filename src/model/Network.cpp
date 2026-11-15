#include "model/Network.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include "common/Exceptions.h"

namespace sit {

bool Network::compatible(VehicleType type, RouteKind kind) {
    return (type == VehicleType::Feeder) == (kind == RouteKind::Feeder);
}

Vehicle& Network::addVehicle(VehicleType type, const std::string& routeId, std::size_t startIndex) {
    auto route = routes_.byId(routeId);
    if (!route) throw InvalidRouteException("la ruta '" + routeId + "' no existe");
    if (!compatible(type, route->kind()))
        throw InvalidRouteException("un bus " + VehicleFactory::label(type) + " no puede operar en " + route->name());
    if (activeVehicles() >= fleetLimit_) throw FleetLimitExceededException(fleetLimit_);

    auto bus = VehicleFactory::create(type, nextVehicleId_++);
    bus->assignRoute(route, startIndex);
    vehicles_.push_back(std::move(bus));
    Vehicle& ref = *vehicles_.back();
    log("Despacho: bus #" + std::to_string(ref.id()) + " (" + ref.typeName() + ") en " + route->name());
    return ref;
}

bool Network::retireVehicle(const std::string& routeId) {
    auto it = std::find_if(vehicles_.begin(), vehicles_.end(), [&](const std::unique_ptr<Vehicle>& v) {
        auto r = v->route();
        return r && r->id() == routeId && v->passengers() == 0;
    });
    if (it == vehicles_.end()) return false;
    log("Retiro: bus #" + std::to_string((*it)->id()) + " sale de servicio en " + routeId);
    vehicles_.erase(it);
    return true;
}

void Network::triggerEvent(EventType type, const std::string& routeId, const std::string& stopId) {
    auto route = routes_.byId(routeId);
    if (!route) throw InvalidRouteException("la ruta '" + routeId + "' no existe");

    std::string stop = stopId;
    if (type == EventType::Protest) {
        if (stop.empty()) {                      // elige un paradero intermedio al azar
            std::uniform_int_distribution<std::size_t> pick(0, route->size() - 1);
            stop = route->stops()[pick(rng_)]->id();
        } else if (!stops_.byId(stop)) {
            throw InvalidEventException("el paradero '" + stop + "' no existe");
        }
    }
    auto event = TrafficEventFactory::create(type, routeId, stop);
    std::string where = route->name();
    if (!stop.empty()) where += " (paradero " + stop + ")";
    log("ALERTA: " + event->name() + " en " + where);
    events_.add(std::move(event));
}

double Network::speedFactorFor(const Route& route) const { return events_.speedFactorFor(route.id()); }

bool Network::isStopClosed(const Stop& stop) const { return events_.isStopClosed(stop); }

// 1 segundo simulado = 30 segundos de reloj: el reloj arranca a las 06:00.
static double hourOfDay(double simTime) { return std::fmod(6.0 + simTime * 30.0 / 3600.0, 24.0); }

double Network::demandMultiplier() const {
    const double h = hourOfDay(simTime_);
    auto bump = [&](double center, double width) {
        const double d = h - center;
        return std::exp(-(d * d) / (2.0 * width * width));
    };
    double m = 1.0 + 1.8 * bump(8.0, 0.9) + 1.8 * bump(18.0, 1.0);   // horas punta
    if (h < 6.0 || h >= 22.0) m *= 0.4;                               // noche
    return m;
}

std::string Network::clockString() const {
    const int minutes = static_cast<int>(hourOfDay(simTime_) * 60.0);
    char buf[16];
    std::snprintf(buf, sizeof buf, "%02d:%02d", minutes / 60, minutes % 60);
    return buf;
}

int Network::vehiclesOnRoute(const std::string& routeId) const {
    return static_cast<int>(std::count_if(vehicles_.begin(), vehicles_.end(), [&](const std::unique_ptr<Vehicle>& v) {
        auto r = v->route();
        return r && r->id() == routeId;
    }));
}

void Network::serve(Vehicle& v) {
    auto stop = v.targetStop();
    if (!stop) return;
    if (isStopClosed(*stop)) { v.finishService(0.0); return; }   // paradero cerrado: no se detiene

    const int wantOff = v.atRouteEnd() ? v.passengers()
                                       : static_cast<int>(std::ceil(v.passengers() * 0.35));
    const int off = v.alight(wantOff);
    transported_ += off;
    const int on = stop->takeWaiting(v.freeSeats());
    v.board(on);
    v.finishService(std::min(6.0, 1.0 + 0.04 * (off + on)));
}

void Network::step(double dt) {
    simTime_ += dt;

    // 1. Eventos de trafico: avanzan y expiran.
    for (const auto& msg : events_.tick(dt)) log(msg);

    // 2. Llegada de pasajeros a los paraderos (con variacion aleatoria).
    const double mult = demandMultiplier();
    std::uniform_real_distribution<double> jitter(0.6, 1.4);
    for (const auto& s : stops_.all()) abandoned_ += s->generate(dt, mult * jitter(rng_));

    // 3. Buses: avanzan y atienden paraderos.
    for (auto& v : vehicles_) {
        auto route = v->route();
        const double factor = route ? speedFactorFor(*route) : 1.0;
        if (v->update(dt, factor)) serve(*v);
    }
}

void Network::log(const std::string& message) {
    log_.push_back("[" + clockString() + "] " + message);
    if (log_.size() > 500) log_.erase(log_.begin(), log_.begin() + 250);
}

std::vector<std::string> Network::drainLog() {
    std::vector<std::string> out;
    out.swap(log_);
    return out;
}

Snapshot Network::snapshot() const {
    Snapshot s;
    s.simTime = simTime_;
    s.clock = clockString();

    double satSum = 0.0;
    for (const auto& st : stops_.all()) {
        StopView v;
        v.id = st->id(); v.name = st->name();
        v.x = st->position().x; v.y = st->position().y;
        v.waiting = st->waiting(); v.saturation = st->saturation();
        v.closed = isStopClosed(*st);
        satSum += st->saturation();
        s.stops.push_back(std::move(v));
    }
    for (const auto& r : routes_.all()) {
        RouteView v;
        v.id = r->id(); v.name = r->name(); v.color = r->color();
        v.trunk = r->kind() == RouteKind::Trunk;
        for (const auto& st : r->stops()) v.stopIds.push_back(st->id());
        s.routes.push_back(std::move(v));
    }
    for (const auto& b : vehicles_) {
        VehicleView v;
        v.id = b->id(); v.type = b->typeName();
        auto r = b->route();
        v.routeId = r ? r->id() : "";
        v.x = b->position().x; v.y = b->position().y;
        v.passengers = b->passengers(); v.capacity = b->capacity();
        v.dwelling = b->state() == VehicleState::Dwelling;
        s.vehicles.push_back(std::move(v));
    }
    for (const auto& e : events_.all()) s.events.push_back({e->name(), e->routeId(), e->stopId(), e->remaining()});

    s.stats.transported = transported_;
    s.stats.abandoned = abandoned_;
    s.stats.activeVehicles = activeVehicles();
    s.stats.fleetLimit = fleetLimit_;
    s.stats.avgSaturation = stops_.empty() ? 0.0 : satSum / static_cast<double>(stops_.size());
    return s;
}

} // namespace sit
