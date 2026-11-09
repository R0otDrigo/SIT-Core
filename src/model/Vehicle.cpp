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
    int out = std::clamp(n, 0, passengers_);
    passengers_ -= out;
    return out;
}

void Vehicle::moveTowards(const Position& target, double dt) {
    Position d = target - pos_;
    double dist = std::hypot(d.x, d.y);
    double step = speed() * dt;
    if (dist <= step || dist == 0.0) { pos_ = target; return; }
    pos_ = pos_ + Position{d.x / dist * step, d.y / dist * step};
}

std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    return os << v.typeName() << " #" << v.id() << " " << v.position()
              << " [" << v.passengers() << "/" << v.capacity() << "]";
}

} // namespace sit
