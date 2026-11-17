#include "model/Route.h"

namespace sit {

double Route::length() const {
    double total = 0.0;
    for (std::size_t i = 1; i < stops_.size(); ++i)
        total += stops_[i - 1]->position().distanceTo(stops_[i]->position());
    return total;
}

std::vector<GeoCoordinate> Route::waypoints() const {
    std::vector<GeoCoordinate> out;
    out.reserve(stops_.size());
    for (const auto& stop : stops_) out.push_back(stop->geoPosition());
    return out;
}

bool Route::hasGeo() const {
    if (stops_.empty()) return false;
    for (const auto& stop : stops_)
        if (!stop->hasGeo()) return false;
    return true;
}

double Route::geoLength() const {
    double total = 0.0;
    for (std::size_t i = 1; i < stops_.size(); ++i)
        total += stops_[i - 1]->geoPosition().distanceTo(stops_[i]->geoPosition());
    return total;
}

} // namespace sit
