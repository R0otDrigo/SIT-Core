#include "model/Vehicle.h"
#include <algorithm>
#include <cmath>
#include "common/Exceptions.h"

namespace sit {

void Vehicle::board(int n) {
    if (n < 0 || passengers_ + n > capacity()) throw CapacityExceededException();
    passengers_ += n;
}

int Vehicle::alight(int n) {
    const int out = std::clamp(n, 0, passengers_);
    passengers_ -= out;
    return out;
}

void Vehicle::moveTowards(const Position& target, double dt) {
    const Position d = target - pos_;
    const double dist = std::hypot(d.x, d.y);
    const double step = speed() * dt;
    if (dist <= step || dist == 0.0) { pos_ = target; return; }
    pos_ = pos_ + Position{d.x / dist * step, d.y / dist * step};
}

void Vehicle::assignRoute(const std::shared_ptr<Route>& route, std::size_t startIndex) {
    if (!route || route->size() < 2) throw InvalidRouteException("una ruta necesita al menos 2 paraderos");
    route_ = route;
    const long n = static_cast<long>(route->size());
    const long start = std::min<long>(static_cast<long>(startIndex), n - 1);
    pos_ = route->stops()[static_cast<std::size_t>(start)]->position();
    dir_ = (start + 1 >= n) ? -1 : 1;
    index_ = static_cast<std::size_t>(start + dir_);
    state_ = VehicleState::Travelling;
    dwell_ = 0.0;
}

std::shared_ptr<Stop> Vehicle::targetStop() const {
    auto r = route_.lock();
    if (!r || index_ >= r->size()) return nullptr;
    return r->stops()[index_];
}

bool Vehicle::atRouteEnd() const {
    auto r = route_.lock();
    return r && (index_ == 0 || index_ + 1 == r->size());
}

void Vehicle::advanceIndex(std::size_t routeSize) {
    long next = static_cast<long>(index_) + dir_;
    if (next < 0 || next >= static_cast<long>(routeSize)) {
        dir_ = -dir_;                       // llego al extremo: da la vuelta
        next = static_cast<long>(index_) + dir_;
    }
    index_ = static_cast<std::size_t>(next);
}

bool Vehicle::update(double dt, double speedFactor) {
    auto r = route_.lock();
    if (!r || r->size() < 2) { state_ = VehicleState::Idle; return false; }

    if (state_ == VehicleState::Dwelling) {
        dwell_ -= dt;
        if (dwell_ <= 0.0) { advanceIndex(r->size()); state_ = VehicleState::Travelling; }
        return false;
    }

    state_ = VehicleState::Travelling;
    const Position target = r->stops()[index_]->position();
    moveTowards(target, dt * speedFactor);
    if (pos_ == target) {
        state_ = VehicleState::Dwelling;
        dwell_ = 0.0;                       // la Network lo ajusta con finishService()
        return true;
    }
    return false;
}

void Vehicle::finishService(double dwellSeconds) { dwell_ = std::max(0.0, dwellSeconds); }

std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    return os << v.typeName() << " #" << v.id() << " " << v.position()
              << " [" << v.passengers() << "/" << v.capacity() << "]";
}

} // namespace sit
